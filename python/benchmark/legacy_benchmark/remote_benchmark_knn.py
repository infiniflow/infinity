# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import functools
import multiprocessing
import os
import struct
import time
import traceback

import infinity
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.common import LOCAL_HOST, LOCAL_INFINITY_PATH

def fvecs_read_all(filename):
    vectors = []
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}f'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                vectors.append(list(vec))
            except struct.error:
                break
    return vectors


def read_groundtruth(filename):
    vectors = []
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}i'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                vectors.append(list(vec))
            except struct.error:
                break

    # print("len(vectors): ", len(vectors))

    gt_count = len(vectors)
    gt_top_k = len(vectors[0])

    ground_truth_sets_1 = [set() for _ in range(gt_count)]
    ground_truth_sets_10 = [set() for _ in range(gt_count)]
    ground_truth_sets_100 = [set() for _ in range(gt_count)]

    for i in range(gt_count):
        for j in range(gt_top_k):
            x = vectors[i][j]
            if j < 1:
                ground_truth_sets_1[i].add(x)
            if j < 10:
                ground_truth_sets_10[i].add(x)
            if j < 100:
                ground_truth_sets_100[i].add(x)

    return ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100


def calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100, query_results):
    correct_1 = 0.0
    correct_10 = 0.0
    correct_100 = 0.0

    for query_idx in range(len(query_results)):
        result = query_results[query_idx]
        ground_truth_1 = ground_truth_sets_1[query_idx]
        ground_truth_10 = ground_truth_sets_10[query_idx]
        ground_truth_100 = ground_truth_sets_100[query_idx]

        for i in range(len(result)):
            if i < 1 and result[i] in ground_truth_1:
                correct_1 += 1.0
            if i < 10 and result[i] in ground_truth_10:
                correct_10 += 1.0
            if i < 100 and result[i] in ground_truth_100:
                correct_100 += 1.0

    recall_1 = correct_1 / (len(query_results) * 1)
    recall_10 = correct_10 / (len(query_results) * 10)
    recall_100 = correct_100 / (len(query_results) * 100)
    return recall_1, recall_10, recall_100


def trace_unhandled_exceptions(func):
    @functools.wraps(func)
    def wrapped_func(*args, **kwargs):
        try:
            func(*args, **kwargs)
        except:
            print('Exception in ' + func.__name__)
            traceback.print_exc()

    return wrapped_func


@trace_unhandled_exceptions
def work(queries, topk, metric_type, column_name, data_type,ef: int, remote: bool, table_name="sift_benchmark"):
    infinity_obj = None
    if remote:
        infinity_obj = infinity.connect(LOCAL_HOST)
    else:
        infinity_obj = infinity.connect(LOCAL_INFINITY_PATH)
    table = infinity_obj.get_database("default_db").get_table(table_name)
    for query in queries:
        # print(len(query))
        # table.match_dense(column_name, query_vec, data_type, metric_type, topk).output(["_row_id"]).to_result()
        query_builder = InfinityThriftQueryBuilder(table)
        query_builder.output(["_row_id"])
        query_builder.match_dense(column_name, query, data_type, metric_type, topk, {"ef": str(ef)})
        query_builder.to_result()
    infinity_obj.disconnect()


def fvecs_read(filename):
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}f'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                yield list(vec)
            except struct.error:
                break


def process_pool(threads, rounds, query_path, ef: int, remote: bool, table_name):
    if not os.path.exists(query_path):
        print(f"File: {query_path} doesn't exist")
        raise Exception(f"File: {query_path} doesn't exist")

    results = []

    total_queries = fvecs_read_all(query_path)
    queries = [[] for i in range(threads)]
    total_queries_count = len(total_queries)
    print(f"Total Query Count: {total_queries_count}")
    for i, query in enumerate(total_queries):
        queries[i % threads].append(query)

    for i in range(rounds):
        p = multiprocessing.Pool(threads)
        start = time.time()
        for idx in range(threads):
            p.apply_async(work, args=(queries[idx], 100, "l2", "col1", "float", ef, remote, table_name))
        p.close()
        p.join()
        end = time.time()
        dur = end - start
        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur:.2f} s")
        results.append(f"Query Count: {total_queries_count}")
        results.append(f"QPS: {(total_queries_count / dur):.2f}")

    for result in results:
        print(result)


