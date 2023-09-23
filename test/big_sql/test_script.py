import os
import random
import subprocess

cur_dir = os.path.dirname(os.path.realpath(__file__))


def generate_test_varchar(slt_path: str):
    csv_path = cur_dir + "/tmp/test_varchar.csv"
    # if os.path.exists(slt_path) and os.path.exists(csv_path):
    #     return
    row_n = 8192
    length = 65534
    low, high = 0, 100
    table_name = "test_varchar_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 int, c2 varchar({}));\n".format(table_name, length)
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
        for _i in range(row_n):
            integer = random.randint(low, high)
            col_n = random.randint(length // 2, length)
            varchar = "".join(
                [chr(random.randint(ord("A"), ord("Z"))) for _ in range(col_n)]
            )
            csv_file.write(str(integer) + ',"' + varchar + '"\n')
            slt_file.write(str(integer) + " " + varchar + "\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))


def generate_test_embedding(slt_path: str):
    csv_path = cur_dir + "/tmp/test_embedding.csv"
    if os.path.exists(slt_path) and os.path.exists(csv_path):
        return
    row_n = 8192
    dimension = 100
    low, high = 0, 1000
    table_name = "test_embedding_table"
    with open(slt_path, "w") as slt_file, open(csv_path, "w") as csv_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} ( c1 int, c2 embedding(int, {}));\n".format(
                table_name, dimension
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
        for _i in range(row_n):
            integer = random.randint(low, high)
            col_n = random.randint(dimension // 2, dimension)
            embedding = [random.randint(low, high) for _ in range(col_n)]
            csv_file.write(
                str(integer) + ',"[' + ",".join([str(x) for x in embedding]) + ']"\n'
            )
            embedding.extend([0] * (dimension - col_n))
            slt_file.write(
                str(integer) + " " + ",".join([str(x) for x in embedding]) + "\n"
            )
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))


def execute_slt(slt_path: str, log_path: str):
    with open(log_path, "w") as log_file:
        log_file = open(log_path, "w")
        Popen = subprocess.Popen(
            ["sqllogictest", slt_path], stdout=log_file, stderr=log_file
        )
        Popen.wait()


if __name__ == "__main__":
    # slt_path = cur_dir + "/tmp/test_embedding.slt"
    # log_path = cur_dir + "/tmp/test_embedding.log"
    # generate_test_embedding(slt_path)
    # execute_slt(slt_path, log_path)

    slt_path = cur_dir + "/tmp/test_varchar.slt"
    log_path = cur_dir + "/tmp/test_varchar.log"
    generate_test_varchar(slt_path)
    execute_slt(slt_path, log_path)
