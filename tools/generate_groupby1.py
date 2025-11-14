import argparse
import os
import csv
import random
from collections import defaultdict


def generate(generate_if_exists: bool, copy_dir: str):
    data_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/aggregate"

    table_name = "test_big_groupby"
    data_path = data_dir + "/test_big_groupby.csv"
    data_path2 = data_dir + "/test_big_groupby2.csv"
    slt_path = slt_dir + "/test_big_groupby.slt"
    copy_path = copy_dir + "/test_big_groupby.csv"
    copy_path2 = copy_dir + "/test_big_groupby2.csv"

    os.makedirs(data_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if (
        os.path.exists(data_path)
        and os.path.exists(slt_path)
        and not generate_if_exists
    ):
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, data_path
            )
        )
        return

    row_n = 100000
    group_n1 = 150
    group_n2 = 100
    groupby_c1 = defaultdict(list)
    groupby_c2 = defaultdict(list)
    for d_path in [data_path, data_path2]:
        with open(d_path, "w") as data_file:
            writer = csv.writer(data_file)
            for i in range(row_n):
                c1 = random.randint(0, group_n1 - 1)
                c2 = random.randint(0, group_n2 - 1)
                writer.writerow([c1, c2])
                groupby_c1[c1].append(c2)
                groupby_c2[c2].append(c1)

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("CREATE TABLE {} (c1 int, c2 int);\n".format(table_name))
        slt_file.write("\n")

        for c_path in [copy_path, copy_path2]:
            slt_file.write("statement ok\n")
            slt_file.write(
                "COPY {} FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );\n".format(
                    table_name, c_path
                )
            )
            slt_file.write("\n")

        slt_file.write("query III rowsort\n")
        slt_file.write(
            "SELECT c2, COUNT(*), SUM(c1) FROM {} GROUP BY c2;\n".format(table_name)
        )
        slt_file.write("----\n")
        select_res = []
        for c2, c1_list in groupby_c2.items():
            select_res.append(f"{c2} {len(c1_list)} {sum(c1_list)}\n")
        select_res.sort()
        for res in select_res:
            slt_file.write(res)
        slt_file.write("\n")

        slt_file.write("query III rowsort\n")
        slt_file.write(
            "SELECT c1, COUNT(*), SUM(c2) FROM {} GROUP BY c1;\n".format(table_name)
        )
        slt_file.write("----\n")
        select_res = []
        for c1, c2_list in groupby_c1.items():
            select_res.append(f"{c1} {len(c2_list)} {sum(c2_list)}\n")
        select_res.sort()
        for res in select_res:
            slt_file.write(res)
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate groupby data for test")
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
