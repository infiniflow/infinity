import argparse
import os
import random

import numpy as np


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 9000
    sort_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/sort_top"

    table_name = "test_sort"
    sort_path = sort_dir + "/test_big_sort.csv"
    slt_path = slt_dir + "/big_sort.slt"
    copy_path = copy_dir + "/test_big_sort.csv"

    os.makedirs(sort_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if (
        os.path.exists(sort_path)
        and os.path.exists(slt_path)
        and not generate_if_exists
    ):
        print(
            f"File {slt_path} and {sort_path} already existed exists. Skip Generating."
        )
        return
    with open(sort_path, "w") as sort_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(f"CREATE TABLE {table_name} (c1 int, c2 int);\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(f"SELECT * FROM {table_name} order by c1, c2;\n")
        slt_file.write("----\n")

        random_integers = np.random.randint(low=1, high=row_n, size=row_n)

        for i in random_integers:
            sort_file.write("0," + str(i))
            sort_file.write("\n")
        for i in sorted(random_integers):
            slt_file.write("0 " + str(i))
            slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
    random.random()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate sort data for test")

    parser.add_argument(
        "-g",
        "--generate",
        type=bool,
        default=False,
        dest="generate_if_exists",
    )
    parser.add_argument(
        "-c",
        "--copy",
        type=str,
        default="/var/infinity/test_data",
        dest="copy_dir",
    )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
