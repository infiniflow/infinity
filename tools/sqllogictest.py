import argparse
import os
from generate_big import generate as generate1
from generate_fvecs import generate as generate2


def test_process(sqllogictest_bin, slt_dir):
    print("sqlllogictest-bin path is {}".format(sqllogictest_bin))

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

    data_dir = current_path + "/test/data"
    test_dir = current_path + "/test/sql"

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
        default="./tools/sqllogictest",
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

    args = parser.parse_args()

    print("Generating file...")
    generate1(args.generate_if_exists)
    generate2(args.generate_if_exists)
    print("Generate file finshed.")
    test_process(args.path, args.test)
