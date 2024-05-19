import argparse
import itertools
import os
import sys
import threading
import time
from shutil import copyfile
import subprocess
import signal

from generate_big import generate as generate1
from generate_fvecs import generate as generate2
from generate_sort import generate as generate3
from generate_limit import generate as generate4
from generate_aggregate import generate as generate5
from generate_top import generate as generate6
from generate_top_varchar import generate as generate7
from generate_compact import generate as generate8
from generate_hnsw_with_delete import generate as generate9
from generate_index_scan import generate as generate10
from generate_many_import import generate as generate11
from generate_big_point_query_test_fastroughfilter import generate as generate12
from generate_many_import_drop import generate as generate13
from generate_mem_hnsw import generate as generate14

class SpinnerThread(threading.Thread):
    def __init__(self):
        super(SpinnerThread, self).__init__()
        self.stop = False

    def run(self):
        spinner = itertools.cycle(["-", "/", "|", "\\"])
        while not self.stop:
            print(next(spinner), end="\r")
            time.sleep(0.1)

    def stop_spinner(self):
        self.stop = True


def process_test(sqllogictest_bin: str, slt_dir: str, data_dir: str, copy_dir: str):
    print("sqlllogictest-bin path is {}".format(sqllogictest_bin))
    print("slt_dir path is {}".format(slt_dir))
    print("data_dir path is {}".format(data_dir))

    test_cnt = 0
    for dirpath, dirnames, filenames in os.walk(slt_dir):
        for filename in filenames:
            file = os.path.join(dirpath, filename)

            # filename = os.path.basename(file)
            # if "fulltext" in filename or "fusion" in filename or "test_compact_big" in filename:
            #     continue

            print("Start running test file: " + file)
            process = subprocess.run(
                [sqllogictest_bin, file], stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )
            output, error = process.stdout, process.stderr
            print(f"Output: {output.decode()}")  # Prints the output.
            if process.returncode != 0:
                raise Exception(
                    f"An error occurred: {error.decode()}"
                )  # Prints the error message.
            print("=" * 99)
            test_cnt += 1

    print("Finished {} tests.".format(test_cnt))


# copy data
def copy_all(data_dir, copy_dir):
    if not os.path.exists(copy_dir):
        os.makedirs(copy_dir)
    for dirpath, dirnames, filenames in os.walk(data_dir):
        for filename in filenames:
            src_path = os.path.join(dirpath, filename)
            dest_path = os.path.join(copy_dir, filename)
            copyfile(src_path, dest_path)
    print("Finished copying all files.")


# main
if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")

    current_path = os.getcwd()

    test_dir = current_path + "/test/sql"
    data_dir = current_path + "/test/data"
    copy_dir = "/var/infinity/test_data"

    parser = argparse.ArgumentParser(description="SQL Logic Test For Infinity")

    parser.add_argument(
        "-g",
        "--generate",
        type=bool,
        default=True,
        dest="generate_if_exists",
    )
    parser.add_argument(
        "-p",
        "--path",
        help="path of sqllogictest-rs",
        type=str,
        default="sqllogictest",
        dest="path",
    )
    parser.add_argument(
        "-t",
        "--test",
        help="path of test directory",
        type=str,
        default=test_dir,
        dest="test",
    )
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default=data_dir,
        dest="data",
    )
    parser.add_argument(
        "-c",
        "--copy",
        type=str,
        default=copy_dir,
        dest="copy",
    )
    parser.add_argument(
        "-jc",
        "--just_copy",
        type=bool,
        default=False,
        dest="just_copy_all_data",
    )

    args = parser.parse_args()

    print("Generating file...")
    generate1(args.generate_if_exists, args.copy)
    generate2(args.generate_if_exists, args.copy)
    generate3(args.generate_if_exists, args.copy)
    generate4(args.generate_if_exists, args.copy)
    generate5(args.generate_if_exists, args.copy)
    generate6(args.generate_if_exists, args.copy)
    generate7(args.generate_if_exists, args.copy)
    generate8(args.generate_if_exists, args.copy)
    generate9(args.generate_if_exists, args.copy)
    generate10(args.generate_if_exists, args.copy)
    generate11(args.generate_if_exists, args.copy)
    generate12(args.generate_if_exists, args.copy)
    generate13(args.generate_if_exists, args.copy)
    generate14(args.generate_if_exists, args.copy)
    print("Generate file finshed.")

    print("Start copying data...")
    if args.just_copy_all_data is True:
        copy_all(args.data, args.copy)
    else:
        copy_all(args.data, args.copy)
        print("Start testing...")
        start = time.time()
        try:
            process_test(args.path, args.test, args.data, args.copy)
        except Exception as e:
            print(e)
            sys.exit(-1)
        end = time.time()
        print("Test finished.")
        print("Time cost: {}s".format(end - start))
