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
    parser.add_argument(
        "--infinity_dir",
        type=str,
        required=True,
    )
    parser.add_argument(
        "--test_case",
        type=str,
        required=False,
    )
    parser.add_argument(
        "--use_sudo",
        action="store_true",
        default=False,
        help="Use sudo to run command",
    )
    parser.add_argument(
        "--minio_port",
        type=int,
        default=9000,
    )
    parser.add_argument(
        "--minio_console_port",
        type=int,
        default=9001,
    )

    args = parser.parse_args()
    infinity_path = args.infinity_path
    docker = args.docker
    infinity_dir = args.infinity_dir
    use_sudo = args.use_sudo
    minio_port = args.minio_port
    minio_console_port = args.minio_console_port

    current_path = os.getcwd()
    python_test_dir = current_path + "/python"

    test_case = None
    if args.test_case:
        test_case = f"{python_test_dir}/test_cluster/{args.test_case}"
    else:
        test_case = f"{python_test_dir}/test_cluster"

    cmd = [
        python_executable,
        "-m",
        "pytest",
        "-v",
        test_case,
        f"--infinity_path={infinity_path}",
        "-x",
        "-s",
        "-m",
        "not slow",
        f"--infinity_dir={infinity_dir}",
        f"--minio_port={minio_port}",
        f"--minio_console_port={minio_console_port}",
    ]
    if use_sudo:
        cmd.append("--use_sudo")
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
