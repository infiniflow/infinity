# generate 'test/sql/dml/delete/test_delete_with_hnsw_big.slt' and 'test/data/csv/test_delete_with_hnsw_big.csv'


import os
import argparse
import random


def generate(generate_if_exists: bool, copy: bool):
    row_n = 10
    table_name = "test_delete_with_hnsw_big"
    index_name = "hnsw_index"
    diff = 0.1
    p = 2  # 1/p is the probability of a row being deleted
    embedding_len = 10

    def to_result_embedding(v: float) -> str:
        return ",".join([str(v) for i in range(embedding_len)])

    def to_sql_embedding(v: float) -> str:
        return to_result_embedding(v).join(["[", "]"])

    def to_csv_embedding(v: float) -> str:
        return to_sql_embedding(v).join(['"', '"'])

    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dml/delete"
    csv_name = "{}.csv".format(table_name)
    slt_name = "{}.slt".format(table_name)

    copy_dir = "/var/infinity/test_data"
    copy_path = copy_dir + "/" + csv_name

    csv_path = csv_dir + "/" + csv_name
    slt_path = slt_dir + "/" + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            "File {} and {} already existed exists. Skip Generating.".format(
                slt_path, csv_path
            )
        )
        return

    x = [i for i in range(row_n)]
    random.shuffle(x)

    delete_n_all = row_n // p
    delete_n_1 = delete_n_all // 2

    delete_x_all_list = random.sample(range(row_n), delete_n_all)
    delete_x_1 = {
        delete_x_all_list[i] for i in random.sample(range(delete_n_all), delete_n_1)
    }
    delete_x_all = set(delete_x_all_list)

    y = [1 if v in delete_x_1 else (2 if v in delete_x_all else 0) for v in x]

    def find_nearest(delete_set: set[int]) -> dict[int, int]:
        delete_nearest = {}
        for v_d in delete_set:
            v_after = v_d + 1
            while v_after in delete_set:
                v_after += 1
            v_before = v_d - 1
            while v_before in delete_set:
                v_before -= 1
            if v_after >= row_n:
                assert v_before >= 0
                delete_nearest[v_d] = v_before
            elif v_before < 0:
                delete_nearest[v_d] = v_after
            else:
                delete_nearest[v_d] = (
                    v_before if v_d - v_before < v_after - v_d else v_after
                )
        return delete_nearest

    delete_nearest_1 = find_nearest(delete_x_1)
    delete_nearest_all = find_nearest(delete_x_all)

    with open(csv_path, "w") as csv_file:
        for v_x, v_y in zip(x, y):
            csv_file.write("{},{}\n".format(to_csv_embedding(v_x), v_y))

    if copy:
        os.makedirs(copy_dir, exist_ok=True)
        os.system("cp {} {}".format(csv_path, copy_path))

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE TABLE {} (c1 EMBEDDING(FLOAT, {}), c2 INTEGER);\n".format(
                table_name, embedding_len
            )
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            "COPY {} FROM '/var/infinity/test_data/{}' WITH ( DELIMITER ',' );\n".format(
                table_name, csv_name
            )
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DELETE FROM {} WHERE c2 = 1;\n".format(table_name))
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            "CREATE INDEX {} ON {}(c1) USING HNSW WITH (M = 100, ef_construction = 100, ef = 100, metric = l2);\n".format(
                index_name, table_name
            )
        )
        slt_file.write("\n")

        for v_x in x:
            slt_file.write("query I\n")
            slt_file.write(
                "SELECT c1 FROM {} SEARCH KNN(c1, {}, 'float', 'l2', 1) WITH (ef = 4);\n".format(
                    table_name, to_sql_embedding(v_x + diff)
                )
            )
            slt_file.write("----\n")
            if v_x in delete_x_1:
                slt_file.write(
                    "{}\n".format(to_result_embedding(delete_nearest_1[v_x]))
                )
            else:
                slt_file.write("{}\n".format(to_result_embedding(v_x)))
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write("DELETE FROM {} WHERE c2 = 2;\n".format(table_name))
        slt_file.write("\n")

        random.shuffle(x)
        for v_x in x:
            slt_file.write("query I\n")
            slt_file.write(
                "SELECT c1 FROM {} SEARCH KNN(c1, {}, 'float', 'l2', 1) WITH (ef = 4);\n".format(
                    table_name, to_sql_embedding(float(v_x) + diff)
                )
            )
            slt_file.write("----\n")
            if v_x in delete_x_all:
                slt_file.write(
                    "{}\n".format(to_result_embedding(delete_nearest_all[v_x]))
                )
            else:
                slt_file.write("{}\n".format(to_result_embedding(v_x)))
            slt_file.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate top data for test")

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
        type=bool,
        default=True,
        dest="copy_dir",
    )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
