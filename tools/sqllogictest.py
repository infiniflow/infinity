import argparse
import os
from shutil import copyfile

from generate_big import generate as generate1
from generate_fvecs import generate as generate2
from generate_sort import generate as generate3


def python_skd_test(python_test_dir: str):
    print("python test path is {}".format(python_test_dir))
    # os.system(f"cd {python_test_dir}/test")
    os.system(f"pip install infinity_sdk")
    os.system(f"python -m pytest {python_test_dir}/test")


def test_process(sqllogictest_bin: str, slt_dir: str, data_dir: str, copy_dir: str):
    print("sqlllogictest-bin path is {}".format(sqllogictest_bin))

    copy_all(data_dir, copy_dir)

    test_cnt = 0
    for dirpath, dirnames, filenames in os.walk(slt_dir):
        for filename in filenames:
            file = os.path.join(dirpath, filename)
            os.system(sqllogictest_bin + " " + file)
            print("Finished running test file: " + file)
            print("-" * 100)
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
    copy_dir = "/tmp/infinity/test_data"
    python_test_dir = current_path + "/python"

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
    parser.add_argument(
        "-jc",
        "--just_copy",
        default=copy_all(data_dir, copy_dir),
        dest="just_copy_all_data",
    )

    args = parser.parse_args()

    print("Generating file...")
    generate1(args.generate_if_exists, args.copy)
    generate2(args.generate_if_exists, args.copy)
    generate3(args.generate_if_exists, args.copy)
    print("Generate file finshed.")
    python_skd_test(python_test_dir)
    test_process(args.path, args.test, args.data, args.copy)
