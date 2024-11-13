import argparse
import os
import random
import shutil
import string

parser = argparse.ArgumentParser(description="Collect and copy embedded log files.")
parser.add_argument(
    "--output_dir", type=str, required=True, help="Path to the output directory"
)
parser.add_argument("--failure", type=str, required=True, help="If the test failured")
parser.add_argument(
    "--log_path", type=str, required=True, help="Path to the embedded log file"
)

args = parser.parse_args()
output_dir = args.output_dir
failure = args.failure == "true" or args.failure == "True"
log_path = args.log_path
show_lines = 1000

print(f"Embedded test {"failed" if failure else "succeeded"}")

if not os.path.isdir(output_dir):
    os.makedirs(output_dir)

if failure:
    random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
    print(f"Random log file name: {random_name}")

if not os.path.isfile(log_path):
    print("Error: log file not found")
elif failure:
    shutil.copy(log_path, f"{output_dir}/{random_name}_embedded.log")
    print(f"Last {show_lines} lines from {log_path}:")
    with open(log_path, "r") as f:
        lines = f.readlines()
        for line in lines[-show_lines:]:
            print(line.strip())
