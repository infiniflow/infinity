import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    limit_offset = [[10, 9000], [10, 10], [8, 9995], [9000, 1000]]
    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql"
    csv_name = "/test_big_top.csv"
    slt_name = "/big_top.slt"
    table_name = "test_big_top"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name
    copy_path = copy_dir + csv_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and generate_if_exists:
        print("File {} and {} already existed exists. Skip Generating.".format(
            slt_path, csv_path))
        return
    with (open(csv_path, "w") as top_csv_file, open(slt_path, "w") as top_slt_file):
        x = [i for i in range(row_n)]
        random.shuffle(x)
        for i in x:
            if i % 3 == 0:
                top_csv_file.write("{},true\n".format(i))
            else:
                top_csv_file.write("{},false\n".format(i))

        top_slt_file.write("statement ok\n")
        top_slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        top_slt_file.write("\n")
        top_slt_file.write("statement ok\n")
        top_slt_file.write(
            "CREATE TABLE {} (c1 integer, c2 boolean);\n".format(table_name))
        top_slt_file.write("\n")
        top_slt_file.write("statement ok\n")
        top_slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(table_name, copy_path))
        top_slt_file.write("\nquery I\n")
        top_slt_file.write(
            "SELECT * FROM {} order by c1 - c1, c2 desc, c1 + c1 limit 10 offset 3330;\n".format(table_name))
        top_slt_file.write("----\n")
        top_slt_file.write("9990 true\n9993 true\n9996 true\n9999 true\n")
        top_slt_file.write(
            "1 false\n2 false\n4 false\n5 false\n7 false\n8 false\n")

        for lim_off in limit_offset:
            limit = lim_off[0]
            offset = lim_off[1]
            top_slt_file.write("\nquery I\n")
            top_slt_file.write(
                "SELECT * FROM {} order by c1 limit {} offset {};\n".format(table_name, limit, offset))
            top_slt_file.write("----\n")

            limit = min(limit, row_n - offset)
            for j in range(limit):
                k = j + offset
                if k % 3 == 0:
                    top_slt_file.write("{} true\n".format(k))
                else:
                    top_slt_file.write("{} false\n".format(k))

        top_slt_file.write("\n")
        top_slt_file.write("statement ok\n")
        top_slt_file.write("DROP TABLE {};\n".format(table_name))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate top data for test")

    parser.add_argument("-g", "--generate", type=bool,
                        default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str,
                        default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
