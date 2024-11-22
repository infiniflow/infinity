import argparse
import os
import random
import shutil
import string
import collect_thread_sanitizer_log


def collect_log(executable_path, log_dir, output_dir, failure):
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    if failure:
        random_name = "".join(
            random.choices(string.ascii_lowercase + string.digits, k=8)
        )
        print(f"Random log file name: {random_name}")

    show_lines = 100

    log_files = []
    for root, dirs, files in os.walk(log_dir):
        for file in files:
            if file == "infinity.log":
                log_files.append(os.path.join(root, file))
    print(f"Found log files: {log_files}")

    for log_file in log_files:
        filename = log_file.replace("/", "_")
        if failure:
            shutil.copy(log_file, f"{output_dir}/{random_name}{filename}")
        print(f"Last {show_lines} lines from {log_file}:")
        with open(log_file, "r") as f:
            lines = f.readlines()
            for line in lines[-show_lines:]:
                print(line.strip())

    if not os.path.isfile(executable_path):
        print(f"Executable file {executable_path} does not exist")
    else:
        if failure:
            shutil.copy(executable_path, f"{output_dir}/{random_name}.exe")

    py_log_files: list[str] = []
    for file in os.listdir("./"):
        if file.endswith(".log"):
            py_log_files.append(file)
    print(f"Found py log files with .log suffix: {py_log_files}")
    if failure:
        for py_log_file in py_log_files:
            shutil.copy(py_log_file, f"{output_dir}/{random_name}_{py_log_file}")



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python cluster test for infinity")
    parser.add_argument(
        "--executable_path", type=str, required=True, help="infinity executable path"
    )
    parser.add_argument(
        "--log_dir",
        type=str,
        required=True,
        help="Path to dir that may contain log files",
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
    log_dir = args.log_dir
    output_dir = args.output_dir
    failure = args.failure == "true" or args.failure == "True"
    tsan_log = args.tsan_log
    show_lines = 1000

    if not failure and tsan_log is not None:
        collect_thread_sanitizer_log.collect_log(tsan_log, output_dir, show_lines)
    else:
        collect_log(executable_path, log_dir, output_dir, failure)
