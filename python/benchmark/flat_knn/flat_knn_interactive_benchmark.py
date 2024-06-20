import os
import sys
import inspect
import time
import infinity
from infinity.common import REMOTE_HOST, ConflictType
from infinity.errors import ErrorCode

current_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parent_dir = os.path.dirname(current_dir)
sys.path.insert(0, parent_dir + "/legacy_benchmark")
from remote_benchmark_knn import benchmark, read_groundtruth, calculate_recall_all
import numpy as np
import faiss


def infinity_import_sift_1m_no_index(path):
    infinity_obj = infinity.connect(REMOTE_HOST)
    assert infinity_obj
    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("sift_benchmark", ConflictType.Ignore)
    db_obj.create_table("sift_benchmark", {"col1": {"type": "vector,128,float"}})
    table_obj = db_obj.get_table("sift_benchmark")
    assert table_obj
    assert os.path.exists(path)
    start = time.time()
    res = table_obj.import_data(path, {'file_type': 'fvecs'})
    end = time.time()
    dur = end - start
    print(f"Import sift_1m cost time: {dur} s")
    assert res.error_code == ErrorCode.OK


def infinity_import_gist_1m_no_index(path):
    infinity_obj = infinity.connect(REMOTE_HOST)
    assert infinity_obj
    db_obj = infinity_obj.get_database("default_db")
    assert db_obj
    db_obj.drop_table("gist_benchmark", ConflictType.Ignore)
    db_obj.create_table("gist_benchmark", {"col1": {"type": "vector,960,float"}})
    table_obj = db_obj.get_table("gist_benchmark")
    assert table_obj
    assert os.path.exists(path)
    start = time.time()
    res = table_obj.import_data(path, {'file_type': 'fvecs'})
    end = time.time()
    dur = end - start
    print(f"Import gist_1m cost time: {dur} s")
    assert res.error_code == ErrorCode.OK


def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname):
    return ivecs_read(fname).view('float32')


class FlatKNNBenchmark:
    def __init__(self):
        self.all_data_sets = ["sift_1m", "gist_1m"]
        self.dataset_path_map = {}
        self.data_suffix = {"sift_1m": "/sift_base.fvecs", "gist_1m": "/gist_base.fvecs"}
        self.query_suffix = {"sift_1m": "/sift_query.fvecs", "gist_1m": "/gist_query.fvecs"}
        self.gt_suffix = {"sift_1m": "/sift_groundtruth.ivecs", "gist_1m": "/gist_groundtruth.ivecs"}
        self.query_top_k = {"sift_1m": 100, "gist_1m": 100}
        self.embedding_dim = {"sift_1m": 128, "gist_1m": 960}
        self.infinity_import_data_func_map = {"sift_1m": infinity_import_sift_1m_no_index,
                                              "gist_1m": infinity_import_gist_1m_no_index}
        self.faiss_index = {}

    def infinity_import(self, data_set):
        self.infinity_import_data_func_map[data_set](self.dataset_path_map[data_set] + self.data_suffix[data_set])

    def faiss_import(self, data_set):
        start = time.time()
        self.faiss_index[data_set] = faiss.IndexFlatL2(self.embedding_dim[data_set])
        print(self.faiss_index[data_set].is_trained)
        xb = fvecs_read(self.dataset_path_map[data_set] + self.data_suffix[data_set])
        self.faiss_index[data_set].add(xb)
        end = time.time()
        dur = end - start
        print(self.faiss_index[data_set].ntotal)
        print(f"faiss import cost time: {dur} s")

    def infinity_benchmark_flat_knn(self, data_set):
        threads = int(input("Enter number of threads:\n"))
        rounds = int(input("Enter number of rounds:\n"))
        benchmark(threads, rounds, data_set, self.dataset_path_map[data_set])

    def faiss_benchmark_flat_knn_batch_query(self, data_set):
        xq = fvecs_read(self.dataset_path_map[data_set] + self.query_suffix[data_set])
        start = time.time()
        D, I = self.faiss_index[data_set].search(xq, self.query_top_k[data_set])
        end = time.time()
        ground_truth_path = self.dataset_path_map[data_set] + self.gt_suffix[data_set]
        ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(ground_truth_path)
        recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                               ground_truth_sets_100, I)
        dur = end - start
        print(f"Total Dur: {dur:.2f} s")
        print(f"Query Count: {len(xq)}")
        print(f"QPS: {(len(xq) / dur):.2f}")
        print(f"Recall@1: {recall_1}")
        print(f"Recall@10: {recall_10}")
        print(f"Recall@100: {recall_100}")

    def faiss_benchmark_flat_knn(self, data_set):
        xq = fvecs_read(self.dataset_path_map[data_set] + self.query_suffix[data_set])
        dur = 0.0
        total_I = []
        for i in range(len(xq)):
            start = time.time()
            _, I = self.faiss_index[data_set].search(xq[i:i + 1, :], self.query_top_k[data_set])
            end = time.time()
            this_dur = end - start
            if i == 10:
                print(f"the 10th query cost time: {this_dur} s")
            dur += this_dur
            total_I.append(list(I[0]))
        ground_truth_path = self.dataset_path_map[data_set] + self.gt_suffix[data_set]
        ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(ground_truth_path)
        recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                               ground_truth_sets_100, total_I)
        print(f"Total Dur: {dur:.2f} s")
        print(f"Query Count: {len(xq)}")
        print(f"QPS: {(len(xq) / dur):.2f}")
        print(f"Recall@1: {recall_1}")
        print(f"Recall@10: {recall_10}")
        print(f"Recall@100: {recall_100}")

    def main_loop(self):
        while True:
            # take input from user
            data_set_idx = int(input("Enter data set: sift_1m(0), gist_1m(1), or exit(2).\n"))
            if data_set_idx == len(self.all_data_sets):
                break
            if data_set_idx > len(self.all_data_sets) or data_set_idx < 0:
                print(f"Invalid data set id: {data_set_idx}")
                continue
            data_set = self.all_data_sets[data_set_idx]
            if data_set not in self.dataset_path_map:
                self.dataset_path_map[data_set] = input(f"Enter path for {data_set}:\n")
            else:
                new_path = input(f"Enter optional new path for {data_set}:\n")
                if new_path:
                    self.dataset_path_map[data_set] = new_path

            client_target = int(input("Enter target: all(0), infinity(1), faiss(2), or exit(3).\n"))
            if client_target == 3:
                break
            if client_target > 3 or client_target < 0:
                print(f"Invalid client target {client_target}")
                continue

            operation = input("Enter operation: benchmark(b), import(i), or exit(e).\n")
            if operation[0] == "e":
                break
            elif operation[0] == "i":
                if client_target == 0 or client_target == 1:
                    self.infinity_import(data_set)
                if client_target == 0 or client_target == 2:
                    self.faiss_import(data_set)
            elif operation[0] == "b":
                if client_target == 0 or client_target == 1:
                    self.infinity_benchmark_flat_knn(data_set)
                if client_target == 0 or client_target == 2:
                    self.faiss_benchmark_flat_knn(data_set)
            else:
                print(f"Invalid operation {operation}")
                continue


if __name__ == '__main__':
    benchmark_obj = FlatKNNBenchmark()
    benchmark_obj.main_loop()
