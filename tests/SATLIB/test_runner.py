#! /usr/bin/env python3

import argparse
import queue
import tarfile
import time
import urllib.parse
from pathlib import Path
import requests
import tempfile
import os
import sys
import threading
import subprocess
import re
import config
import config_ci
import textwrap
import tqdm
from typing import List

THREAD_POOL_SIZE = 50
DOWNLOAD_ATTEMPTS_MAX = 3


# https://stackoverflow.com/questions/31804799/how-to-get-pdf-filename-with-python-requests
def get_filename(response: requests.Response):
    if 'content-disposition' in response.headers:
        d = response.headers['content-disposition']
        return re.findall("filename=(.+)", d)[0]
    else:
        return response.url.rsplit('/', 1)[-1]


def download_archive(url: str, destination: str) -> str:
    """
    :param url: url of .tar.gz file to download and unpack
    :param destination: path to destination folder
    :return: temporary directory with unpacked tar. Caller is responsible for deleting the directory when done with it.
    """

    attempts = 0
    while attempts < DOWNLOAD_ATTEMPTS_MAX:
        try:
            with requests.get(url, stream=True) as response:
                filename = get_filename(response).split('.')[0]
                with tarfile.open(fileobj=response.raw, mode="r:gz") as tar:
                    tar_dir = os.path.join(destination, filename)
                    os.mkdir(tar_dir)
                    def is_within_directory(directory, target):
                        
                        abs_directory = os.path.abspath(directory)
                        abs_target = os.path.abspath(target)
                    
                        prefix = os.path.commonprefix([abs_directory, abs_target])
                        
                        return prefix == abs_directory
                    
                    def safe_extract(tar, path=".", members=None, *, numeric_owner=False):
                    
                        for member in tar.getmembers():
                            member_path = os.path.join(path, member.name)
                            if not is_within_directory(path, member_path):
                                raise Exception("Attempted Path Traversal in Tar File")
                    
                        tar.extractall(path, members, numeric_owner=numeric_owner) 
                        
                    
                    safe_extract(tar, path=tar_dir)
                    return tar_dir
        except Exception as e:
            if attempts == DOWNLOAD_ATTEMPTS_MAX:
                message = f"Couldn't download archive {url}: " + str(e)
            else:
                message = f"Error while downloading {url}, retrying..."
            test_progressbar.write(message, file=sys.stderr)
            attempts += 1
            time.sleep(0.5 * attempts)


# https://stackoverflow.com/questions/18394147/how-to-do-a-recursive-sub-folder-search-and-return-files-in-a-list
def get_files_in_dir_recursively(dir_path: str) -> List[Path]:
    return list(Path(dir_path).rglob("*.[cC][nN][fF]"))


def test_assignment(cnf_path: Path, assignment: str) -> bool:
    with open(cnf_path, "r") as f:
        lines = f.readlines()
    lines = list(filter(lambda line: not line.startswith('c'), lines))
    p, cnf, vars, clauses = lines[0].split()
    if p != 'p' or cnf != 'cnf':
        return False
    vars, clauses_count = int(vars), int(clauses)
    clauses = ' '.join(lines[1:]).split()
    clauses = ' '.join(clauses).split(' 0')[:clauses_count]
    assignment = [not x.startswith("-") for x in assignment.split()]
    for clause in clauses:
        clause = [int(x) for x in clause.split()]
        result = False
        for literal in clause:
            value = assignment[abs(literal) - 1]
            if literal < 0:
                value = not value
            result |= value
        if not result:
            return False
    return True


