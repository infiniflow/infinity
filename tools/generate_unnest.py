import argparse
import os
import csv
import random
from collections import defaultdict


def generate(generate_if_exists: bool, copy_dir: str):
    data_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/unnest"

    table_name = "test_big_unnest"
    slt_path = slt_dir + "/test_big_unnest.slt"
    copy_path = copy_dir + "/test_big_unnest.csv"

    os.makedirs(data_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(slt_path) and not generate_if_exists:
        print("File {} already existed exists. Skip Generating.".format(slt_path))
        return

    varchar_samples = [
        "Beijing",
        "Shanghai",
        "Guangzhou",
        "Shenzhen",
        "Hangzhou",
        "Chengdu",
        "Nanjing",
        "Wuhan",
        "Suzhou",
        "Tianjin",
        "Chongqing",
        "Xian",
        "Changsha",
        "Zhengzhou",
        "Dalian",
        "Qingdao",
        "Shenyang",
        "Changchun",
        "Harbin",
        "Nanning",
        "Kunming",
        "Lhasa",
        "Hefei",
        "Fuzhou",
        "Xiamen",
        "Nanchang",
        "Guiyang",
        "Haikou",
    ]
    array_length_max = 5
    assert array_length_max < len(varchar_samples)
    c1_max = 100
    row_n = 10000
    batch_size = 100
    data = []

    varchar_to_int = defaultdict(list)
    for i in range(row_n):
        c1 = random.randint(0, c1_max - 1)
        c2 = []
        c2_len = random.randint(0, array_length_max)
        while len(c2) < c2_len:
            sample = random.choice(varchar_samples)
            if sample not in c2:
                c2.append(sample)
        data.append((c1, c2))
        for c2_item in c2:
            varchar_to_int[c2_item].append(c1)

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (id INTEGER, c1 INTEGER, c2 ARRAY(VARCHAR));\n"
        )
        slt_file.write("\n")

        off = 0
        for i in range(0, row_n, batch_size):
            slt_file.write("statement ok\n")
            slt_file.write(f"INSERT INTO {table_name} VALUES\n")
            batch_data = data[i : i + batch_size]
            for i, (c1, c2) in enumerate(batch_data):
                c2_str = "{" + ",".join(list(map(lambda x: f"'{x}'", c2))) + "}"
                id = i + off
                slt_file.write(f"({id}, {c1}, {c2_str})")
                if i != len(batch_data) - 1:
                    slt_file.write(",")
                else:
                    slt_file.write(";")
                slt_file.write("\n")
            slt_file.write("\n")
            off += len(batch_data)

        slt_file.write("query IIT rowsort\n")
        slt_file.write(f"SELECT id, c1, UNNEST(c2) FROM {table_name};\n")
        slt_file.write("----\n")
        select_res = []
        for id, (c1, c2) in enumerate(data):
            for c2_item in c2:
                select_res.append(f"{id} {c1} {c2_item}\n")
        select_res.sort()
        for res in select_res:
            slt_file.write(res)
        slt_file.write("\n")

        slt_file.write("query TI rowsort\n")
        slt_file.write(
            f"SELECT UNNEST(c2) as uc2, SUM(c1) FROM {table_name} GROUP BY uc2;\n"
        )
        slt_file.write("----\n")
        select_res = []
        for c2, c1_list in varchar_to_int.items():
            select_res.append(f"{c2} {sum(c1_list)}\n")
        select_res.sort()
        for res in select_res:
            slt_file.write(res)
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-g", "--generate", type=bool, default=False, dest="generate_if_exists"
    )
    parser.add_argument(
        "-c", "--copy", type=str, default="/var/infinity/test_data", dest="copy_dir"
    )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
