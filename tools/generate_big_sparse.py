# generate 'test/sql/dml/import/test_import_big_sparse.slt' & 'test/sql/dml/import/test_insert_big_sparse.slt' & 'test/sql/dql/knn/sparse/test_knn_sparse_bmp_big.slt'
# genrate 'test/data/csv/test_sparse/big_sparse.csv'

import argparse
import os

from generate_util.format_data import sparse_format_float
from generate_util.generate_sparse_data import find_topk, generate_sparse_data


def generate(generate_if_exists: bool, copy_dir: str):
    row_n = 10000
    max_dim = 30000
    sparsity = 0.01

    csv_dir = "./test/data/csv/test_sparse"
    import_slt_dir = "./test/sql/dml/import"
    insert_slt_dir = "./test/sql/dml/insert"
    knn_slt_dir = "./test/sql/dql/knn/sparse"

    table_name = "big_sparse_table"
    csv_filename = "big_sparse.csv"
    csv_path = csv_dir + "/" + csv_filename
    import_slt_path = import_slt_dir + "/test_import_big_sparse.slt"
    insert_slt_path = insert_slt_dir + "/test_insert_big_sparse.slt"
    bmp_knn_slt_path = knn_slt_dir + "/test_knn_sparse_bmp_big.slt"
    copy_path = copy_dir + "/" + csv_filename

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(import_slt_dir, exist_ok=True)
    os.makedirs(insert_slt_dir, exist_ok=True)
    if (
        os.path.exists(csv_path)
        and os.path.exists(import_slt_path)
        and os.path.exists(insert_slt_path)
        and os.path.exists(bmp_knn_slt_path)
        and not generate_if_exists
    ):
        print(
            f"File {csv_path} and {import_slt_path} and {insert_slt_path} and {bmp_knn_slt_path} already existed. Skip Generating."
        )
        return

    indptr, indices, data = generate_sparse_data(row_n, max_dim, sparsity)

    with open(import_slt_path, "w") as import_slt_file, open(csv_path, "w") as csv_file:
        import_slt_file.write("statement ok\n")
        import_slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        import_slt_file.write("\n")

        import_slt_file.write("statement ok\n")
        import_slt_file.write(
            f"CREATE TABLE {table_name} ( c1 INT, c2 SPARSE(FLOAT, {max_dim}) WITH (SORTED));\n"
        )
        import_slt_file.write("\n")

        import_slt_file.write("statement ok\n")
        import_slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV);\n"
        )
        import_slt_file.write("\n")

        import_slt_file.write("query I\n")
        import_slt_file.write(f"SELECT * FROM {table_name};\n")
        import_slt_file.write("----\n")
        for row_id in range(row_n):
            start, end = indptr[row_id], indptr[row_id + 1]

            csv_file.write(f"{row_id},")
            csv_file.write('"[')
            for j in range(start, end):
                csv_file.write(f"{indices[j]}:{data[j]}")
                if j != end - 1:
                    csv_file.write(",")
            csv_file.write(']"\n')

            import_slt_file.write(f"{row_id} [")
            for j in range(start, end):
                import_slt_file.write(
                    f"{indices[j]}:{sparse_format_float(data[j])}",
                )
                if j != end - 1:
                    import_slt_file.write(",")
            import_slt_file.write("]\n")

        import_slt_file.write("\n")
        import_slt_file.write("statement ok\n")
        import_slt_file.write(f"DROP TABLE {table_name};\n")
        import_slt_file.write("\n")

    insert_batch = 10

    with open(insert_slt_path, "w") as insert_slt_file:
        insert_slt_file.write("statement ok\n")
        insert_slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        insert_slt_file.write("\n")

        insert_slt_file.write("statement ok\n")
        insert_slt_file.write(
            f"CREATE TABLE {table_name} ( c1 INT, c2 SPARSE(FLOAT, {max_dim}));\n"
        )
        insert_slt_file.write("\n")

        for i in range(0, row_n, insert_batch):
            insert_slt_file.write("statement ok\n")
            insert_slt_file.write(f"INSERT INTO {table_name} VALUES\n")
            for j in range(i, min(i + insert_batch, row_n)):
                start, end = indptr[j], indptr[j + 1]
                insert_slt_file.write(f"({j},[")
                for k in range(start, end):
                    insert_slt_file.write(
                        f"{indices[k]}:{sparse_format_float(data[k])}"
                    )
                    if k != end - 1:
                        insert_slt_file.write(",")
                insert_slt_file.write("])")
                if j != min(i + insert_batch, row_n) - 1:
                    insert_slt_file.write(",\n")
            insert_slt_file.write(";\n")
            insert_slt_file.write("\n")

        insert_slt_file.write("query I\n")
        insert_slt_file.write(f"SELECT * FROM {table_name};\n")
        insert_slt_file.write("----\n")
        for i in range(row_n):
            start, end = indptr[i], indptr[i + 1]
            insert_slt_file.write(f"{i} [")
            for j in range(start, end):
                insert_slt_file.write(
                    f"{indices[j]}:{sparse_format_float(data[j])}",
                )
                if j != end - 1:
                    insert_slt_file.write(",")
            insert_slt_file.write("]\n")

    index_name = "bmp_index"
    topk = 3
    query_n = 10
    qsparsity = 0.005
    qindptr, qindices, qdata = generate_sparse_data(query_n, max_dim, qsparsity)

    with open(bmp_knn_slt_path, "w") as bmp_knn_slt_file:
        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        bmp_knn_slt_file.write("\n")

        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(
            f"CREATE TABLE {table_name} ( c1 INT, c2 SPARSE(FLOAT, {max_dim}));\n"
        )
        bmp_knn_slt_file.write("\n")

        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(
            f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV);\n"
        )
        bmp_knn_slt_file.write("\n")

        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(
            f"CREATE INDEX {index_name} ON {table_name} (c2) USING Bmp WITH (block_size = 8, compress_type = compress);\n"
        )
        bmp_knn_slt_file.write("\n")

        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(
            f"ALTER {index_name} ON {table_name} WITH (bp_reorder, topk = {topk});\n"
        )
        bmp_knn_slt_file.write("\n")

        for i in range(query_n):
            bmp_knn_slt_file.write("query I\n")

            start, end = qindptr[i], qindptr[i + 1]
            bmp_knn_slt_file.write(
                "SELECT c1 FROM {} SEARCH MATCH SPARSE (c2, [{}], 'ip', {}) WITH (alpha = 1.0, beta = 1.0);\n".format(
                    table_name,
                    ",".join(
                        [
                            f"{i}:{d}"  # sparse_format_float(d)
                            for (i, d) in zip(qindices[start:end], qdata[start:end])
                        ]
                    ),
                    topk,
                )
            )
            res = find_topk(
                indptr, indices, data, topk, qindices[start:end], qdata[start:end]
            )
            bmp_knn_slt_file.write("----\n")
            bmp_knn_slt_file.writelines(f"{r}\n" for r in res)
            bmp_knn_slt_file.write("\n")

        bmp_knn_slt_file.write("statement ok\n")
        bmp_knn_slt_file.write(f"DROP TABLE {table_name};\n")
        bmp_knn_slt_file.write("\n")


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
