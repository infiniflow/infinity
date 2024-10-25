import argparse
import os
import random
import shutil
import string


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
parser.add_argument("--failure", type=str, required=True, help="If the test failured")
args = parser.parse_args()

executable_path = args.executable_path
log_dir = args.log_dir
output_dir = args.output_dir
failure = args.failure == "true" or args.failure == "True"

if not os.path.isdir(output_dir):
    os.makedirs(output_dir)

if failure:
    random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
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
        shutil.copy(log_file, f"{output_dir}/{random_name}_{filename}")
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
