#! /usr/bin/env python3
import argparse
import os
import subprocess
import sys
import time

python_executable = sys.executable


def http_api_test(http_api_dir: str, pytest_mark: str):
    print(f"start http api test with {pytest_mark}")
    process = subprocess.Popen(
        # ["python", "-m", "pytest", "--tb=line", '-s', '-x', '-m', pytest_mark, f'{python_test_dir}/test_http_api'],
        [
            python_executable,
            "-m",
            "pytest",
            "--tb=line",
            "-x",
            "-m",
            pytest_mark,
            f"{python_test_dir}/test_http_api",
        ],
        stdout=sys.stdout,
        stderr=sys.stderr,
        universal_newlines=True,
    )
    process.wait()
    if process.returncode != 0:
        raise Exception(f"An error occurred: {process.stderr}")
    return


if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")
    current_path = os.getcwd()
    python_test_dir = current_path + "/python/"
    parser = argparse.ArgumentParser(description="HTTP API Test For Infinity")
    parser.add_argument(
        "-m",
        "--pytest_mark",
        type=str,
        default="not complex and not slow",
        dest="pytest_mark",
    )
    args = parser.parse_args()

    print("Start Python HTTP API testing...")
    start = time.time()
    try:
        http_api_test(python_test_dir, args.pytest_mark)
    except Exception as e:
        print(e)
        sys.exit(-1)
    end = time.time()
    print("Test finished.")
    print("Time cost: {}s".format(end - start))
