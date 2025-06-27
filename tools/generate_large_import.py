# generate 'test/sql/dml/compact/test_compact_big.slt' and 'test/data/csv/test_compact_big.csv'

import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 8192 * 1024 + 1
    table_name = "test_import_more_than_one_segment"

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/import"
    csv_name = "/{}.csv".format(table_name)
    slt_name = "/{}.slt".format(table_name)

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, csv_path
            )
        )
        return

    with open(csv_path, "w") as csv_file:
        for row in range(row_n):
            csv_file.write(f"{row}\n")

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 INTEGER);\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}/{}' WITH ( DELIMITER ',', FORMAT CSV );\n".format(
                table_name, copy_dir, csv_name
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT COUNT(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write("{}\n".format(row_n))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("SHOW TABLE {} SEGMENT 0;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("SHOW TABLE {} SEGMENT 1;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
        slt_file.write("\n")

    pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate more than 8192 * 1024 rows for testing import")

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
