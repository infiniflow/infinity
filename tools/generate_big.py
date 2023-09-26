import os
import random
import argparse


def generate_test_varchar(slt_path: str, csv_path: str, num: int, dim: int):
    if os.path.exists(slt_path) and os.path.exists(csv_path):
        print("file exists, exit")
        return
    low, high = 0, 100
    table_name = "big_varchar_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 int, c2 varchar({}));\n".format(table_name, dim)
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '".format(table_name)
            + csv_path
            + "' WITH ( DELIMITER ',' );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write("SELECT c1, c2 FROM {};\n".format(table_name))
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
        slt_file.write("DROP TABLE {};\n".format(table_name))


def generate_test_embedding(slt_path: str, csv_path: str, num: int, dim: int):
    if os.path.exists(slt_path) and os.path.exists(csv_path):
        print("The target {} and {} already exists. Return.".format(slt_path, csv_path))
        return
    low, high = 0, 1000
    table_name = "big_embedding_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 int, c2 embedding(int, {}));\n".format(
                table_name, dim
            )
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '".format(table_name)
            + csv_path
            + "' WITH ( DELIMITER ',' );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query II\n")
        slt_file.write("SELECT c1, c2 FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for _i in range(num):
            integer = random.randint(low, high)
            col_n = random.randint(dim // 2, dim)
            embedding = [random.randint(low, high) for _ in range(col_n)]
            csv_file.write(
                str(integer) + ',"[' + ",".join([str(x) for x in embedding]) + ']"\n'
            )
            embedding.extend([0] * (dim - col_n))
            slt_file.write(
                str(integer) + " " + ",".join([str(x) for x in embedding]) + "\n"
            )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))


# def execute_slt(slt_path: str, log_path: str):
#     with open(log_path, "w") as log_file:
#         log_file = open(log_path, "w")
#         Popen = subprocess.Popen(
#             ["sqllogictest", slt_path], stdout=log_file, stderr=log_file
#         )
#         Popen.wait()


def generate():
    print("Note: this script must be run under root directory of the project!")
    parser = argparse.ArgumentParser()

    parser.add_argument("--num", type=int, default=1000)
    parser.add_argument("--dim", type=int, default=128)
    parser.add_argument("--slt_dir", type=str, default="./test/sql/dml/import")
    parser.add_argument("--csv_dir", type=str, default="./test/data/csv")
    args = parser.parse_args()

    slt_path = args.slt_dir + "/test_big_embedding.slt"
    csv_path = args.csv_dir + "/big_embedding.csv"
    generate_test_embedding(slt_path, csv_path, args.num, args.dim)

    slt_path = args.slt_dir + "/test_big_varchar.slt"
    csv_path = args.csv_dir + "/big_varchar.csv"
    generate_test_varchar(slt_path, csv_path, args.num, args.dim)

if __name__ == "__main__":
    generate()