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
    "--output_path", type=str, required=True, help="Path to the output debug log file"
)
args = parser.parse_args()

log_path = args.log_path
output_path = args.output_path

if not os.path.isfile(log_path):
    print("Error: /var/infinity/log/infinity.log not found")

with open(output_path, "a") as debug_log, open(log_path, "a") as infinity_log:
    pass  # This will create the files if they don't exist

random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
print(f"Random log file name: {random_name}")

shutil.copy(output_path, f"/var/infinity/log/{random_name}_.log")
shutil.copy(log_path, f"/var/infinity/log/{random_name}.log")
