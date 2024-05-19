import argparse
import itertools
import os
import subprocess
import sys
import threading
import time


def python_sdk_test(python_test_dir: str, pytest_mark: str):
    print("python test path is {}".format(python_test_dir))
    # run test
    print(f"start pysdk test with {pytest_mark}")
    process = subprocess.Popen(
        # ["python", "-m", "pytest", "--tb=line", '-s', '-x', '-m', pytest_mark, f'{python_test_dir}/test'],
        ["python", "-m", "pytest", "--tb=line", '-x', '-m', pytest_mark, f'{python_test_dir}'],
        stdout=sys.stdout,
        stderr=sys.stderr,
        universal_newlines=True,
    )
    process.wait()
    if process.returncode != 0:
        raise Exception(f"An error occurred: {process.stderr}")

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
        default='skipif',
        dest="pytest_mark",
    )
    args = parser.parse_args()

    print("Start Python SDK testing...")
    start = time.time()
    try:
        python_sdk_test(current_path + "/python/test", args.pytest_mark)
        python_sdk_test(current_path + "/python/test_http_api", args.pytest_mark)
    except Exception as e:
        print(e)
        sys.exit(-1)
    end = time.time()
    print("Test finished.")
    print("Time cost: {}s".format(end - start))