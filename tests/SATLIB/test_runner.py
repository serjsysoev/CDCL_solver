#! /usr/bin/env python3

import queue
import tarfile
import time
import urllib.parse
import requests
import tempfile
import os
import sys
import threading
import subprocess
import re
import config
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
                filename = get_filename(response)
                with tarfile.open(fileobj=response.raw, mode="r:gz") as tar:
                    tar_dir = os.path.join(destination, filename)
                    os.mkdir(tar_dir)
                    tar.extractall(path=tar_dir)
                    return tar_dir
        except Exception as e:
            if attempts == DOWNLOAD_ATTEMPTS_MAX:
                message = f"Couldn't download archive {url}: " + str(e)
            else:
                message = f"Error while downloading {url}, retrying..."
            test_progressbar.write(message, file=sys.stderr)
            attempts += 1
            time.sleep(0.5 * attempts)


# https://stackoverflow.com/questions/3207219/how-do-i-list-all-files-of-a-directory
def get_files_in_dir(dir_path: str) -> List[str]:
    return next(os.walk(dir_path), (None, None, []))[2]


def run_sat_solver(test_dir_path: str, test_filename: str, sat_solver_path: str, expected_result: bool):
    cnf_file = os.path.join(test_dir_path, test_filename)
    process = subprocess.run([sat_solver_path, cnf_file], capture_output=True)
    stdout = process.stdout
    stdout = stdout.decode("utf-8")
    lines = stdout.splitlines()
    if len(lines) == 0 or lines[0] not in ["SATISFIABLE", "UNSATISFIABLE"]:
        stderr = process.stderr.decode("utf-8")
        output = textwrap.dedent(f"""
        Invalid output format on {cnf_file}
        stdout:
        {stdout}
        stderr:
        {stderr}
        """)
        test_progressbar.write(output, file=sys.stderr)
    else:
        result = lines[0] == "SATISFIABLE"
        if result != expected_result:
            expected_text = "SATISFIABLE" if expected_result else "UNSATISFIABLE"
            result_text = "SATISFIABLE" if result else "UNSATISFIABLE"
            archive_name = os.path.basename(test_dir_path)
            test_progressbar.write(f"Wrong result for file {test_filename} from {archive_name}:\n"
                                   f"Expected {expected_text}, got {result_text}", file=sys.stderr)
    test_progressbar.update(1)


task_queue = queue.Queue()


def consume_task_queue():
    while True:
        task = task_queue.get()
        task()
        task_queue.task_done()


def download_tests_and_queue(archive_uri: str, tempdir: str, sat_solver_path: str, expected_result: bool):
    path = download_archive(urllib.parse.urljoin(config.base_url, archive_uri), tempdir)
    filenames = get_files_in_dir(path)
    test_progressbar.total += len(filenames)
    test_progressbar.refresh()
    task_queue.put(lambda: test_progressbar.write(f"Processing archive {archive_uri}..."))
    for filename in filenames:
        task_queue.put(lambda: run_sat_solver(path, filename, sat_solver_path, expected_result))


def run_tests(sat_solver_path: str):
    for i in range(THREAD_POOL_SIZE):
        threading.Thread(target=consume_task_queue).start()
    with tempfile.TemporaryDirectory() as tempdir:
        for archive in config.satisfiable_cnf_archives:
            download_tests_and_queue(archive, tempdir, sat_solver_path, True)
            download_progressbar.update(1)
            download_progressbar.write(f"Downloaded {archive}")

        for archive in config.unsatisfiable_cnf_archives:
            download_tests_and_queue(archive, tempdir, sat_solver_path, False)
            download_progressbar.update(1)
            download_progressbar.write(f"Downloaded {archive}")

    # poison pills
    for i in range(THREAD_POOL_SIZE):
        task_queue.put(lambda: sys.exit(0))


def get_default_bar_format() -> str:
    l_bar = "{desc}: {percentage:3.0f}%|"
    r_bar = "| {n_fmt:>5}/{total_fmt:>5}"
    return f"{l_bar}{{bar}}{r_bar}"


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./test_runner.py path_to_sat_solver")

    archives_count = len(config.satisfiable_cnf_archives + config.unsatisfiable_cnf_archives)
    download_progressbar_desc = "Test archives downloaded"
    test_progressbar_desc = "Tests processed"
    diff = len(download_progressbar_desc) - len(test_progressbar_desc)
    download_progressbar = tqdm.tqdm(total=archives_count, position=0, bar_format=get_default_bar_format(),
                                     desc=download_progressbar_desc + " " * max(0, -diff))
    test_progressbar = tqdm.tqdm(total=0, position=1, bar_format=get_default_bar_format(),
                                 desc=test_progressbar_desc + " " * max(0, diff))

    run_tests(sys.argv[1])
