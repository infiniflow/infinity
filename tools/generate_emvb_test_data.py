import os
import argparse
import random


def generate(generate_if_exists: bool, copy_dir: str):
    fix_embedding_num_in_tensor = 32
    fix_dim = 128
    row_n = 1024
    pq_subspace_num = 32
    pq_subspace_bits = 8
    csv_dir = "./test/data/csv"
    slt_dir = "./test/sql/dql/knn/tensor"
    csv_name = "/test_emvb_index_big.csv"
    slt_name = "/test_emvb_index_big.slt"
    table_name = "test_emvb_index_big"

    csv_path = csv_dir + csv_name
    slt_path = slt_dir + slt_name
    copy_path = copy_dir + csv_name

    os.makedirs(csv_dir, exist_ok=True)
    os.makedirs(slt_dir, exist_ok=True)
    if os.path.exists(csv_path) and os.path.exists(slt_path) and not generate_if_exists:
        print("File {} and {} already existed exists. Skip Generating.".format(slt_path, csv_path))
        return
    with (open(csv_path, "w") as top_csv_file, open(slt_path, "w") as top_slt_file):
        for i in range(row_n):
            t = []
            for j in range(fix_embedding_num_in_tensor):
                tt = [random.uniform(-1, 1) for _ in range(fix_dim)]
                tt[i % fix_dim] += i + j
                t.append(tt)
            top_csv_file.write('{},"{}"\n'.format(i, t))

        top_slt_file.write("statement ok\n")
        top_slt_file.write("DROP TABLE IF EXISTS {};\n".format(table_name))
        top_slt_file.write("\n")
        top_slt_file.write("statement ok\n")
        top_slt_file.write("CREATE TABLE {} (c1 integer, c2 TENSOR(FLOAT, {}));\n".format(table_name, fix_dim))
        top_slt_file.write("\n")
        top_slt_file.write("statement ok\n")
        top_slt_file.write("COPY {} FROM '{}' WITH ( DELIMITER ',' );\n".format(table_name, copy_path))
        top_slt_file.write("\nstatement ok\n")
        top_slt_file.write("CREATE INDEX idx1 ON {} (c2) USING EMVB WITH ".format(table_name))
        top_slt_file.write("(pq_subspace_num = {}, pq_subspace_bits = {});\n".format(pq_subspace_num, pq_subspace_bits))
        query_vec = [0] * fix_dim
        query_vec[0] = 1
        query_vec[1] = 1
        query_vec[2] = 1
        query_vec[3] = 1
        top_slt_file.write("\n# test index search")
        top_slt_file.write("\nstatement ok\n")
        top_slt_file.write("SELECT c1 FROM {} SEARCH MATCH TENSOR".format(table_name))
        top_slt_file.write(" (c2, {}, 'float', 'maxsim', 'topn=10');\n".format(query_vec))
        top_slt_file.write("\nstatement ok\n")
        top_slt_file.write("SELECT c1 FROM {} SEARCH MATCH TENSOR (c2, {}".format(table_name, query_vec))
        top_slt_file.write(", 'float', 'maxsim', 'topn=10;emvb_threshold_first=0.4;emvb_threshold_final=0.5');\n")
        top_slt_file.write("\n# test small mem index of exhaustive scan")
        insert_vec = [0] * fix_dim
        insert_vec[0] = 2 * row_n
        insert_vec[1] = 2 * row_n
        insert_vec[2] = 2 * row_n
        insert_vec[3] = 2 * row_n
        top_slt_file.write("\nstatement ok\n")
        top_slt_file.write("INSERT INTO {} VALUES ({}, {});\n".format(table_name, row_n, insert_vec))
        top_slt_file.write("\nquery I\n")
        top_slt_file.write("SELECT c1 FROM {} SEARCH MATCH TENSOR".format(table_name))
        top_slt_file.write(" (c2, {}, 'float', 'maxsim', 'topn=1');\n".format(query_vec))
        top_slt_file.write("----\n")
        top_slt_file.write("{}\n".format(row_n))
        top_slt_file.write("\nstatement ok\n")
        top_slt_file.write("DROP TABLE {};\n".format(table_name))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate test data for emvb")

    parser.add_argument("-g", "--generate", type=bool, default=False, dest="generate_if_exists", )
    parser.add_argument("-c", "--copy", type=str, default="/var/infinity/test_data", dest="copy_dir", )
    args = parser.parse_args()
    generate(args.generate_if_exists, args.copy_dir)
