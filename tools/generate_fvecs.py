import numpy as np
import random
import os
import argparse
from generate_util.format_data import format_float1


def generate(generate_if_exists: bool, copy_dir: str):
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
    if os.path.exists(fvecs_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, fvecs_path
            )
        )
        return
    with open(fvecs_path, "wb") as fvecs_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 embedding(float, {}));\n".format(table_name, dim)
        )
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',', FORMAT fvecs);\n".format(
                table_name, copy_path
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT c1 FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for _ in range(row_n):
            fvecs_file.write((dim).to_bytes(4, byteorder="little"))
            fvec = np.random.random(dim).astype(np.float32)
            fvec.tofile(fvecs_file)
            fvec_str = ",".join([format_float1(x) for x in fvec])
            slt_file.write(fvec_str)
            slt_file.write("\n")
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
    random.random()


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
