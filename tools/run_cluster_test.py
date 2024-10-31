import argparse
import os
import subprocess
import sys

python_executable = sys.executable

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python cluster test for infinity")
    parser.add_argument(
        "--infinity_path",
        type=str,
        default="./build/Debug/src/infinity",
    )
    parser.add_argument(
        "--docker",
        action="store_true",
        default=False,
    )

    args = parser.parse_args()
    infinity_path = args.infinity_path
    docker = args.docker

    current_path = os.getcwd()
    python_test_dir = current_path + "/python"

    cmd = [
        python_executable,
        "-m",
        "pytest",
        f"{python_test_dir}/test_cluster",
        f"--infinity_path={infinity_path}",
        "-x",
        "-s",
        "-m",
        "not slow",
    ]
    if docker:
        cmd.append("--docker")
    process = subprocess.Popen(cmd)
    process.wait()

    cmd = [
        python_executable,
        f"{python_test_dir}/test_cluster/clear_docker.py",
    ]
    if docker:
        cmd.append("--docker")
    process2 = subprocess.run(cmd)
    if process.returncode != 0:
        print(f"An error occurred: {process.stderr}")
        sys.exit(-1)
