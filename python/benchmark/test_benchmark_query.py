# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import multiprocessing
import os
import struct
import time
from concurrent.futures import ThreadPoolExecutor

import polars as pl

from benchmark.test_benchmark import trace_unhandled_exceptions
from benchmark.test_benchmark_import import find_path
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable


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


def calculate_recall(ground_truth_sets, result_sets):
    recall = 0.0
    for i in range(len(ground_truth_sets)):
        if ground_truth_sets[i] in result_sets:
            recall += 1
    return recall / len(result_sets) * len(ground_truth_sets)


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


def test_read_all():
    sift_query_path = os.getcwd() + "/sift_1m/sift/l2_groundtruth.ivecs"
    vectors = fvecs_read_all(sift_query_path)
    print(len(vectors))
    print(pl.DataFrame(vectors))


def test_read():
    sift_query_path = os.getcwd() + "/sift_1m/sift/l2_groundtruth.ivecs"
    for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
        print(pl.DataFrame([query_vec]))
        if idx == 10:
            assert idx == 10
            break


def test_read_groundtruth():
    read_groundtruth_path = os.getcwd() + "/sift_1m/sift/l2_groundtruth.ivecs"
    ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(read_groundtruth_path)
    print(len(ground_truth_sets_1))
    print(len(ground_truth_sets_10))
    print(len(ground_truth_sets_100))
    print(len(ground_truth_sets_1[0]))
    print(len(ground_truth_sets_10[0]))
    print(len(ground_truth_sets_100[0]))


@trace_unhandled_exceptions
def work(query_vec, topk, metric_type, column_name, data_type):
    conn = ThriftInfinityClient(REMOTE_HOST)
    table = RemoteTable(conn, "default", "sift_benchmark")
    query_builder = InfinityThriftQueryBuilder(table)
    query_builder.output(["_row_id"])
    query_builder.knn(column_name, query_vec, data_type, metric_type, topk)
    query_builder.to_result()


class TestQueryBenchmark:

    def test_process_pool(self):
        round = 1
        total_times = 10000
        client_num = 25
        sift_query_path = find_path() + "/sift_query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            raise Exception(f"File: {sift_query_path} doesn't exist")

        start = time.time()

        p = multiprocessing.Pool(client_num)

        for i in range(round):
            for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
                p.apply_async(work, args=(query_vec, 100, "l2", "col1", "float"))
                if idx == total_times:
                    assert idx == total_times
                    break

        p.close()
        p.join()

        end = time.time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        print(f"Total Times: {total_times * round}")
        print(f"Total Dur: {dur}")
        qps = (total_times * round) / dur
        print(f"QPS: {qps}")

    def test_thread_pool(self):
        total_times = 10000
        sift_query_path = find_path() + "/sift_1m/sift_query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        start = time.time()

        with ThreadPoolExecutor(max_workers=16) as executor:
            for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
                executor.submit(work, query_vec, 100, "l2", "col1", "float")
                if idx == total_times:
                    assert idx == total_times
                    break

        end = time.time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        print(f"Total Times: {total_times}")
        print(f"Total Dur: {dur}")
        qps = total_times / dur
        print(f"QPS: {qps}")

    def test_query(self):
        thread_num = 1
        total_times = 10000

        print(">>> Query Benchmark Start <<<")
        print(f"Thread Num: {thread_num}, Times: {total_times}")

        sift_query_path = find_path() + "/sift_query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

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

        read_groundtruth_path = os.getcwd() + "/sift_1m/sift_groundtruth.ivecs"
        ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100 = read_groundtruth(read_groundtruth_path)

        recall_1, recall_10, recall_100 = calculate_recall_all(ground_truth_sets_1, ground_truth_sets_10,
                                                               ground_truth_sets_100, query_results)
        print("recall_1: ", recall_1)
        print("recall_10: ", recall_10)
        print("recall_100: ", recall_100)

        print(">>> Query Benchmark End <<<")
        qps = total_times / dur
        print(f"Total Times: {total_times}")
        print(f"Total Dur: {dur}")
        print(f"QPS: {qps}")
