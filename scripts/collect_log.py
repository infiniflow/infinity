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
    "--output_dir",
    type=str,
    default="/var/infinity/log",
    help="Path to the output directory",
)
args = parser.parse_args()

log_path = args.log_path
stdout_path = args.stdout_path
output_dir = args.output_dir

if not os.path.isfile(stdout_path):
    print("Error: stdout file not found")
    with open(stdout_path, "w") as f:
        f.write("")

if not os.path.isfile(log_path):
    print("Error: /var/infinity/log/infinity.log not found")
    with open(log_path, "w") as f:
        f.write("")

if not os.path.isdir(output_dir):
    os.makedirs(output_dir)


random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
print(f"Random log file name: {random_name}")

shutil.copy(stdout_path, f"{output_dir}/{random_name}_.log")
shutil.copy(log_path, f"{output_dir}/{random_name}.log")
