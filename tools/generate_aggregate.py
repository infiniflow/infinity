import numpy as np
import random
import os
import argparse


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 9000
    sort_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/aggregate"

    table_name = "test_simple_agg_big_cpp"
    agg_path = sort_dir + "/test_simple_agg_big.csv"
    slt_path = slt_dir + "/test_simple_agg_big.slt"
    copy_path = copy_dir + "/test_simple_agg_big.csv"

    os.makedirs(sort_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(agg_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, agg_path
            )
        )
        return

    sequence = np.arange(1, row_n + 1)
    with open(agg_path, "w") as agg_file, open(slt_path, "w") as slt_file:
        # write to csv
        for i in sequence:
            agg_file.write(str(i) + "," + str(i))
            agg_file.write("\n")

        # write to slt
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 int, c2 float);\n".format(table_name)
        )

        # select count(*) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(0))
        slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );\n".format(
                table_name, copy_path
            )
        )

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT * FROM {};\n".format(table_name))
        slt_file.write("----\n")
        for i in sequence:
            slt_file.write(str(i) + " " + str(i)+".000000")
            slt_file.write("\n")
        slt_file.write("\n")

        # select max(c1) from test_simple_agg_big

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT max(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(row_n))
        slt_file.write("\n")

        # select min(c2) from test_simple_agg_big

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT min(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(1))
        slt_file.write("\n")

        # select sum(c1) from test_simple_agg_big

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT sum(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(np.sum(sequence)))
        slt_file.write("\n")

        # select avg(c1) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT AVG(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(np.mean(sequence)) + "00000")
        slt_file.write("\n")

        # select count(c1) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(row_n))
        slt_file.write("\n")

        # select count(*) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(row_n))
        slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))

        # -------------------------------------
        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 SMALLINT, c2 TINYINT);\n".format(table_name)
        )

        # select count(*) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(0))
        slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',', FORMAT CSV );\n".format(
                table_name, copy_path
            )
        )

        sequence = np.arange(1, row_n + 1)

        # select max(c1) from test_simple_agg_big
        # c2 is tinyint
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT max(c2) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(pow(2, 7) - 1))
        slt_file.write("\n")

        # select min(c2) from test_simple_agg_big

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT min(c1) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(1))
        slt_file.write("\n")

        # select sum(c1) from test_simple_agg_big

        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT sum(c2) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(np.sum(np.arange(1, 128))))
        slt_file.write("\n")

        # select avg(c1) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT AVG(c2) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(np.around(np.sum(np.arange(1, 128)) / 9000, 6)))
        slt_file.write("\n")

        # select count(c1) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(c2) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(row_n))
        slt_file.write("\n")

        # select count(*) from test_simple_agg_big
        slt_file.write("\n")
        slt_file.write("query I\n")
        slt_file.write("SELECT count(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write(str(row_n))
        slt_file.write("\n")

        slt_file.write("\n")
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate limit data for test")

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
