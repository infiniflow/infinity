import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    repeat_n = 100
    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/filter"
    csv_name = "/test_big_top.csv"
    table_name1 = "test_big_point_query_bloom_filter_miss"
    table_name2 = "test_big_point_query_bloom_filter_exist"
    slt_name1 = "/big_point_query_bloom_filter_miss.slt"
    slt_name2 = "/big_point_query_bloom_filter_exist.slt"

    copy_path = copy_dir + csv_name
    slt_path1 = slt_dir + slt_name1
    slt_path2 = slt_dir + slt_name2

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(slt_path1) and os.path.exists(slt_path2) and generate_if_exists:
        print("File {} and {} already existed exists. Skip Generating.".format(
            slt_path1, slt_path2))
        return
    with (open(slt_path1, "w") as test_slt_file1, open(slt_path2, "w") as test_slt_file2):
        test_slt_file1.write("statement ok\n")
        test_slt_file1.write("DROP TABLE IF EXISTS {};\n".format(table_name1))
        test_slt_file1.write("\n")
        test_slt_file1.write("statement ok\n")
        test_slt_file1.write(
            "CREATE TABLE {} (c1 integer, c2 boolean) PROPERTIES (bloom_filter_columns = \"c1,c2\");\n".format(
                table_name1))
        test_slt_file1.write("\n")
        test_slt_file1.write("statement ok\n")
        test_slt_file1.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(table_name1, copy_path))

        for i in range(repeat_n):
            x = random.randint(row_n + 1, row_n * 3)
            y = random.randint(row_n + 1, row_n * 3)
            z = random.randint(row_n + 1, row_n * 3)
            test_slt_file1.write("\nquery I\n")
            test_slt_file1.write(
                "SELECT * FROM {} where c1 = {} or c1 = {} or c1 = {};\n".format(table_name1, x, y, z))
            test_slt_file1.write("----\n")

        test_slt_file1.write("\n")
        test_slt_file1.write("statement ok\n")
        test_slt_file1.write("DROP TABLE {};\n".format(table_name1))

        test_slt_file2.write("statement ok\n")
        test_slt_file2.write("DROP TABLE IF EXISTS {};\n".format(table_name2))
        test_slt_file2.write("\n")
        test_slt_file2.write("statement ok\n")
        test_slt_file2.write(
            "CREATE TABLE {} (c1 integer, c2 boolean) PROPERTIES (bloom_filter_columns = \"c1,c2\");\n".format(
                table_name2))
        test_slt_file2.write("\n")
        test_slt_file2.write("statement ok\n")
        test_slt_file2.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(table_name2, copy_path))

        for i in range(repeat_n):
            x = random.randint(0, row_n - 1)
            test_slt_file2.write("\nquery I\n")
            test_slt_file2.write(
                "SELECT * FROM {} where c1 = {};\n".format(table_name2, x))
            test_slt_file2.write("----\n")
            test_slt_file2.write("{} {}\n".format(
                x, "true" if x % 3 == 0 else "false"))

        test_slt_file2.write("\n")
        test_slt_file2.write("statement ok\n")
        test_slt_file2.write("DROP TABLE {};\n".format(table_name2))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate point query test data")

    parser.add_argument("-g", "--generate", type=bool,
                        default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str,
                        default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
