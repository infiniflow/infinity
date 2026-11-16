import argparse
import os
import random


def generate_test_varchar(
    slt_path: str,
    csv_path: str,
    copy_path: str,
    num: int,
    dim: int,
    generate_if_exists: bool,
):
    if os.path.exists(slt_path) and os.path.exists(csv_path) and not generate_if_exists:
        print(
            f"File {slt_path} and {csv_path} already existed exists. Skip Generating."
        )
        return
    low, high = 0, 100
    table_name = "big_varchar_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"CREATE TABLE {table_name} ( c1 int, c2 varchar);\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write(f"SELECT c1, c2 FROM {table_name};\n")
        slt_file.write("----\n")
        for _i in range(num):
            integer = random.randint(low, high)
            col_n = random.randint(dim // 2, dim)
            varchar = "".join(
                [chr(random.randint(ord("A"), ord("Z"))) for _ in range(col_n)]
            )
            csv_file.write(str(integer) + ',"' + varchar + '"\n')
            slt_file.write(str(integer) + " " + varchar + "\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")


def generate_test_embedding(
    slt_path: str,
    csv_path: str,
    copy_path: str,
    num: int,
    dim: int,
    generate_if_exists: bool,
):
    if os.path.exists(slt_path) and os.path.exists(csv_path) and not generate_if_exists:
        print(
            f"File {slt_path} and {csv_path} already existed exists. Skip Generating."
        )
        return
    low, high = 0, 1000
    table_name = "big_embedding_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} ( c1 int, c2 embedding(int, {dim}));\n"
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write(f"SELECT c1, c2 FROM {table_name};\n")
        slt_file.write("----\n")
        for _i in range(num):
            integer = random.randint(low, high)
            col_n = dim
            embedding = [random.randint(low, high) for _ in range(col_n)]
            csv_file.write(
                str(integer) + ',"[' + ",".join([str(x) for x in embedding]) + ']"\n'
            )
            slt_file.write(
                str(integer) + " [" + ",".join([str(x) for x in embedding]) + "]\n"
            )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")


def generate(generate_if_exists, copy_dir: str):
    print("Note: this script must be run under root directory of the project.")
    row_n = 1000
    dim = 128
    slt_dir = "./test/sql/dml/import"
    csv_dir = "./test/data/csv"

    os.makedirs(slt_dir, exist_ok=True)
    os.makedirs(csv_dir, exist_ok=True)

    slt_path = slt_dir + "/test_big_embedding.slt"
    csv_path = csv_dir + "/big_embedding.csv"
    copy_path = copy_dir + "/big_embedding.csv"
    generate_test_embedding(
        slt_path, csv_path, copy_path, row_n, dim, generate_if_exists
    )
    slt_path = slt_dir + "/test_big_varchar.slt"
    csv_path = csv_dir + "/big_varchar.csv"
    copy_path = copy_dir + "/big_varchar.csv"
    generate_test_varchar(slt_path, csv_path, copy_path, row_n, dim, generate_if_exists)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate big data for test")

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
