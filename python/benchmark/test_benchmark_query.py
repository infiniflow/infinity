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
import os
import struct
import time

import numpy as np

from infinity.common import REMOTE_HOST
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
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


class TestQueryBenchmark:

    def test_query(self):
        thread_num = 1
        total_times = 1

        path = "/tmp/infinity"

        print(">>> Query Benchmark Start <<<")
        print(f"Thread Num: {thread_num}, Times: {total_times}")
        start = time.process_time()

        sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
        if not os.path.exists(sift_query_path):
            print(f"File: {sift_query_path} doesn't exist")
            return

        conn = ThriftInfinityClient(REMOTE_HOST)
        table = RemoteTable(conn, "default", "knn_benchmark")

        for query_vec in fvecs_read(sift_query_path):
            query_builder = InfinityThriftQueryBuilder(table)
            query_builder.output(["*"])
            query_builder.knn('c1', query_vec, 'float', 'l2', 100)
            print(query_vec)
            # print(query_builder.to_df())

        end = time.process_time()
        dur = end - start
        print(">>> Query Benchmark End <<<")
        print(dur)

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