def one_thread(rounds, query_path, ground_truth_path, ef: int, remote: bool, table_name):
    if not os.path.exists(query_path):
        print(f"File: {query_path} doesn't exist")
        raise Exception(f"File: {query_path} doesn't exist")

    results = []
    queries = fvecs_read_all(query_path)

    infinity_obj = None
    if remote:
        infinity_obj = infinity.connect(LOCAL_HOST)
    else:
        infinity_obj = infinity.connect(LOCAL_INFINITY_PATH)

    table = infinity_obj.get_database("default_db").get_table(table_name)
    query_builder = InfinityThriftQueryBuilder(table)
    query_builder.output(["_row_id"])
    query_builder.match_dense('col1', queries[0], 'float', 'l2', 100, {'ef': str(ef)})
    res, _ = query_builder.to_result()

    dur_sum = 0
    for i in range(rounds):

        query_results = [[] for _ in range(len(queries))]

        dur = 0.0
        for idx, query_vec in enumerate(queries):

            start = time.time()

            query_builder = InfinityThriftQueryBuilder(table)
            query_builder.output(["_row_id"])
            query_builder.match_dense('col1', query_vec, 'float', 'l2', 100, {'index_name': 'hnsw_index', 'ef': str(ef)})
            res, _ = query_builder.to_result()
            end = time.time()

            diff = end - start
            dur += diff

            res_list = res["ROW_ID"]
            # print(len(res_list))

            for j in range(len(res_list)):
                query_results[idx].append(res_list[j])

        ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(ground_truth_path)

        recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                               ground_truth_sets_100, query_results)
        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur:.2f} s")
        results.append(f"Query Count: {len(queries)}")
        results.append(f"QPS: {(len(queries) / dur):.2f}")
        results.append(f"Recall@1: {recall_1}")
        results.append(f"Recall@10: {recall_10}")
        results.append(f"Recall@100: {recall_100}")

        dur_sum += dur
    
    dur_sum /= rounds
    results.append(f"Avg total dur: {dur_sum:.2f} s")
    results.append(f"Avg QPS: {(len(queries) / dur_sum):.2f}")

    infinity_obj.disconnect()

    for result in results:
        print(result)


def benchmark(threads, rounds, data_set, ef: int, remote: bool, path):
    if not os.path.exists(path):
        print(f"Path: {path} doesn't exist")
        raise Exception(f"Path: {path} doesn't exist")
    if data_set == "sift_1m":
        query_path = path + "/sift_query.fvecs"
        ground_truth_path = path + "/sift_groundtruth.ivecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            print(f"Rounds: {rounds}")
            process_pool(threads, rounds, query_path, ef, remote, "sift_benchmark")

        else:
            print(f"Single-thread")
            print(f"Rounds: {rounds}")
            one_thread(rounds, query_path, ground_truth_path, ef, remote, "sift_benchmark")
    elif data_set == "gist_1m":
        query_path = path + "/gist_query.fvecs"
        ground_truth_path = path + "/gist_groundtruth.ivecs"
        if threads > 1:
            print(f"Multi-threads: {threads}")
            print(f"Rounds: {rounds}")
            process_pool(threads, rounds, query_path, ef, "gist_benchmark")

        else:
            print(f"Single-thread")
            print(f"Rounds: {rounds}")
            one_thread(rounds, query_path, ground_truth_path, ef, remote, "gist_benchmark")

def str2bool(value):
    if isinstance(value, bool):
        return value
    if value.lower() in ("yes", "true", "t", "y", "1"):
        return True
    elif value.lower() in ("no", "false", "f", "n", "0"):
        return False
    else:
        raise argparse.ArgumentTypeError("Boolean value expected")

if __name__ == '__main__':
    current_path = os.getcwd()

    parser = argparse.ArgumentParser(description="Benchmark Infinity")

    parser.add_argument(
        "-t",
        "--threads",
        type=int,
        default=1,
        dest="threads",
    )
    parser.add_argument(
        "-r",
        "--rounds",
        type=int,
        default=5,
        dest="rounds",
    )
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default='sift_1m',  # gist_1m
        dest="data_set",
    )
    parser.add_argument(
        "--ef",
        type=int,
        default=200,
        dest="ef"
    )
    parser.add_argument(
        "-R",
        "--remote",
        type=str2bool,
        default=True,
        dest="remote"
    )

    data_dir = current_path + "/test/data/benchmark/" + parser.parse_args().data_set
    print(f"Data Dir: {data_dir}")

    args = parser.parse_args()

    benchmark(args.threads, args.rounds, args.data_set, args.ef, args.remote, path=data_dir)
