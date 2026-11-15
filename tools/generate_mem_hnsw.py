# generate 'test/sql/dml/mem_index/insert_with_hnsw_index_big.slt' and 'test/data/csv/insert_with_hnsw_index_big.csv'

import argparse
import os
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
    slt_name = f"/{table_name}.slt"

    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)

    if os.path.exists(slt_path) and not generate_if_exist:
        print(f"File {slt_path}  already existed. Skip Generating.")
        return

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (c1 INTEGER, c2 EMBEDDING(FLOAT, {dim}));\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE INDEX {index_name} ON {table_name}(c2) USING Hnsw WITH (M = {M}, ef_construction = {ef_construction}, metric = {metric});\n"
        )
        slt_file.write("\n")

        row_id = 0
        for i in range(insert_n):
            slt_file.write("statement ok\n")
            slt_file.write(f"INSERT INTO {table_name} VALUES")
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

        slt_file.write(
            "# Wait HNSW index chunk creation done since the chunk rows are invisible during dumping.\n"
        )
        slt_file.write("system ok\n")
        slt_file.write("sleep 1\n")
        slt_file.write("\n")

        for i in range(insert_n):
            row_id = i * batch_n + random.randint(0, batch_n - 1)
            slt_file.write("query I\n")
            slt_file.write(
                "SELECT c1 FROM {} SEARCH MATCH VECTOR (c2, [{}], 'float', '{}', 1) WITH (ef = {});\n".format(
                    table_name, ",".join([str(row_id) for _ in range(dim)]), metric, ef
                )
            )
            slt_file.write("----\n")
            slt_file.write(f"{row_id}\n")
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
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
