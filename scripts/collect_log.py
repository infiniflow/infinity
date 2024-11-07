import os
import shutil
import random
import string
import argparse

parser = argparse.ArgumentParser(description="Collect and copy log files.")
parser.add_argument(
    "--log_path", type=str, required=True, help="Path to the infinity log file"
)
parser.add_argument(
    "--stdout_path", type=str, required=True, help="Path to the stdout debug log file"
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
parser.add_argument("--failure", type=str, required=True, help="If the test failured")
args = parser.parse_args()

log_path = args.log_path
stdout_path = args.stdout_path
stderror_path = args.stderror_path
executable_path = args.executable_path
output_dir = args.output_dir
failure = args.failure == "true" or args.failure == "True"

if not os.path.isdir(output_dir):
    os.makedirs(output_dir)

if failure:
    random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
    print(f"Random log file name: {random_name}")

show_lines = 1000

if not os.path.isfile(stdout_path):
    print("Error: stdout file not found")
else:
    if failure:
        shutil.copy(stdout_path, f"{output_dir}/{random_name}_stdout.log")
    print(f"Last {show_lines} lines from {stdout_path}:")
    with open(stdout_path, "r") as f:
        lines = f.readlines()
        for line in lines[-show_lines:]:
            print(line.strip())

if not os.path.isfile(stderror_path):
    print("Error: stderror file not found")
else:
    if failure:
        shutil.copy(stderror_path, f"{output_dir}/{random_name}_stderror.log")
    print(f"Last {show_lines} lines from {stderror_path}:")
    with open(stderror_path, "r") as f:
        lines = f.readlines()
        for line in lines[-show_lines:]:
            print(line.strip())

if not os.path.isfile(log_path):
    print("Error: /var/infinity/log/infinity.log not found")
else:
    if failure:
        shutil.copy(log_path, f"{output_dir}/{random_name}.log")
    print(f"Last {show_lines} lines from {log_path}:")
    with open(log_path, "r") as f:
        lines = f.readlines()
        for line in lines[-show_lines:]:
            print(line.strip())

if not os.path.isfile(executable_path):
    print("Error: Executable file not found")
else:
    if failure:
        shutil.copy(executable_path, f"{output_dir}/{random_name}.exe")

run_parallel_test_log = "run_parallel_test.log"

if not os.path.isfile(run_parallel_test_log):
    print("Error: run_parallel_test log file not found")
else:
    if failure:
        shutil.copy(run_parallel_test_log, f"{output_dir}/{random_name}_{run_parallel_test_log}")
