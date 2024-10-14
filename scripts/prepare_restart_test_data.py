import argparse
import os

filepath_pair = [
    ("benchmark/enwiki/enwiki-10w.csv", "test/data/benchmark/enwiki-10w.csv"),
    ("benchmark/sift1M/sift_base.fvecs", "test/data/benchmark/sift_1m/sift_base.fvecs"),
]

parser = argparse.ArgumentParser(description="Prepare Restart Test Data")
parser.add_argument(
    "--from_dir",
    type=str,
    required=False,
    dest="from_dir",
)
parser.add_argument(
    "--op",
    type=str,
    required=True,
    dest="op",
)

add = True

args = parser.parse_args()
from_dir = args.from_dir
op = args.op
if op == "add":
    add = True
    if from_dir is None:
        raise Exception("from_dir is required for add operation")
elif op == "remove":
    add = False
else:
    raise Exception("Invalid operation")

if add:
    for from_file, to_file in filepath_pair:
        from_path = f"{from_dir}/{from_file}"
        to_dir = os.path.dirname(to_file)
        if not os.path.exists(to_dir):
            os.makedirs(to_dir)
        print(f"rm -f {to_file}")
        os.system(f"rm -f {to_file}")
        print(f"cp {from_path} {to_file}")
        os.system(f"cp {from_path} {to_file}")
else:
    for from_file, to_file in filepath_pair:
        print(f"rm -f {to_file}")
        os.system(f"rm -f {to_file}")
