import os
import random
import shutil
import string


def collect_log(stderror_path, output_dir):
    random_name = "".join(random.choices(string.ascii_lowercase + string.digits, k=8))
    print(f"Random log file name: {random_name}")
    if not os.path.isfile(stderror_path):
        print("Error: stderror file not found")
    else:
        shutil.copy(stderror_path, f"{output_dir}/{random_name}_sanitizer_stderror.log")