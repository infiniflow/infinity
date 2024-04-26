# generate 'test/sql/dml/compact/test_compact_big.slt' and 'test/data/csv/test_compact_big.csv'

import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000000
    table_name = "test_compact_big"

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/compact"
    csv_name = "/{}.csv".format(table_name)
    slt_name = "/{}.slt".format(table_name)

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, csv_path
            )
        )
        return

    x = [i for i in range(row_n)]
    random.shuffle(x)
    delete_x = set(random.sample(range(row_n), row_n // 100))
    y = [False if i in delete_x else True for i in x]

    with open(csv_path, "w") as csv_file:
        for x1, y1 in zip(x, y):
            csv_file.write("{},{}\n".format(x1, y1))

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 INTEGER, c2 BOOLEAN);\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}/{}' WITH ( DELIMITER ',' );\n".format(
                table_name, copy_dir, csv_name
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DELETE FROM {} WHERE c2 = False;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("COMPACT TABLE {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT c1 FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for x1, y1 in zip(x, y):
            if y1:
                slt_file.write("{}\n".format(x1))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
        slt_file.write("\n")

    pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate top data for test")

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
