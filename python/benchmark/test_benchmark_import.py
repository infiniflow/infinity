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

import os
import time

import infinity
from infinity import index
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.table import RemoteTable

def find_path():
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)

    print(f"Current Path: {current_path}")
    print(f"Parent Path: {parent_path}")

    data_dir = parent_path + "/test/data/benchmark/sift_1m"
    print(f"Data Dir: {data_dir}")
    return data_dir
class TestImportBenchmark:

    def test_import(self):
        """
        target: test import data to remote server
        method: connect server, create table, import data, search, drop table, disconnect
        expect: all operations successfully
        """

        infinity_obj = infinity.connect(REMOTE_HOST)
        assert infinity_obj

        st = time.process_time()

        db_obj = infinity_obj.get_database("default")
        assert db_obj
        db_obj.drop_table("sift_benchmark", True)
        db_obj.create_table("sift_benchmark", {"col1": "vector,128,float"}, None)
        table_obj = db_obj.get_table("sift_benchmark")
        assert table_obj

        test_fvecs_dir = find_path() + "/sift_base.fvecs"
        assert os.path.exists(test_fvecs_dir)
        #
        res = table_obj.import_data(test_fvecs_dir, None)
        assert res.success

        end = time.process_time()
        dur = end - st
        print(dur)

    def test_create_index(self):
        st = time.process_time()
        conn = ThriftInfinityClient(REMOTE_HOST)
        table = RemoteTable(conn, "default", "sift_benchmark")
        res = table.create_index("hnsw_index",
                                 [index.IndexInfo("col1",
                                                  index.IndexType.Hnsw,
                                                  [
                                                      index.InitParameter("M", "16"),
                                                      index.InitParameter("ef_construction", "200"),
                                                      index.InitParameter("ef", "200"),
                                                      index.InitParameter("metric", "l2"),
                                                      index.InitParameter("encode", "lvq")
                                                  ])], None)

        assert res.success

        end = time.process_time()
        dur = end - st
        print(dur)
