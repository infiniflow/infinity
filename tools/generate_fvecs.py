import argparse
import os
import random

import numpy as np
from generate_util.format_data import format_float1


def generate1(generate_if_exists: bool, copy_dir: str):
    row_n = 1000
    dim = 128
    fvecs_dir = "./test/data/fvecs"
    slt_dir = "./test/sql/dml/import"

    table_name = "test_fvecs"
    fvecs_path = fvecs_dir + "/test.fvecs"
    slt_path = slt_dir + "/test_fvecs.slt"
    copy_path = copy_dir + "/test.fvecs"

    os.makedirs(fvecs_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if (
        os.path.exists(fvecs_path)
        and os.path.exists(slt_path)
        and not generate_if_exists
    ):
        print(
            f"File {slt_path} and {fvecs_path} already existed exists. Skip Generating."
        )
        return
    with open(fvecs_path, "wb") as fvecs_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(f"CREATE TABLE {table_name} ( c1 embedding(float, {dim}));\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT fvecs);\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(f"SELECT c1 FROM {table_name};\n")
        slt_file.write("----\n")
        for _ in range(row_n):
            fvecs_file.write((dim).to_bytes(4, byteorder="little"))
            fvec = np.random.random(dim).astype(np.float32)
            fvec.tofile(fvecs_file)
            fvec_str = ",".join([format_float1(x) for x in fvec])
            slt_file.write("[")
            slt_file.write(fvec_str)
            slt_file.write("]")
            slt_file.write("\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
    random.random()


def generate_fvecs(num, dim, filename):
    with open(os.getcwd() + "/test/data/fvecs/" + filename, "wb") as fvecs_file:
        for _ in range(num):
            fvecs_file.write((dim).to_bytes(4, byteorder="little"))
            fvec = np.random.random(dim).astype(np.float32)
            fvec.tofile(fvecs_file)
    fvecs_file.close()


def generate(generate_if_exists: bool, copy_dir: str):
    generate1(generate_if_exists, copy_dir)
    generate_fvecs(100, 128, "pysdk_test.fvecs")


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
