import argparse
import os

import pyarrow as pa
import pyarrow.parquet as pq
from generate_util.format_data import sparse_format_float
from generate_util.generate_sparse_data import generate_sparse_data


def generate(generate_if_exists: bool, copy_dir: str):
    parquet_dir = "./test/data/parquet"
    import_slt_dir = "./test/sql/dml/import"

    table_name = "parquet_sparse_table"
    table_name1 = "parquet_sparse_table1"
    table_name_err = "parquet_sparse_table_err"
    parquet_filename = "gen_sparse.parquet"
    parquet_filename1 = "gen_sparse1.parquet"
    parquet_path = parquet_dir + "/" + parquet_filename
    import_slt_path = import_slt_dir + "/test_import_gen_parquet_sparse.slt"
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
    max_dim = 30000
    sparsity = 0.01
    indptr, indices, data = generate_sparse_data(row_n, max_dim, sparsity)
    col1 = pa.array(range(row_n), type=pa.int32())
    col2_vec = []

    with open(import_slt_path, "w") as slt_file:

        def write_query():
            for row_id in range(row_n):
                start, end = indptr[row_id], indptr[row_id + 1]
                slt_file.write(f"{row_id} [")
                for j in range(start, end):
                    slt_file.write(f"{indices[j]}:{sparse_format_float(data[j])}")
                    if j != end - 1:
                        slt_file.write(",")
                slt_file.write("]\n")
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (c1 INT, c2 SPARSE(FLOAT, {max_dim}) WITH (SORTED));\n"
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} FROM '{copy_path}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(f"SELECT * FROM {table_name};\n")
        slt_file.write("----\n")
        write_query()

        slt_file.write("statement ok\n")
        slt_file.write(f"COPY {table_name} TO '{copy_path1}' WITH (FORMAT PARQUET);\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name1};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name1} (c1 INT, c2 SPARSE(FLOAT, {max_dim}) WITH (SORTED));\n"
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
        write_query()

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
            f"CREATE TABLE {table_name_err} (c1 INT, c2 SPARSE(DOUBlE, {max_dim}) WITH (SORTED));\n"
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

    for row_id in range(row_n):
        start, end = indptr[row_id], indptr[row_id + 1]
        col2_vec.append(
            {
                "index": indices[start:end],
                "value": data[start:end],
            }
        )
    col2 = pa.array(
        col2_vec,
        type=pa.struct(
            [
                pa.field("index", pa.list_(pa.int16())),
                pa.field("value", pa.list_(pa.float32())),
            ]
        ),
    )
    table = pa.table(
        {
            "col1": col1,
            "col2": col2,
        }
    )

    with pq.ParquetWriter(
        parquet_path,
        table.schema,
    ) as writer:
        writer.write_table(table)


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
