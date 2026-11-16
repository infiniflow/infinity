# generate 'test/sql/dml/compact/test_big_many_import_drop.slt'

import argparse
import os
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 100
    import_n = 100
    loop_n = 100
    table_name = "test_big_many_import_drop"

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/cleanup"
    csv_name = f"/{table_name}.csv"
    slt_name = f"/{table_name}.slt"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            f"File {slt_path} and {csv_path} already existed exists. Skip Generating."
        )
        return

    p = 1000
    max_v = row_n // p

    with open(csv_path, "w") as csv_file:
        for _ in range(p):
            x = [i for i in range(max_v)]
            random.shuffle(x)
            csv_file.writelines(f"{x1}\n" for x1 in x)

    with open(slt_path, "w") as slt_file:
        for i in range(loop_n):
            slt_file.write("statement ok\n")
            slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
            slt_file.write("\n")

            slt_file.write("statement ok\n")
            slt_file.write(f"CREATE TABLE {table_name} (c1 INTEGER);\n")
            slt_file.write("\n")

            for _ in range(import_n):
                slt_file.write("statement ok\n")
                slt_file.write(
                    f"COPY {table_name} FROM '{copy_dir}{csv_name}' WITH ( DELIMITER ',', FORMAT CSV );\n"
                )
                slt_file.write("\n")

            slt_file.write("statement ok\n")
            slt_file.write(f"DROP TABLE {table_name};\n")
            slt_file.write("\n")
        # # The delete will throw exception when compacting, so add this to wait for sometime
        # slt_file.write("statement ok\n")
        # slt_file.write("SELECT * FROM {};\n".format(table_name))
        # slt_file.write("\n")

        # for v in range(max_v):
        #     slt_file.write("statement ok\n")
        #     slt_file.write("DELETE FROM {} WHERE c1 = {};\n".format(table_name, v))
        #     slt_file.write("\n")

        # slt_file.write("query I\n")
        # slt_file.write("SELECT * FROM {};\n".format(table_name))
        # slt_file.write("----\n")
        # slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate many import for test")

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
