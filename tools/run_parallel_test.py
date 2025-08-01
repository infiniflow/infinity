import argparse
import os
import subprocess
import sys
import time

python_executable = sys.executable

def python_sdk_test(python_test_dir: str, pytest_mark: str):
    print("python test path is {}".format(python_test_dir))
    # run test
    print(f"start pysdk test with {pytest_mark}")
    args = [
        python_executable,
        "-m",
        "pytest",
        "-W",
        "error",
        # "--capture=tee-sys",
        "--tb=short",
        "-x",
        "-m",
        pytest_mark,
        f"{python_test_dir}/parallel_test",
    ]
    quoted_args = ['"' + arg + '"' if " " in arg else arg for arg in args]
    print(" ".join(quoted_args))

    process = subprocess.Popen(
        args,
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
