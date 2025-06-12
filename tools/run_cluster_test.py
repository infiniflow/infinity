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

    args = parser.parse_args()
    infinity_path = args.infinity_path
    use_sudo = args.use_sudo

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
        "--tb=short",
        test_case,
        f"--infinity_path={infinity_path}",
        "-x",
        "-s",
        "-m",
        "not slow",
    ]
    if use_sudo:
        cmd.append("--use_sudo")
    process = subprocess.Popen(cmd)
    process.wait()
