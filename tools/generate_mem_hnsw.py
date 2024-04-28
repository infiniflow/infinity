# generate 'test/sql/dml/mem_index/insert_with_hnsw_index_big.slt' and 'test/data/csv/insert_with_hnsw_index_big.csv'

import os
import argparse
import random


def generate(generate_if_exist: bool, copy_dir: str):
    batch_n = 1000
    insert_n = 10
    dim = 16
    table_name = "insert_with_hnsw_index_big"
    index_name = "idx1"
    M = 16
    ef_construction = 200
    metric = "l2"
    ef = 8

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/mem_index"
    slt_name = "/{}.slt".format(table_name)

    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)

    if os.path.exists(slt_path) and generate_if_exist:
        print("File {}  already existed. Skip Generating.".format(slt_path))
        return

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 INTEGER, c2 EMBEDDING(FLOAT, {}));\n".format(
                table_name, dim
            )
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE INDEX {} ON {}(c2) USING Hnsw WITH (M = {}, ef_construction = {}, metric = {});\n".format(
                index_name, table_name, M, ef_construction, metric
            )
        )
        slt_file.write("\n")

        row_id = 0
        for i in range(insert_n):
            slt_file.write("statement ok\n")
            slt_file.write("INSERT INTO {} VALUES".format(table_name))
            for j in range(batch_n):
                slt_file.write(
                    " ({}, [{}])".format(
                        row_id, ",".join([str(row_id) for _ in range(dim)])
                    )
                )
                row_id += 1
                if j != batch_n - 1:
                    slt_file.write(",")
            slt_file.write(";\n")
            slt_file.write("\n")

        for i in range(insert_n):
            row_id = i * batch_n + random.randint(0, batch_n - 1)
            slt_file.write("query I\n")
            slt_file.write(
                "SELECT c1 FROM {} SEARCH KNN(c2, [{}], 'float', '{}', 1) WITH (ef = {});\n".format(
                    table_name, ",".join([str(row_id) for _ in range(dim)]), metric, ef
                )
            )
            slt_file.write("----\n")
            slt_file.write("{}\n".format(row_id))
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
        slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate mem hnsw data for test")

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
