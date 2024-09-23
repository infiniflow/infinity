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
    "--stdout_path", type=str, required=True, help="Path to the output debug log file"
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
args = parser.parse_args()

log_path = args.log_path
stdout_path = args.stdout_path
executable_path = args.executable_path
output_dir = args.output_dir

if not os.path.isdir(output_dir):
    os.makedirs(output_dir)


random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
print(f"Random log file name: {random_name}")

if not os.path.isfile(stdout_path):
    print("Error: stdout file not found")
else:
    shutil.copy(stdout_path, f"{output_dir}/{random_name}_.log")

if not os.path.isfile(log_path):
    print("Error: /var/infinity/log/infinity.log not found")
else:
    shutil.copy(log_path, f"{output_dir}/{random_name}.log")

if not os.path.isfile(executable_path):
    print("Error: Executable file not found")
else:
    shutil.copy(executable_path, f"{output_dir}/{random_name}.exe")
