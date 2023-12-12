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

import infinity
from benchmark.test_benchmark import trace_unhandled_exceptions
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


@trace_unhandled_exceptions
def work(query_vec, topk, metric_type, column_name, data_type):
    conn = ThriftInfinityClient(REMOTE_HOST)
    table = RemoteTable(conn, "default", "knn_benchmark")
    query_builder = InfinityThriftQueryBuilder(table)
    query_builder.output(["*"])
    query_builder.knn(column_name, query_vec, data_type, metric_type, topk)
    query_builder.to_df()


class TestQueryBenchmark:

    def test_process_pool(self):
        total_times = 10000
        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        start = time.time()

        p = multiprocessing.Pool(1)
        for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
            p.apply_async(work, args=(query_vec, 100, "l2", "c1", "float"))
            if idx == total_times:
                assert idx == total_times
                break
        p.close()
        p.join()

        end = time.time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        print(f"Total Times: {total_times}")
        print(f"Total Dur: {dur}")
        qps = total_times / dur
        print(f"QPS: {qps}")

    def test_thread_pool(self):
        total_times = 10000
        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        start = time.time()

        with ThreadPoolExecutor(max_workers=1) as executor:
            for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
                executor.submit(work, query_vec, 100, "l2", "c1", "float")
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
        start = time.time()

        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        conn = ThriftInfinityClient(REMOTE_HOST)
        table = RemoteTable(conn, "default", "knn_benchmark")

        for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
            query_builder = InfinityThriftQueryBuilder(table)
            query_builder.output(["*"])
            query_builder.knn('c1', query_vec, 'float', 'l2', 100)
            query_builder.to_df()
            if idx == total_times:
                assert idx == total_times
                break
        end = time.time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        qps = total_times / dur
        print(f"Total Times: {total_times}")
        print(f"Total Dur: {dur}")
        print(f"QPS: {qps}")

    def test_one_query(self):
        conn = ThriftInfinityClient(REMOTE_HOST)
        table = RemoteTable(conn, "default", "knn_benchmark_1")
        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        for query_vec in fvecs_read(sift_query_path):
            query_builder = InfinityThriftQueryBuilder(table)
            query_builder.output(["*"])
            query_builder.knn('c1', query_vec, 'float', 'l2', 100)
            print(query_vec)
            print(query_builder.to_df())
            break

    def test_query_2(self):
        thread_num = 1
        total_times = 10000

        print(">>> Query Benchmark Start <<<")
        print(f"Thread Num: {thread_num}, Times: {total_times}")
        start = time.time()

        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
            infinity_obj = infinity.connect(REMOTE_HOST)
            db_obj = infinity_obj.get_database("default")
            assert db_obj is not None
            if idx == total_times:
                assert idx == total_times
                break
        end = time.time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        qps = total_times / dur
        print(f"Total Times: {total_times}")
        print(f"Total Dur: {dur}")
        print(f"QPS: {qps}")
