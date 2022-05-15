import tarfile
import urllib.parse
import requests
import tempfile
import shutil
import os
import sys
import multiprocessing
import subprocess
import itertools
import config
from typing import List


def download_archive(url: str) -> str:
    """
    :param url: url of .tar.gz file to download and unpack
    :return: temporary directory with unpacked tar. Caller is responsible for deleting the directory when done with it.
    """

    try:
        with requests.get(url, stream=True) as response:
            with tarfile.open(fileobj=response.raw, mode="r:gz") as tar:
                tempdir = tempfile.mkdtemp()
                tar.extractall(path=tempdir)
                return tempdir
    except Exception as e:
        print(f"Couldn't download archive {url}: ", e)
        if tempdir is not None:
            shutil.rmtree(tempdir)


# https://stackoverflow.com/questions/3207219/how-do-i-list-all-files-of-a-directory
def get_files_in_dir(dir_path: str) -> List[str]:
    return next(os.walk(dir_path), (None, None, []))[2]


def run_sat_solver(sat_solver_path: str, cnf_file: str) -> bool:
    stdout = subprocess.run([sat_solver_path, cnf_file], capture_output=True).stdout
    return True  # fixme


def run_tests_for_archive(archive_uri: str, pool: multiprocessing.Pool, sat_solver_path: str, expected_result: True):
    # TODO: start downloading next archive while executing sat solvers for this one.
    tempdir = download_archive(urllib.parse.urljoin(config.base_url, archive_uri))
    filenames = get_files_in_dir(tempdir)
    file_paths = map(lambda x: os.path.join(tempdir, x), filenames)
    results = pool.starmap(run_sat_solver, zip(itertools.repeat(sat_solver_path), file_paths))
    shutil.rmtree(tempdir)

    for result, filename in zip(results, filenames):
        if result != expected_result:
            expected_text = "SATISFIABLE" if expected_result else "UNSATISFIABLE"
            result_text = "SATISFIABLE" if result else "UNSATISFIABLE"
            print(f"Wrong result for file {filename} from {archive_uri}:\n"
                  f"Expected {expected_text}, got {result_text}", file=sys.stderr)


def run_tests(sat_solver_path: str):
    pool = multiprocessing.Pool(8)
    for archive in config.satisfiable_cnf_archives:
        run_tests_for_archive(archive, pool, sat_solver_path, True)

    for archive in config.unsatisfiable_cnf_archives:
        run_tests_for_archive(archive, pool, sat_solver_path, False)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./test_runner.py path_to_sat_solver")
    run_tests(sys.argv[1])