import os
import shutil
import random
import string
import argparse
import collect_thread_sanitizer_log


def collect_log(
        log_path,
        stdout_path,
        stderror_path,
        executable_path,
        output_dir,
        failure,
        show_lines,
        run_id,
        run_attempt,
):
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    name_prefix = f'{run_id}_{run_attempt}'
    print(f"log file name prefix: {name_prefix}")

    if not os.path.isfile(stdout_path):
        print("Error: stdout file not found")
    else:
        if failure:
            shutil.copy(stdout_path, f"{output_dir}/{name_prefix}_stdout.log")
        print(f"Last {show_lines} lines from {stdout_path}:")
        with open(stdout_path, "r", errors="ignore") as f:
            lines = f.readlines()
            for line in lines[-show_lines:]:
                print(line.strip())

    if not os.path.isfile(stderror_path):
        print("Error: stderror file not found")
    else:
        if failure:
            shutil.copy(stderror_path, f"{output_dir}/{name_prefix}_stderror.log")
        print(f"Last {show_lines} lines from {stderror_path}:")
        with open(stderror_path, "r", errors="ignore") as f:
            lines = f.readlines()
            for line in lines[-show_lines:]:
                print(line.strip())

    if not os.path.isfile(log_path):
        print("Error: /var/infinity/log/infinity.log not found")
    else:
        if failure:
            shutil.copy(log_path, f"{output_dir}/{name_prefix}.log")
        print(f"Last {show_lines} lines from {log_path}:")
        with open(log_path, "r", errors="ignore") as f:
            lines = f.readlines()
            for line in lines[-show_lines:]:
                print(line.strip())

    if not os.path.isfile(executable_path):
        print("Error: Executable file not found")
    else:
        if failure:
            shutil.copy(executable_path, f"{output_dir}/{name_prefix}.exe")

    run_parallel_test_log = "run_parallel_test.log"

    if not os.path.isfile(run_parallel_test_log):
        print("Error: run_parallel_test log file not found")
    elif failure:
        shutil.copy(
            run_parallel_test_log, f"{output_dir}/{name_prefix}_{run_parallel_test_log}"
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Collect and copy log files.")
    parser.add_argument(
        "--run_id",
        type=str,
        required=True,
        help="A unique number for each workflow run within a repository. This number does not change if you re-run the workflow run.",
        default=None
    )
    parser.add_argument(
        "--run_attempt",
        type=str,
        required=True,
        help="A unique number for each attempt of a particular workflow run in a repository. This number begins at 1 for the workflow run's first attempt, and increments with each re-run.",
        default=None
    )
    parser.add_argument(
        "--log_path", type=str, required=True, help="Path to the infinity log file"
    )
    parser.add_argument(
        "--stdout_path",
        type=str,
        required=True,
        help="Path to the stdout debug log file",
    )
    parser.add_argument(
        "--stderror_path",
        type=str,
        required=True,
        help="Path to the stderror debug log file",
    )
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

    run_id = args.run_id
    run_attempt = args.run_attempt
    log_path = args.log_path
    stdout_path = args.stdout_path
    stderror_path = args.stderror_path
    executable_path = args.executable_path
    output_dir = args.output_dir
    failure = args.failure == "true" or args.failure == "True"
    tsan_log = args.tsan_log
    show_lines = 1000

    if not failure and tsan_log is not None:
        collect_thread_sanitizer_log.collect_log(tsan_log, output_dir, show_lines)
    else:
        collect_log(
            log_path,
            stdout_path,
            stderror_path,
            executable_path,
            output_dir,
            failure,
            show_lines,
            run_id,
            run_attempt,
        )
