import argparse
import os
import random
import string

filter_keywords = [
    # https://stackoverflow.com/questions/53917857/c-thrift-is-tthreadedserverstop-thread-safe
    "TServerSocket::interruptChildren",
    "TServerSocket::notify",
    "TServerSocket::interrupt",
    #
]

thread_sanitizer_delimiter = "=================="


def filter_warning(lines: list[str]):
    ret = []
    warning = []
    in_warning = False
    skip = False
    for line in lines:
        if thread_sanitizer_delimiter in line:
            in_warning = not in_warning
            if not skip:
                warning.append(line)
            if not in_warning:
                if not skip:
                    ret.extend(warning)
                warning = []
                skip = False
        elif in_warning:
            if skip:
                continue
            for keyword in filter_keywords:
                if keyword in line:
                    skip = True
                    warning.clear()
                    break
            if not skip:
                warning.append(line)
    return ret


def collect_log(tsan_log, output_dir, show_lines):
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
    print(f"Random log file name: {random_name}")
    target_path = os.path.join(output_dir, f"{random_name}_{tsan_log}")
    print(f"Target path: {target_path}")

    rets = []
    for root, dirs, files in os.walk("."):
        for file in files:
            if file.startswith(tsan_log):
                file_path = os.path.join(root, file)
                print(f"Processing log file: {file_path}")
                with open(file_path, "r") as f:
                    lines = f.readlines()
                    rets.extend(filter_warning(lines))

    with open(target_path, "w") as f2:
        f2.writelines(rets)

    print(f"Last {show_lines} lines from {target_path}:")
    with open(target_path, "r") as f:
        lines = f.readlines()
        for line in lines[-show_lines:]:
            print(line.strip())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect and copy log files.")
    parser.add_argument(
        "--tsan_log",
        type=str,
        required=True,
        help="Thread sanitizer log prefix",
    )
    parser.add_argument(
        "--output_dir",
        type=str,
        required=True,
        help="Path to the output directory",
    )
    args = parser.parse_args()
    show_lines = 1000
    collect_log(args.tsan_log, args.output_dir, show_lines)
