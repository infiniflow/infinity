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

from infinity.common import REMOTE_HOST
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable


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

    print("len(vectors): ", len(vectors))

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
def work(query_vec, topk, metric_type, column_name, data_type):
    conn = ThriftInfinityClient(REMOTE_HOST)
    table = RemoteTable(conn, "default", "sift_benchmark")
    query_builder = InfinityThriftQueryBuilder(table)
    query_builder.output(["_row_id"])
    query_builder.knn(column_name, query_vec, data_type, metric_type, topk)
    query_builder.to_result()


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


def process_pool(threads, rounds, path):
    sift_query_path = path + "/sift_query.fvecs"
    if not os.path.exists(sift_query_path):
        print(f"File: {sift_query_path} doesn't exist")
        raise Exception(f"File: {sift_query_path} doesn't exist")

    results = []

    for i in range(rounds):
        start = time.time()
        p = multiprocessing.Pool(threads)
        for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
            p.apply_async(work, args=(query_vec, 100, "l2", "col1", "float"))
        p.close()
        p.join()

        end = time.time()
        dur = end - start
        results.append(f"Round {i + 1}:")
        results.append(f"Total Dur: {dur}")
        qps = 10000 / dur
        results.append(f"QPS: {qps}")

    for result in results:
        print(result)


def one_thread(rounds, path):
    sift_query_path = path + "/sift_query.fvecs"
    if not os.path.exists(sift_query_path):
        print(f"File: {sift_query_path} doesn't exist")
        raise Exception(f"File: {sift_query_path} doesn't exist")

    results = []

    for i in range(rounds):
        start = time.time()

        conn = ThriftInfinityClient(REMOTE_HOST)
        table = RemoteTable(conn, "default", "sift_benchmark")
        queries = fvecs_read_all(sift_query_path)
        query_results = [[] for _ in range(len(queries))]

        dur = 0.0
        for idx, query_vec in enumerate(queries):

            start = time.time()

            query_builder = InfinityThriftQueryBuilder(table)
            query_builder.output(["_row_id"])
            query_builder.knn('col1', query_vec, 'float', 'l2', 100)
            res, _ = query_builder.to_result()
            end = time.time()

            diff = end - start
            dur += diff

            res_list = res["ROW_ID"]
            # print(len(res_list))

            for i in range(len(res_list)):
                query_results[idx].append(res_list[i][1])


        ground_truth_path = path + "/sift_groundtruth.ivecs"
        ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(ground_truth_path)

        recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                               ground_truth_sets_100, query_results)
        results.append(f"Round {i + 1}:")
        results.append(f"recall_1: {recall_1}")
        results.append(f"recall_10: {recall_10}")
        results.append(f"recall_100: {recall_100}")
        results.append(f"Total Dur: {dur}")
        qps = 10000 / dur
        results.append(f"QPS: {qps}")

    for result in results:
        print(result)


def benchmark(threads, rounds, path):
    if threads > 1:
        print(f"Multi-threads: {threads}")
        print(f"Rounds: {rounds}")
        process_pool(threads, rounds, path)

    else:
        print(f"Single-thread")
        print(f"Rounds: {rounds}")
        one_thread(rounds, path)


if __name__ == '__main__':
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)

    print(f"Current Path: {current_path}")
    print(f"Parent Path: {parent_path}")

    data_dir = parent_path + "/test/data/benchmark/sift_1m"
    print(f"Data Dir: {data_dir}")

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
        default=1,
        dest="rounds",
    )

    args = parser.parse_args()

    benchmark(args.threads, args.rounds, path=data_dir)
