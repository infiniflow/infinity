import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 20000
    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/index_scan"
    csv_name = "/test_big_index_scan.csv"
    slt_name = "/big_index_scan.slt"
    table_name = "test_big_index_scan"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name
    copy_path = copy_dir + csv_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print("File {} and {} already existed exists. Skip Generating.".format(
            slt_path, csv_path))
        return
    with (open(csv_path, "w") as index_scan_csv_file, open(slt_path, "w") as index_scan_slt_file):
        x = [i for i in range(row_n)]
        random.shuffle(x)
        for i in x:
            j = ((i + 128) % 256) - 128
            k = i % 7
            index_scan_csv_file.write("{},{},{}\n".format(i, j, k))

        index_scan_slt_file.write("statement ok\n")
        index_scan_slt_file.write(
            "DROP TABLE IF EXISTS {};\n".format(table_name))

        index_scan_slt_file.write("\nstatement ok\n")
        index_scan_slt_file.write(
            "CREATE TABLE {} (c1 integer, mod_256_min_128 tinyint, mod_7 tinyint);\n".format(table_name))

        index_scan_slt_file.write("\nstatement ok\n")
        index_scan_slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(table_name, copy_path))

        index_scan_slt_file.write("\nstatement ok\n")
        index_scan_slt_file.write(
            "CREATE INDEX idx_c1 on {}(c1);\n".format(table_name))

        index_scan_slt_file.write("\n# index scan\n")
        index_scan_slt_file.write("query I\n")
        index_scan_slt_file.write(
            "SELECT * FROM {} WHERE (c1 < 5) OR (c1 > 10000 AND c1 < 10005) OR c1 = 19990 ORDER BY c1;\n".format(
                table_name))
        index_scan_slt_file.write("----\n")
        index_scan_slt_file.write(
            "0 0 0\n1 1 1\n2 2 2\n3 3 3\n4 4 4\n10001 17 5\n10002 18 6\n10003 19 0\n10004 20 1\n19990 22 5\n")

        index_scan_slt_file.write("\n# both index scan and ordinary filter\n")
        index_scan_slt_file.write("query II\n")
        index_scan_slt_file.write(
            "SELECT * FROM {} WHERE ((c1 < 5) OR (c1 > 10000 AND c1 < 10005) OR c1 = 19990) AND NOT mod_7 = 1 ORDER BY c1;\n".format(
                table_name))
        index_scan_slt_file.write("----\n")
        index_scan_slt_file.write(
            "0 0 0\n2 2 2\n3 3 3\n4 4 4\n10001 17 5\n10002 18 6\n10003 19 0\n19990 22 5\n")

        index_scan_slt_file.write("\nstatement ok\n")
        index_scan_slt_file.write(
            "CREATE INDEX idx_mod_7 on {}(mod_7);\n".format(table_name))

        index_scan_slt_file.write("\n# index scan with 2 index\n")
        index_scan_slt_file.write("query III\n")
        index_scan_slt_file.write(
            "SELECT * FROM {} WHERE ((c1 < 5) OR (c1 > 10000 AND c1 < 10005) OR c1 = 19990) AND (mod_7 > 3 AND mod_7 < 6) ORDER BY c1;\n".format(
                table_name))
        index_scan_slt_file.write("----\n")
        index_scan_slt_file.write("4 4 4\n10001 17 5\n19990 22 5\n")

        index_scan_slt_file.write("\n# index scan large output (8996 rows)\n")
        index_scan_slt_file.write("query IV\n")
        index_scan_slt_file.write(
            "SELECT * FROM {} WHERE (c1 >= 5) AND (c1 <= 9000) ORDER BY c1;\n".format(table_name))
        index_scan_slt_file.write("----\n")
        # from 5 to 9000 (included), 8996 rows
        for i in range(5, 9001):
            index_scan_slt_file.write("{} {} {}\n".format(
                i, ((i + 128) % 256) - 128, i % 7))

        index_scan_slt_file.write("\nstatement ok\n")
        index_scan_slt_file.write("DROP TABLE {};\n".format(table_name))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate top data for test")

    parser.add_argument("-g", "--generate", type=bool,
                        default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str,
                        default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
