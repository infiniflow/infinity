import os
import argparse
import random
from math import sin, cos, acos, pi
import numpy as np


def get_sphere_point(r: float):
    phi = random.uniform(0, 2 * pi)
    theta = acos(random.uniform(-1, 1))
    x = r * sin(theta) * cos(phi)
    y = r * sin(theta) * sin(phi)
    z = r * cos(theta)
    return [x, y, z]


row_n = 100
top_n = 5
num_in_row = 10
radius = 1000.0
query_data = [1, 50, 97]
require_min_diff = 1e-6


def get_numpy_l2_norm(v1: np.ndarray, v2: np.ndarray):
    diff = v1 - v2
    return np.dot(diff, diff)


def get_random_data():
    while True:
        # generate random vectors at several positions
        # prepare row_n multivectors on sphere, with radius
        all_multivector_centers = [get_sphere_point(radius) for _ in range(row_n)]
        all_multivectors = []
        for i in range(row_n):
            append_mv = []
            c = all_multivector_centers[i]
            # generate num_in_row vectors around the center
            for j in range(num_in_row):
                append_mv.append(
                    [c[0] + random.uniform(-1, 1), c[1] + random.uniform(-1, 1), c[2] + random.uniform(-1, 1)])
            all_multivectors.append(append_mv)
        all_multivector_centers = np.array(all_multivector_centers, dtype=np.float32)
        all_multivectors = np.array(all_multivectors, dtype=np.float32)
        assert all_multivector_centers.shape == (row_n, 3)
        assert all_multivectors.shape == (row_n, num_in_row, 3)
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
            for j in range(top_n):
                if distance_pair[j + 1][1] - distance_pair[j][1] < require_min_diff:
                    good_diff = False
                    break
            if not good_diff:
                break
            distance_results.append(distance_pair[:top_n])
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
    table_name = "test_big_multivector_scan"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name
    copy_path = copy_dir + csv_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print("File {} and {} already existed exists. Skip Generating.".format(slt_path, csv_path))
        return
    # generate data
    all_multivector_centers, all_multivectors, distance_results = get_random_data()
    # test method:
    # 1. insert row 0 - 4
    insert_val_1 = ",".join([f"({i},{get_multivector_str_not_quoted(all_multivectors[i])})" for i in range(5)])
    # 2. import row 5 - 94
    with open(csv_path, "w") as multivector_scan_csv_file:
        # write row 5 - 94
        for i in range(5, 95):
            multivector_scan_csv_file.write(f'{i},"{get_multivector_str_not_quoted(all_multivectors[i])}"\n')
    # 3. insert row 95 - 99
    insert_val_3 = ",".join([f"({i},{get_multivector_str_not_quoted(all_multivectors[i])})" for i in range(95, 100)])
    # 4. dense scan for row in query_data
    with open(slt_path, "w") as multivector_scan_slt_file:
        multivector_scan_slt_file.write("statement ok\n")
        multivector_scan_slt_file.write(f"DROP TABLE IF EXISTS {table_name};\n")
        multivector_scan_slt_file.write("\nstatement ok\n")
        multivector_scan_slt_file.write(f"CREATE TABLE {table_name} (c1 integer, c2 multivector(float,3));\n")
        # 1. insert row 0 - 4
        multivector_scan_slt_file.write("\nstatement ok\n")
        multivector_scan_slt_file.write(f"INSERT INTO {table_name} VALUES {insert_val_1};\n")
        # 2. import row 5 - 94
        multivector_scan_slt_file.write("\nstatement ok\n")
        multivector_scan_slt_file.write(f"COPY {table_name} FROM '{copy_path}' WITH ( DELIMITER ',', FORMAT CSV );\n")
        # 3. insert row 95 - 99
        multivector_scan_slt_file.write("\nstatement ok\n")
        multivector_scan_slt_file.write(f"INSERT INTO {table_name} VALUES {insert_val_3};\n")
        # 4. dense scan for row 1, 50, 97
        multivector_scan_slt_file.write("\n# multivector scan\n")
        for i, q_id in enumerate(query_data):
            multivector_scan_slt_file.write("\nquery I\n")
            query_v = np.array2string(all_multivector_centers[q_id], separator=',')
            query_str = f"SELECT c1 FROM {table_name} SEARCH MATCH VECTOR (c2, {query_v}, 'float', 'l2', {top_n});\n"
            multivector_scan_slt_file.write(query_str)
            multivector_scan_slt_file.write("----\n")
            distance_result = distance_results[i]
            for j in range(top_n):
                multivector_scan_slt_file.write(f"{distance_result[j][0]}\n")
        # drop table
        multivector_scan_slt_file.write("\nstatement ok\n")
        multivector_scan_slt_file.write(f"DROP TABLE {table_name};\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate data for multivector test")
    parser.add_argument("-g", "--generate", type=bool, default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str, default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
