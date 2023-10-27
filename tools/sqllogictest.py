import argparse
import os
from generate_big import generate as generate1
from generate_fvecs import generate as generate2
from shutil import copyfile


def test_process(sqllogictest_bin: str, slt_dir: str, data_dir: str, copy_dir: str):
    print("sqlllogictest-bin path is {}".format(sqllogictest_bin))

    if not os.path.exists(copy_dir):
        os.makedirs(copy_dir)

    for dirpath, dirnames, filenames in os.walk(data_dir):
        for filename in filenames:
            src_path = os.path.join(dirpath, filename)
            dest_path = os.path.join(copy_dir, filename)
            copyfile(src_path, dest_path)
    test_cnt = 0
    for dirpath, dirnames, filenames in os.walk(slt_dir):
        for filename in filenames:
            file = os.path.join(dirpath, filename)
            os.system(sqllogictest_bin + " " + file)
            print("Finished running test file: " + file)
            print("----------------------------------------------------------\n")
            test_cnt += 1

    print("Finished {} tests.".format(test_cnt))


# main
if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")

    current_path = os.getcwd()

    test_dir = current_path + "/test/sql"
    data_dir = current_path + "/test/data"
    copy_dir = "/tmp/infinity/test_data"

    parser = argparse.ArgumentParser(description="SQL Logic Test For Infinity")

    parser.add_argument(
        "-g",
        "--generate",
        type=bool,
        default=False,
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

    args = parser.parse_args()

    print("Generating file...")
    generate1(args.generate_if_exists)
    generate2(args.generate_if_exists)
    print("Generate file finshed.")
    test_process(args.path, args.test, args.data, args.copy)
