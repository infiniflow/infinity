import os
import argparse
import random
from math import sin, cos, acos, pi
import numpy as np


def get_sphere_point(r, rand1, rand2):
    phi = rand1 * 2.0 * pi
    theta = acos(rand2 * 2.0 - 1.0)
    x = r * sin(theta) * cos(phi)
    y = r * sin(theta) * sin(phi)
    z = r * cos(theta)
    return np.array([x, y, z], dtype=np.float32)


row_n = 1000
query_data = [3, 500, 990, random.randint(0, row_n - 1)]
top_n = [5, 255, 256, row_n]
num_in_row = 10
radius = 10000.0
require_min_diff = 1e-6


def get_numpy_l2_norm(v1: np.ndarray, v2: np.ndarray):
    diff = v1 - v2
    return np.dot(diff, diff)


def get_random_data():
    while True:
        # generate random vectors at several positions
        # prepare row_n multivectors on sphere, with radius
        rand_1 = np.random.rand(row_n, 2)
        all_multivector_centers = np.zeros((row_n, 3), dtype=np.float32)
        for i in range(row_n):
            all_multivector_centers[i, :] = get_sphere_point(radius, rand_1[i][0], rand_1[i][1])
        all_multivectors = np.repeat(all_multivector_centers[:, np.newaxis, :], num_in_row, axis=1) + (
                np.random.rand(row_n, num_in_row, 3).astype(np.float32) - 0.5)
        assert all_multivectors.shape == (row_n, num_in_row, 3)
        assert all_multivectors.dtype == np.float32
        assert all_multivector_centers.shape == (row_n, 3)
        assert all_multivector_centers.dtype == np.float32
        # check query result for query_data, return if all query distances are different
        distance_results = []
        good_diff = True
        for i in query_data:
            query_v = all_multivector_centers[i]
            distance_pair = []
            for j in range(row_n):
                diff = all_multivectors[j] - query_v
                l2_d = np.sum(diff ** 2, axis=1)
                distance_pair.append((j, np.min(l2_d)))
            # sort by distance
            distance_pair.sort(key=lambda x: x[1])
            for j in range(row_n - 1):
                if (distance_pair[j + 1][1] - distance_pair[j][1]) / distance_pair[j + 1][1] < require_min_diff:
                    good_diff = False
                    break
            if not good_diff:
                break
            distance_results.append(distance_pair)
        if good_diff:
            return all_multivector_centers, all_multivectors, distance_results


def get_multivector_str_not_quoted(mv: np.ndarray):
    assert mv.ndim == 2
    assert mv.shape[1] == 3
    return '[' + (",".join([f"[{v[0]},{v[1]},{v[2]}]" for v in mv])) + ']'


def generate(generate_if_exists: bool, copy_dir: str):
    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/knn/multivector"
    csv_name = "/test_big_multivector_scan.csv"
    slt_name = "/big_multivector_scan.slt"
    slt_name_hnsw = "/big_multivector_hnsw.slt"
    table_name = "test_big_multivector_scan"
    table_name_hnsw = "test_big_multivector_hnsw"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name
    slt_path_hnsw = slt_dir + slt_name_hnsw
    copy_path = copy_dir + csv_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and os.path.exists(slt_path_hnsw) and not generate_if_exists:
        print("File {}, {} and {} already existed. Skip Generating.".format(slt_path, slt_path_hnsw, csv_path))
        return
    # generate data
    all_multivector_centers, all_multivectors, distance_results = get_random_data()
    # test method:
    # 1. insert row [0, 20)
    insert_val_1 = ",".join([f"({i},{get_multivector_str_not_quoted(all_multivectors[i])})" for i in range(20)])
    # 2. import row [20, 980)
    with open(csv_path, "w") as multivector_scan_csv_file:
        for i in range(20, 980):
            multivector_scan_csv_file.write(f'{i},"{get_multivector_str_not_quoted(all_multivectors[i])}"\n')
    # 3. insert row [980, 1000)
    insert_val_3 = ",".join([f"({i},{get_multivector_str_not_quoted(all_multivectors[i])})" for i in range(980, 1000)])
    # 4. dense scan for row in query_data
    with open(slt_path, "w") as multivector_scan_slt_file, open(slt_path_hnsw, "w") as multivector_hnsw_slt_file:
        def write_twice(statement: str, output_table_name: bool = False):
            multivector_scan_slt_file.write(statement.format(table_name) if output_table_name else statement)
            multivector_hnsw_slt_file.write(statement.format(table_name_hnsw) if output_table_name else statement)

        write_twice("statement ok\n")
        write_twice("DROP TABLE IF EXISTS {};\n", True)
        write_twice("\nstatement ok\n")
        write_twice("CREATE TABLE {} (c1 integer, c2 multivector(float,3));\n", True)
        # create hnsw index for table_name_hnsw
        multivector_hnsw_slt_file.write("\nstatement ok\n")
        multivector_hnsw_slt_file.write(
            f'CREATE INDEX idx ON {table_name_hnsw} (c2) USING HNSW WITH (M = 16, ef_construction = 200, metric = l2);\n')
        # 1. insert row [0, 20)
        write_twice("\nstatement ok\n")
        write_twice(f"INSERT INTO \u007b\u007d VALUES {insert_val_1};\n", True)
        # 2. import row [20, 980)
        write_twice("\nstatement ok\n")
        write_twice(f"COPY \u007b\u007d FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV );\n", True)
        # 3. insert row [980, 1000)
        write_twice("\nstatement ok\n")
        write_twice(f"INSERT INTO \u007b\u007d VALUES {insert_val_3};\n", True)
        # 4. dense scan for row in query_data
        write_twice("\n# multivector scan\n")
        for i, q_id in enumerate(query_data):
            write_twice("\nquery I\n")
            query_v = np.array2string(all_multivector_centers[q_id], separator=',')
            query_str = f"SELECT c1 FROM \u007b\u007d SEARCH MATCH VECTOR (c2, {query_v}, 'float', 'l2', {top_n[i]});\n"
            write_twice(query_str, True)
            write_twice("----\n")
            distance_result = distance_results[i]
            for j in range(top_n[i]):
                write_twice(f"{distance_result[j][0]}\n")
        # drop table
        write_twice("\nstatement ok\n")
        write_twice(f"DROP TABLE \u007b\u007d;\n", True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate data for multivector test")
    parser.add_argument("-g", "--generate", type=bool, default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str, default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
