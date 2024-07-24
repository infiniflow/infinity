# generate 'test/sql/dml/import/test_csr.slt' & 'test/data/csr/test.csr'

import argparse
import argparse
import os
from generate_util.generate_sparse_data import generate_sparse_data
from generate_util.format_data import sparse_format_float


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    max_dim = 30000
    sparsity = 0.01
    fvecs_dir = "./test/data/csr"
    slt_dir = "./test/sql/dml/import"

    table_name = "test_csr"
    csr_path = fvecs_dir + "/test.csr"
    slt_path = slt_dir + "/test_csr.slt"
    copy_path = copy_dir + "/test.csr"

    os.makedirs(fvecs_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csr_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, csr_path
            )
        )
        return

    nnz = int(row_n * max_dim * sparsity)

    indptr, indices, data = generate_sparse_data(row_n, max_dim, sparsity)

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 SPARSE(FLOAT, {}));\n".format(table_name, max_dim)
        )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',', FORMAT csr);\n".format(
                table_name, copy_path
            )
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT c1 FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for row_id in range(row_n):
            start, end = indptr[row_id], indptr[row_id + 1]
            slt_file.write("[")
            for j in range(start, end):
                slt_file.write(
                    "{}:{}".format(indices[j], sparse_format_float(data[j])),
                )
                if j != end - 1:
                    slt_file.write(",")
            slt_file.write("]\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
        slt_file.write("\n")

    with open(csr_path, "wb") as csr_file:
        csr_file.write(row_n.to_bytes(8, "little"))
        csr_file.write(max_dim.to_bytes(8, "little"))
        csr_file.write(nnz.to_bytes(8, "little"))
        indptr.tofile(csr_file)
        indices.tofile(csr_file)
        data.tofile(csr_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate fvecs data for test")
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
