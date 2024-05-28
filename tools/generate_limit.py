import numpy as np
import random
import os
import argparse


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 9000
    limit = 8500
    offset = 20
    limit_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql"

    table_name = "test_big_limit"
    limit_path = limit_dir + "/test_big_limit.csv"
    slt_path = slt_dir + "/big_limit.slt"
    copy_path = copy_dir + "/test_big_limit.csv"

    os.makedirs(limit_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(limit_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, limit_path
            )
        )
        return
    with open(limit_path, "w") as limit_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 int, c2 int);\n".format(table_name)
        )
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(
                table_name, copy_path
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            "SELECT * FROM {} limit {} offset {};\n".format(table_name, limit, offset))
        slt_file.write("----\n")

        for _ in range(row_n):
            limit_file.write("0,0")
            limit_file.write("\n")

        for _ in range(limit):
            slt_file.write("0 0")
            slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
    random.random()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate limit data for test")

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
