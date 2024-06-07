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

from common import common_values
import infinity
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.db import RemoteDatabase
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable
from infinity.common import ConflictType
from src.test_sdkbase import TestSdk


class TestQuery(TestSdk):
    def test_query(self):
        conn = ThriftInfinityClient(common_values.TEST_REMOTE_HOST)
        db = RemoteDatabase(conn, "default_db")
        db.drop_table("my_table", conflict_type=ConflictType.Ignore)
        db.create_table(
            "my_table", {
                "num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)

        table = RemoteTable(conn, "default_db", "my_table")
        res = table.insert(
            [{"num": 1, "body": "undesirable, unnecessary, and harmful", "vec": [1.0] * 5}])
        assert res.error_code == ErrorCode.OK
        res = table.insert(
            [{"num": 2, "body": "publisher=US National Office for Harmful Algal Blooms", "vec": [4.0] * 5}])
        assert res.error_code == ErrorCode.OK
        res = table.insert(
            [{"num": 3, "body": "in the case of plants, growth and chemical", "vec": [7.0] * 5}])
        assert res.error_code == ErrorCode.OK

        res = table.create_index("my_index",
                                 [index.IndexInfo("body",
                                                  index.IndexType.FullText,
                                                  []),
                                  ], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # select_res = table.query_builder().output(["*"]).to_df()
        # print(select_res)
        # Create a query builder
        query_builder = InfinityThriftQueryBuilder(table)
        query_builder.output(["num", "body"])
        query_builder.knn('vec', [3.0] * 5, 'float', 'ip', 2)
        query_builder.match('body', 'harmful', 'topn=2')
        query_builder.fusion('rrf')
        res = query_builder.to_df()
        print(res)
        res = table.drop_index("my_index", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db.drop_table("my_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = conn.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_query_builder(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_query_builder",
                          conflict_type=ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_query_builder", {"c1": {"type": "int"}}, ConflictType.Error)
        query_builder = table_obj.query_builder
        query_builder.output(["*"]).to_df()

        res = db_obj.drop_table("test_query_builder", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
