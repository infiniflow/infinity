import subprocess
import argparse
import sys
import os

python_executable = sys.executable

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python restart test for infinity")
    parser.add_argument("--infinity_path", type=str, default="./build/Debug/src/infinity")
    parser.add_argument("--slow", type=bool, default=False)
    parser.add_argument("--test_case", type=str, required=False)
    parser.add_argument("--loop", type=int, required=False)
    args = parser.parse_args()
    infinity_path = args.infinity_path
    slow = args.slow

    current_path = os.getcwd()
    python_test_dir = current_path + "/python"
    test_case = ""
    if args.test_case:
        test_case = f"{python_test_dir}/restart_test/{args.test_case}"
    else:
        test_case = f"{python_test_dir}/restart_test"

    loop = 1
    if args.loop:
        loop = args.loop

    for idx in range(loop):
        print(f"Running test case: {test_case}, loop: {idx + 1}/{loop}")
        if not slow:
            process = subprocess.Popen(
                [
                    python_executable,
                    "-m",
                    "pytest",
                    "-v",
                    "--tb=short",
                    test_case,
                    f"--infinity_path={infinity_path}",
                    "-x",
                    "-s",
                    "-m",
                    "not slow",
                    # "-W",
                    # "error",
                    # "-k",
                    # "test_optimize_from_different_database"
                ]
            )
        else:
            process = subprocess.Popen(
                [
                    python_executable,
                    "-m",
                    "pytest",
                    "-v",
                    "--tb=short",
                    test_case,
                    f"--infinity_path={infinity_path}",
                    "-x",
                    "-s",
                ]
            )
        process.wait()
        if process.returncode != 0:
            print(f"An error occurred: {process.stderr}")
            sys.exit(-1)
