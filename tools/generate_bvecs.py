import argparse
import os
import random

import numpy as np
from generate_util.format_data import format_float1


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 1000
    dim = 128
    bvecs_dir = "./test/data/bvecs"
    slt_dir = "./test/sql/dml/import"

    table_name = "test_bvecs"
    bvecs_path = bvecs_dir + "/test.bvecs"
    slt_path = slt_dir + "/test_bvecs.slt"
    copy_path = copy_dir + "/test.bvecs"

    os.makedirs(bvecs_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if (
        os.path.exists(bvecs_path)
        and os.path.exists(slt_path)
        and not generate_if_exists
    ):
        print(
            f"File {slt_path} and {bvecs_path} already existed exists. Skip Generating."
        )
        return
    with open(bvecs_path, "wb") as bvecs_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} ( c1 embedding(unsigned tinyint, {dim}));\n"
        )
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT bvecs);\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(f"SELECT c1 FROM {table_name};\n")
        slt_file.write("----\n")
        for _ in range(row_n):
            bvecs_file.write((dim).to_bytes(4, byteorder="little"))
            random_array = 20 * np.random.random(dim)  # 0~20
            ivec = random_array.astype(np.uint8)  # 0~20
            ivec.tofile(bvecs_file)
            bvec = ivec.astype(np.uint8)
            bvec_str = ",".join([format_float1(x) for x in bvec])
            slt_file.write("[")
            slt_file.write(bvec_str)
            slt_file.write("]")
            slt_file.write("\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
    random.random()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate bvecs data for test")

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
