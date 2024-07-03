#! /usr/bin/env python3
import argparse
import os
import subprocess
import sys
import time
import shutil

python_executable = sys.executable


def python_sdk_test(python_test_dir: str, pytest_mark: str):
    print("python test path is {}".format(python_test_dir))
    # run test
    print(f"start pysdk test with {pytest_mark}")
    i = 0
    begin_time = time.time()
    while time.time() - begin_time < 8 * 3600:
        process = subprocess.Popen(
            # ["python", "-m", "pytest", "--tb=line", '-s', '-x', '-m', pytest_mark, f'{python_test_dir}/test'],
            [
                python_executable,
                "-m",
                "pytest",
                "--tb=line",
                "-x",
                "-m",
                pytest_mark,
                f"{python_test_dir}/parallel_test",
            ],
            stdout=sys.stdout,
            stderr=sys.stderr,
            universal_newlines=True,
        )
        process.wait()
        if process.returncode != 0:
            raise Exception(f"An error occurred: {process.stderr}")
        print(f"iteration {i} complete")
        i += 1
        directory = "/var/infinity"
        folder_size = shutil.disk_usage(directory)
        print(f"{directory} size: {folder_size.used / (1024 * 1024)} MB")

    print("pysdk test finished.")


if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")
    current_path = os.getcwd()
    python_test_dir = current_path + "/python/"
    parser = argparse.ArgumentParser(description="Python SDK Test For Infinity")
    parser.add_argument(
        "-m",
        "--pytest_mark",
        type=str,
        default="not complex and not slow",
        dest="pytest_mark",
    )
    args = parser.parse_args()

    print("Start Python SDK testing...")
    start = time.time()
    try:
        python_sdk_test(python_test_dir, args.pytest_mark)
    except Exception as e:
        print(e)
        sys.exit(-1)
    end = time.time()
    print("Test finished.")
    print("Time cost: {}s".format(end - start))
