# generate 'test/sql/dml/import/test_import_big_sparse.slt' & 'test/sql/dml/import/test_insert_big_sparse.slt' & 'test/data/csv/test_sparse/big_sparse.csv'

import argparse
import os
from generate_util.generate_sparse_data import generate_sparse_data
from generate_util.format_data import format_float


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    max_dim = 30000
    sparsity = 0.01

    csv_dir = "./test/data/csv/test_sparse"
    import_slt_dir = "./test/sql/dml/import"
    insert_slt_dir = "./test/sql/dml/insert"

    table_name = "big_sparse_table"
    csv_filename = "big_sparse.csv"
    csv_path = csv_dir + "/" + csv_filename
    import_slt_path = import_slt_dir + "/test_import_big_sparse.slt"
    insert_slt_path = insert_slt_dir + "/test_insert_big_sparse.slt"
    copy_path = copy_dir + "/" + csv_filename

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(import_slt_dir, exist_ok=True)
    os.makedirs(insert_slt_dir, exist_ok=True)
    if (
        os.path.exists(csv_path)
        and os.path.exists(import_slt_path)
        and os.path.exists(insert_slt_path)
        and not generate_if_exists
    ):
        print(
            "File {} and {} and {} already existed. Skip Generating.".format(
                csv_path, import_slt_path, insert_slt_path
            )
        )
        return

    indptr, indices, data = generate_sparse_data(row_n, max_dim, sparsity)

    with open(import_slt_path, "w") as import_slt_file, open(csv_path, "w") as csv_file:
        import_slt_file.write("statement ok\n")
        import_slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        import_slt_file.write("\n")

        import_slt_file.write("statement ok\n")
        import_slt_file.write(
            "CREATE TABLE {} ( c1 INT, c2 SPARSE(FLOAT, {}) WITH (NOTSORT));\n".format(
                table_name, max_dim
            )
        )
        import_slt_file.write("\n")

        import_slt_file.write("statement ok\n")
        import_slt_file.write(
            "COPY {} FROM '{}' WITH ( DELIMITER ',');\n".format(table_name, copy_path)
        )
        import_slt_file.write("\n")

        import_slt_file.write("query I\n")
        import_slt_file.write("SELECT * FROM {};\n".format(table_name))
        import_slt_file.write("----\n")
        for row_id in range(row_n):
            start, end = indptr[row_id], indptr[row_id + 1]

            csv_file.write("{},".format(row_id))
            csv_file.write('"[')
            for j in range(start, end):
                csv_file.write("{}:{}".format(indices[j], data[j]))
                if j != end - 1:
                    csv_file.write(",")
            csv_file.write(']"\n')

            import_slt_file.write("{} ".format(row_id))
            for j in range(start, end):
                import_slt_file.write(
                    "{}: {}".format(indices[j], format_float(data[j])),
                )
                if j != end - 1:
                    import_slt_file.write(", ")
            import_slt_file.write("\n")

        import_slt_file.write("\n")
        import_slt_file.write("statement ok\n")
        import_slt_file.write("DROP TABLE {};\n".format(table_name))
        import_slt_file.write("\n")

    insert_batch = 10

    with open(insert_slt_path, "w") as insert_slt_file:
        insert_slt_file.write("statement ok\n")
        insert_slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        insert_slt_file.write("\n")

        insert_slt_file.write("statement ok\n")
        insert_slt_file.write(
            "CREATE TABLE {} ( c1 INT, c2 SPARSE(FLOAT, {}));\n".format(
                table_name, max_dim
            )
        )
        insert_slt_file.write("\n")

        for i in range(0, row_n, insert_batch):
            insert_slt_file.write("statement ok\n")
            insert_slt_file.write("INSERT INTO {} VALUES\n".format(table_name))
            for j in range(i, min(i + insert_batch, row_n)):
                start, end = indptr[j], indptr[j + 1]
                insert_slt_file.write("({},[".format(j))
                for k in range(start, end):
                    insert_slt_file.write(
                        "{}:{}".format(indices[k], format_float(data[k]))
                    )
                    if k != end - 1:
                        insert_slt_file.write(",")
                insert_slt_file.write("])")
                if j != min(i + insert_batch, row_n) - 1:
                    insert_slt_file.write(",\n")
            insert_slt_file.write(";\n")
            insert_slt_file.write("\n")

        insert_slt_file.write("query I\n")
        insert_slt_file.write("SELECT * FROM {};\n".format(table_name))
        insert_slt_file.write("----\n")
        for i in range(0, row_n):
            start, end = indptr[i], indptr[i + 1]
            insert_slt_file.write("{} ".format(i))
            for j in range(start, end):
                insert_slt_file.write(
                    "{}: {}".format(indices[j], format_float(data[j])),
                )
                if j != end - 1:
                    insert_slt_file.write(", ")
            insert_slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate fvecs data for test")
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
