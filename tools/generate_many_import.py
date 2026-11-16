# generate 'test/sql/dml/compact/test_big_many_import.slt'

import argparse
import os
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    import_n = 100
    table_name = "test_big_many_import"

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/compact"
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
        slt_file.write(f"COMPACT TABLE {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"COMPACT TABLE {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
        slt_file.write("\n")


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
