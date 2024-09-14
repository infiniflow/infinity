import subprocess
import argparse
import sys
import os

python_executable = sys.executable


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python restart test for infinity")
    parser.add_argument(
        "--infinity_path",
        type=str,
        default="./build/Debug/src/infinity",
    )

    args = parser.parse_args()
    infinity_path = args.infinity_path

    current_path = os.getcwd()
    python_test_dir = current_path + "/python"

    process = subprocess.Popen(
        [
            python_executable,
            "-m",
            "pytest",
            # "--capture=tee-sys",
            f"{python_test_dir}/restart_test",
            f"--infinity_path={infinity_path}",
            "-x",
            "-s",
            "-m",
            "not slow"
        ]
    )
    process.wait()
    if process.returncode != 0:
        raise Exception(f"An error occurred: {process.stderr}")