def run_sat_solver(test_path: Path, sat_solver_path: str, expected_result: bool):
    process = subprocess.run([sat_solver_path, str(test_path)], capture_output=True)
    stdout = process.stdout
    stdout = stdout.decode("utf-8")
    lines = stdout.splitlines()

    if len(lines) == 0 or lines[0] not in ["SATISFIABLE", "UNSATISFIABLE"]:
        stderr = process.stderr.decode("utf-8")
        output = textwrap.dedent(f"""
        Invalid output format on {test_path.name}
        stdout:
        {stdout}
        stderr:
        {stderr}
        """)
        test_progressbar.write(output, file=sys.stderr)
    else:
        result = lines[0] == "SATISFIABLE"
        archive_name = os.path.basename(test_path.parent)
        if result != expected_result:
            expected_text = "SATISFIABLE" if expected_result else "UNSATISFIABLE"
            result_text = "SATISFIABLE" if result else "UNSATISFIABLE"
            test_progressbar.write(f"Wrong result for file {test_path.name} from {archive_name}:\n"
                                   f"Expected {expected_text}, got {result_text}", file=sys.stderr)
        elif lines[0] == "SATISFIABLE" and not test_assignment(test_path, lines[1]):
            test_progressbar.write(f"Wrong assignment for file {test_path.name} from {archive_name}\n"
                                   f"Got {lines[1]}")

    test_progressbar.update(1)


task_queue = queue.Queue()


def consume_task_queue():
    while True:
        task = task_queue.get()
        task()
        task_queue.task_done()


def download_tests_and_queue(archive_uri: str, tempdir: str, sat_solver_path: str, expected_result: bool,
                             is_config_for_ci: bool):
    path = download_archive(
        urllib.parse.urljoin(config_ci.base_url if is_config_for_ci else config.base_url, archive_uri), tempdir)
    paths = get_files_in_dir_recursively(path)
    test_progressbar.total += len(paths)
    test_progressbar.refresh()
    task_queue.put(lambda uri=archive_uri: test_progressbar.write(f"Processing archive {uri}..."))
    for test_path in paths:
        task_queue.put(lambda path_copy=test_path: run_sat_solver(path_copy, sat_solver_path, expected_result))


def run_tests(sat_solver_path: str, is_config_for_ci: bool):
    for i in range(THREAD_POOL_SIZE):
        threading.Thread(target=consume_task_queue).start()
    with tempfile.TemporaryDirectory() as tempdir:
        for archive in (config_ci.satisfiable_cnf_archives if is_config_for_ci else config.satisfiable_cnf_archives):
            download_tests_and_queue(archive, tempdir, sat_solver_path, True, is_config_for_ci)
            download_progressbar.update(1)
            download_progressbar.write(f"Downloaded {archive}")

        for archive in (config_ci.unsatisfiable_cnf_archives if is_config_for_ci else config.unsatisfiable_cnf_archives):
            download_tests_and_queue(archive, tempdir, sat_solver_path, False, is_config_for_ci)
            download_progressbar.update(1)
            download_progressbar.write(f"Downloaded {archive}")

        task_queue.join()

    # poison pills
    for i in range(THREAD_POOL_SIZE):
        task_queue.put(lambda: sys.exit(0))


def get_default_bar_format() -> str:
    l_bar = "{desc}: {percentage:3.0f}%|"
    r_bar = "| {n_fmt:>5}/{total_fmt:>5}"
    return f"{l_bar}{{bar}}{r_bar}"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("path_to_exe", help="executable file path")
    parser.add_argument("-c", action='store_true',
                        help="config type: default or ci version(not to spend much time in ci version)")

    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    archives_count = len(config_ci.satisfiable_cnf_archives) if args.c else len(config.satisfiable_cnf_archives) + \
                        len(config_ci.unsatisfiable_cnf_archives) if args.c else len(config.unsatisfiable_cnf_archives)
    download_progressbar_desc = "Test archives downloaded"
    test_progressbar_desc = "Tests processed"
    diff = len(download_progressbar_desc) - len(test_progressbar_desc)
    download_progressbar = tqdm.tqdm(total=archives_count, position=0, bar_format=get_default_bar_format(),
                                     desc=download_progressbar_desc + " " * max(0, -diff))
    test_progressbar = tqdm.tqdm(total=0, position=1, bar_format=get_default_bar_format(),
                                 desc=test_progressbar_desc + " " * max(0, diff))

    run_tests(args.path_to_exe, args.c)
