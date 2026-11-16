import argparse
import os

import pyarrow as pa
import pyarrow.parquet as pq
from generate_util.format_data import sparse_format_float


def generate(generate_if_exist: bool, copy_dir: str):
    parquet_dir = "./test/data/parquet"
    import_slt_dir = "./test/sql/dml/import"

    table_name = "parquet_test_table"
    table_name1 = "parquet_test_table1"
    table_name_err = "parquet_test_table_err"
    parquet_filename = "gen_test.parquet"
    parquet_filename1 = "gen_test1.parquet"
    parquet_path = parquet_dir + "/" + parquet_filename
    import_slt_path = import_slt_dir + "/test_import_gen_parquet.slt"
    copy_path = copy_dir + "/" + parquet_filename
    copy_path1 = copy_dir + "/tmp/" + parquet_filename1

    os.makedirs(parquet_dir, exist_ok=True)
    os.makedirs(import_slt_dir, exist_ok=True)
    if (
        os.path.exists(parquet_path)
        and os.path.exists(import_slt_path)
        and not generate_if_exist
    ):
        print(
            f"File {parquet_path} and {import_slt_path} already existed. Skip Generating."
        )
        return

    bool_vec = []
    int8_vec = []
    int64_vec = []
    float_vec = []
    row_n = 10
    for i in range(row_n):
        bool_vec.append(True if i % 2 == 0 else False)
        int8_vec.append(i)
        int64_vec.append(i)
        float_vec.append(i)

    col1 = pa.array(bool_vec, type=pa.bool_())
    col2 = pa.array(int8_vec, type=pa.int8())
    col3 = pa.array(int64_vec, type=pa.int64())
    col4 = pa.array(float_vec, type=pa.float32())
    pa_table = pa.table({"col1": col1, "col2": col2, "col3": col3, "col4": col4})
    with pq.ParquetWriter(parquet_path, pa_table.schema) as writer:
        writer.write_table(pa_table)

    with open(import_slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (col1 Boolean, col2 TINYINT, col3 BIGINT, col4 FLOAT);\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} FROM '{copy_path}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(f"SELECT * FROM {table_name};\n")
        slt_file.write("----\n")
        for i in range(row_n):
            slt_file.write(
                "{} {} {} {}\n".format(
                    "true" if bool_vec[i] else "false",
                    int8_vec[i],
                    int64_vec[i],
                    sparse_format_float(float_vec[i]),
                )
            )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} TO '{copy_path1}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name1};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name1} (col1 Boolean, col2 TINYINT, col3 BIGINT, col4 FLOAT);\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"COPY {table_name1} FROM '{copy_path1}' WITH (FORMAT PARQUET);\n"
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(f"SELECT * FROM {table_name1};\n")
        slt_file.write("----\n")
        for i in range(row_n):
            slt_file.write(
                "{} {} {} {}\n".format(
                    "true" if bool_vec[i] else "false",
                    int8_vec[i],
                    int64_vec[i],
                    sparse_format_float(float_vec[i]),
                )
            )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name1};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
        slt_file.write("\n")

        # import with incompatible schema
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name_err};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name_err} (col1 Boolean, col2 TINYINT, col3 BIGINT);\n"
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
