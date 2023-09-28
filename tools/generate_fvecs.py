import random
import numpy as np
import os

def generate():
    row_n = 1000
    dim = 128
    fvecs_dir = "./test/data/fvecs"
    slt_dir = "./test/sql/dml/import"

    table_name = "test_fvecs"
    fvecs_path = fvecs_dir + "/test.fvecs"
    slt_path = slt_dir + "/test_fvecs.slt"
    copy_path = "/tmp/infinity/sqllogictest" + "/test.fvecs"

    os.makedirs(fvecs_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    # if os.path.exists(fvecs_path) and os.path.exists(slt_path):
    #     print("file exists, exit")
    #     return
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
            "COPY {} FROM '".format(table_name)
            + copy_path
            + "' WITH ( DELIMITER ',', FORMAT fvecs);\n"
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