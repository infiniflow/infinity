import argparse
import os

import numpy as np
import pyarrow as pa
import pyarrow.parquet as pq


def generate(generate_if_exists: bool, copy_dir: str):
    parquet_dir = "./test/data/parquet"
    import_slt_dir = "./test/sql/dml/import"

    table_name = "parquet_embedding_table"
    table_name1 = "parquet_embedding_table1"
    table_name_err = "parquet_embedding_table_err"
    parquet_filename = "gen_embedding.parquet"
    parquet_filename1 = "gen_embedding1.parquet"
    parquet_path = parquet_dir + "/" + parquet_filename
    import_slt_path = import_slt_dir + "/test_import_gen_parquet_embedding.slt"
    copy_path = copy_dir + "/" + parquet_filename
    copy_path1 = copy_dir + "/tmp/" + parquet_filename1

    os.makedirs(parquet_dir, exist_ok=True)
    os.makedirs(import_slt_dir, exist_ok=True)
    if (
        os.path.exists(parquet_path)
        and os.path.exists(import_slt_path)
        and not generate_if_exists
    ):
        print(
            f"File {parquet_path} and {import_slt_path} already existed. Skip Generating."
        )
        return

    row_n = 10
    dim = 128
    max_x = 1000
    min_x = 0
    data = []
    for i in range(row_n):
        data.append([int(np.random.uniform(min_x, max_x)) for _ in range(dim)])

    col1 = pa.array(range(row_n), type=pa.int32())
    col2 = pa.array(data, type=pa.list_(pa.int32(), dim))
    table = pa.table({"c1": col1, "c2": col2})

    with pq.ParquetWriter(parquet_path, table.schema) as writer:
        writer.write_table(table)

    with open(import_slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (c1 INT, c2 EMBEDDING(INT, {dim}));\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} FROM '{copy_path}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(f"SELECT * FROM {table_name};\n")
        slt_file.write("----\n")
        for row_id in range(row_n):
            slt_file.write(f"{row_id} [")

            for i in range(dim):
                slt_file.write(f"{data[row_id][i]}")
                if i != dim - 1:
                    slt_file.write(",")
            slt_file.write("]\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} TO '{copy_path1}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name1};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name1} (c1 INT, c2 EMBEDDING(INT, {dim}));\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"COPY {table_name1} FROM '{copy_path1}' WITH (FORMAT PARQUET);\n"
        )
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write(f"SELECT * FROM {table_name1};\n")
        slt_file.write("----\n")
        for row_id in range(row_n):
            slt_file.write(f"{row_id} [")

            for i in range(dim):
                slt_file.write(f"{data[row_id][i]}")
                if i != dim - 1:
                    slt_file.write(",")
            slt_file.write("]\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name1};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
        slt_file.write("\n")

        # import with incompactible schema
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name_err};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name_err} (c1 INT, c2 EMBEDDING(INT, {dim + 1}));\n"
        )
        slt_file.write("\n")

        slt_file.write("statement error\n")
        slt_file.write(
            f"COPY {table_name_err} FROM '{copy_path}' WITH (FORMAT PARQUET);\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name_err};\n")
        slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate parquet data for test")
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
