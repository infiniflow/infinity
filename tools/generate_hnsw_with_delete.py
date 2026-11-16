# generate 'test/sql/dml/delete/test_delete_with_hnsw_big.slt' and 'test/data/csv/test_delete_with_hnsw_big.csv'


import argparse
import os
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
    csv_name = f"{table_name}.csv"
    slt_name = f"{table_name}.slt"

    copy_dir = "/var/infinity/test_data"
    copy_path = copy_dir + "/" + csv_name

    csv_path = csv_dir + "/" + csv_name
    slt_path = slt_dir + "/" + slt_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print(
            f"File {slt_path} and {csv_path} already existed exists. Skip Generating."
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
        csv_file.writelines(
            f"{to_csv_embedding(v_x)},{v_y}\n" for v_x, v_y in zip(x, y)
        )

    if copy:
        os.makedirs(copy_dir, exist_ok=True)
        os.system(f"cp {csv_path} {copy_path}")

    with open(slt_path, "w") as slt_file:
        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE TABLE {table_name} (c1 EMBEDDING(FLOAT, {embedding_len}), c2 INTEGER);\n"
        )
        slt_file.write("\n")

        slt_file.write("query I\n")
        slt_file.write(
            f"COPY {table_name} FROM '/var/infinity/test_data/{csv_name}' WITH ( DELIMITER ',', FORMAT CSV );\n"
        )
        slt_file.write("----\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DELETE FROM {table_name} WHERE c2 = 1;\n")
        slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(
            f"CREATE INDEX {index_name} ON {table_name}(c1) USING HNSW WITH (M = 100, ef_construction = 100, metric = l2);\n"
        )
        slt_file.write("\n")

        for v_x in x:
            slt_file.write("query I\n")
            slt_file.write(
                f"SELECT c1 FROM {table_name} SEARCH MATCH VECTOR (c1, {to_sql_embedding(v_x + diff)}, 'float', 'l2', 1) WITH (ef = 4);\n"
            )
            slt_file.write("----\n")
            if v_x in delete_x_1:
                slt_file.write(f"[{to_result_embedding(delete_nearest_1[v_x])}]\n")
            else:
                slt_file.write(f"[{to_result_embedding(v_x)}]\n")
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DELETE FROM {table_name} WHERE c2 = 2;\n")
        slt_file.write("\n")

        random.shuffle(x)
        for v_x in x:
            slt_file.write("query I\n")
            slt_file.write(
                f"SELECT c1 FROM {table_name} SEARCH MATCH VECTOR (c1, {to_sql_embedding(float(v_x) + diff)}, 'float', 'l2', 1) WITH (ef = 4);\n"
            )
            slt_file.write("----\n")
            if v_x in delete_x_all:
                slt_file.write(f"[{to_result_embedding(delete_nearest_all[v_x])}]\n")
            else:
                slt_file.write(f"[{to_result_embedding(v_x)}]\n")
            slt_file.write("\n")

        slt_file.write("statement ok\n")
        slt_file.write(f"DROP TABLE {table_name};\n")
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
