import argparse
import random
import numpy as np
import os


def generate():
    parser = argparse.ArgumentParser()
    parser.add_argument("--num", type=int, default=1000)
    parser.add_argument("--dim", type=int, default=128)
    parser.add_argument("--fvecs_dir", type=str, default="./test/data/fvecs")
    parser.add_argument("--slt_dir", type=str, default="./test/sql/dml/import")
    args = parser.parse_args()

    table_name = "test_fvecs"
    fvecs_path = args.fvecs_dir + "/test.fvecs"
    slt_path = args.slt_dir + "/test_fvecs.slt"
    # if os.path.exists(fvecs_path) and os.path.exists(slt_path):
    #     print("file exists, exit")
    #     return

    with open(fvecs_path, "wb") as fvecs_file, open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 embedding(float, {}));\n".format(table_name, args.dim)
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '".format(table_name)
            + fvecs_path
            + "' WITH ( DELIMITER ',', FORMAT fvecs);\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT c1 FROM {};\n".format(table_name))
        slt_file.write("----\n")

        for _ in range(args.num):
            fvecs_file.write((args.dim).to_bytes(4, byteorder="little"))
            fvec = np.random.random(args.dim).astype(np.float32)
            fvec.tofile(fvecs_file)
            fvec_str = ",".join(
                [format(x, ".6g").rstrip("0").rstrip(".") for x in fvec]
            )
            slt_file.write(fvec_str)
            slt_file.write("\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
    random.random()


if __name__ == "__main__":
    generate()
