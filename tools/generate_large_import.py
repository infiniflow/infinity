# generate 'test/sql/dml/import/test_slt_import_more_than_one_segment.slt'
# generate 'test/data/csv/test_slt_import_more_than_one_segment.csv'

import os
import argparse


def generate1(generate_if_exists: bool, copy_dir: str):
    row_n = 8192 * 1024 + 1
    table_name = "test_slt_import_more_than_one_segment"

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/import"
    csv_name = "/{}.csv".format(table_name)
    slt_name = "/{}.slt".format(table_name)

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, csv_path
            )
        )
        return

    with open(csv_path, "w") as csv_file:
        for row in range(row_n):
            csv_file.write(f"{row}\n")

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 INTEGER);\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '{}/{}' WITH ( DELIMITER ',', FORMAT CSV );\n".format(
                table_name, copy_dir, csv_name
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write("SELECT COUNT(*) FROM {};\n".format(table_name))
        slt_file.write("----\n")
        slt_file.write("{}\n".format(row_n))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("SHOW TABLE {} SEGMENT 0;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("SHOW TABLE {} SEGMENT 1;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE {};\n".format(table_name))
        slt_file.write("\n")

    pass

def generate_big_columns_csv(num, filename):
    with open(os.getcwd() + "/test/data/csv/" + filename, "w") as f:
        data = "".join(str(i) + "," for i in range(num - 1))
        data += str(num - 1)
        f.write(data)
    f.close()

def generate_big_int_csv(num, filename):
    with open(os.getcwd() + "/test/data/csv/" + filename, "w") as f:
        for i in range(num):
            f.write(str(i) + "," + str(i) + "\n")
    f.close()

def generate_big_rows_csv(num, filename):
    with open(os.getcwd() + "/test/data/csv/" + filename, "w") as f:
        for i in range(num):
            f.write(str(i) + ",asdasdlk中fjio@!#!@asd #$%$23\n")
    f.close()

def generate(generate_if_exists: bool, copy_dir: str):
    generate1(generate_if_exists, copy_dir)
    generate_big_columns_csv(1024, "pysdk_test_big_columns.csv")
    generate_big_int_csv(10000, "pysdk_test_big_int.csv")
    for data_size in [1, 8191, 8192, 8193, 16 * 819]:
        generate_big_rows_csv(data_size, "pysdk_test_import_with_different_size.csv")
    generate_big_rows_csv(1024 * 8192, "pysdk_test_big_varchar_rows.csv")
    generate_big_rows_csv(1024 * 8192 + 1, "test_sdk_import_more_than_one_segment.csv")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate more than 8192 * 1024 rows for testing import")

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
