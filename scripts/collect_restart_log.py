import argparse
import os
import random
import re
import shutil
import string
import glob
import collect_thread_sanitizer_log

PYTEST_LOG_FILE = "restart_test.py.log"

def collect_log(executable_path, output_dir, failure, show_lines):
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    if failure:
        random_name = "".join(
            random.choices(string.ascii_lowercase + string.digits, k=8)
        )
        print(f"Random log file name: {random_name}")

    log_files = []
    for root, dirs, files in os.walk("."):
        for file in files:
            match = re.match(r"restart_test\.log\.(\d+)", file)
            if match:
                log_files.append((os.path.join(root, file), int(match.group(1))))

    log_files = sorted(log_files, key=lambda x: x[1])
    print(f"Found log files: {log_files}")

    for log_file, i in log_files:
        if failure:
            shutil.copy(log_file, f"{output_dir}/{random_name}_{i}.log")
        if i == len(log_files) - 1:
            print(f"Last {show_lines} lines from {log_file}:")
            with open(log_file, "r") as f:
                lines = f.readlines()
                for line in lines[-show_lines:]:
                    print(line.strip())

    if not os.path.isfile(executable_path):
        print("Error: Executable file not found")
    else:
        if failure:
            shutil.copy(executable_path, f"{output_dir}/{random_name}.exe")

    if failure:
        # copy file in /var/infinity/../FULL.*.json
        copy_n = 0
        for filepath in glob.iglob(f"/var/infinity/**/FULL.*.json", recursive=True):
            print(filepath)
            filename = filepath.split("/")[-1]
            shutil.copy(filepath, f"./{filename}")
            copy_n += 1
        if copy_n == 0:
            print("No FULL.*.json file found")
    
    if failure:
        # copy pytest log file
        if os.path.isfile(PYTEST_LOG_FILE):
            shutil.copy(PYTEST_LOG_FILE, f"{output_dir}/{random_name}_pytest.log")
        else:
            print(f"{PYTEST_LOG_FILE} not found")


if "__main__" == __name__:
    parser = argparse.ArgumentParser(description="Python restart test for infinity")
    parser.add_argument(
        "--executable_path",
        type=str,
        required=True,
        help="Path to the executable file",
    )
    parser.add_argument(
        "--output_dir",
        type=str,
        required=True,
        help="Path to the output directory",
    )
    parser.add_argument(
        "--failure", type=str, required=True, help="If the test failured"
    )
    parser.add_argument(
        "--tsan_log",
        type=str,
        required=False,
        help="Thread sanitizer log prefix",
        default=None,
    )
    args = parser.parse_args()

    executable_path = args.executable_path
    output_dir = args.output_dir
    failure = args.failure == "true" or args.failure == "True"
    tsan_log = args.tsan_log
    show_lines = 1000

    if not failure and tsan_log is not None:
        collect_thread_sanitizer_log.collect_log(tsan_log, output_dir, show_lines)
    else:
        collect_log(executable_path, output_dir, failure, show_lines)
