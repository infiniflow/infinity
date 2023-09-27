import os
import random


def generate_test_varchar(slt_path: str, csv_path: str, num: int, dim: int):
    # if os.path.exists(slt_path) and os.path.exists(csv_path):
    #     print("file exists, exit")
    #     return
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
    row_n = 1000
    dim = 128
    slt_dir = "./test/sql/dml/import"
    csv_dir = "./test/data/csv"

    os.makedirs(slt_dir, exist_ok=True)
    os.makedirs(csv_dir, exist_ok=True)

    slt_path = slt_dir + "/test_big_embedding.slt"
    csv_path = csv_dir + "/big_embedding.csv"
    generate_test_embedding(slt_path, csv_path, row_n, dim)
    slt_path = slt_dir + "/test_big_varchar.slt"
    csv_path = csv_dir + "/big_varchar.csv"
    generate_test_varchar(slt_path, csv_path, row_n, dim)

if __name__ == "__main__":
    generate()