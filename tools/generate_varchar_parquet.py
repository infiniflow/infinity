import argparse
import os
import pyarrow as pa
import pyarrow.parquet as pq
import numpy as np


def generate(generate_if_exists: bool, copy_dir: str):
    parquet_dir = "./test/data/parquet"
    import_slt_dir = "./test/sql/dml/import"

    table_name = "parquet_varchar_table"
    table_name1 = "parquet_varchar_table1"
    parquet_filename = "gen_varchar.parquet"
    parquet_filename1 = "gen_varchar1.parquet"
    parquet_path = parquet_dir + "/" + parquet_filename
    import_slt_path = import_slt_dir + "/test_import_gen_parquet_varchar.slt"
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
            "File {} and {} already existed. Skip Generating.".format(
                parquet_path, import_slt_path
            )
        )
        return

    row_n = 10
    max_len = 2000
    min_len = 100
    data = []
    for _ in range(row_n):
        len = np.random.randint(min_len, max_len)
        data.append("".join([chr(np.random.randint(32, 127)) for _ in range(len)]))

    col1 = pa.array(range(row_n), type=pa.int32())
    col2 = pa.array(data, type=pa.string())
    table = pa.table({"c1": col1, "c2": col2})

    with pq.ParquetWriter(parquet_path, table.schema) as writer:
        writer.write_table(table)

    with open(import_slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("CREATE TABLE {} (c1 INT, c2 VARCHAR);\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH (FORMAT PARQUET);\n".format(table_name, copy_path)
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT * FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for row_id in range(row_n):
            slt_file.write("{} {}\n".format(row_id, data[row_id]))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "COPY {} TO '{}' WITH (FORMAT PARQUET);\n".format(table_name, copy_path1)
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("CREATE TABLE {} (c1 INT, c2 VARCHAR);\n".format(table_name1))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH (FORMAT PARQUET);\n".format(table_name1, copy_path1)
        )
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write("SELECT * FROM {};\n".format(table_name1))
        slt_file.write("----\n")
        for row_id in range(row_n):
            slt_file.write("{} {}\n".format(row_id, data[row_id]))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name1))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
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
