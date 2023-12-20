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

import pandas as pd
from numpy import dtype
from sqlglot import condition

import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.table import traverse_conditions


class TestCase:

    def test_version(self):
        print(infinity.__version__)

    def test_connection(self):
        """
        target: test connect and disconnect server ok
        method: connect server
        expect: connect and disconnect successfully
        """
        ports = [9090, 9080, 9070]
        for port in ports:
            infinity_obj = infinity.connect(REMOTE_HOST)
            assert infinity_obj
            assert infinity_obj.disconnect()

    def test_create_db_with_invalid_name(self):
        """
        target: test db name limitation
        method: create db with empty name
        expect: create db fail with error message
        """
        infinity_obj = infinity.connect(REMOTE_HOST)
        assert infinity_obj

        res = infinity_obj.create_database("")
        assert not res.success
        assert res.error_msg

        assert infinity_obj.disconnect()

    def test_infinity_thrift(self):
        """
        target: test basic operation
        method:
        1. connect
        2. create db
        3. list db
        4. drop db
        5. get db
        6. create table
        7. drop table
        8. create index
        9. drop index
        10. insert and search
        11. import and search
        12.
        expect: all operations successfully
        """
        ports = [9080]
        for port in ports:
            infinity_obj = infinity.connect(REMOTE_HOST)
            assert infinity_obj

            # infinity
            db_obj = infinity_obj.create_database("my_db")
            assert db_obj is not None

            res = infinity_obj.list_databases()
            assert res.success

            for db in res.db_names:
                assert db in ['my_db', 'default']

            res = infinity_obj.drop_database("my_db")
            assert res.success

            db_obj = infinity_obj.get_database("default")
            db_obj.drop_table("my_table1", if_exists=True)
            table_obj = db_obj.create_table(
                "my_table1", {"c1": "int, primary key"}, None)
            assert table_obj is not None

            res = db_obj.list_tables()
            assert res.success

            res = db_obj.drop_table("my_table1")
            assert res.success

            # index
            db_obj.drop_table("my_table2", if_exists=True)
            table_obj = db_obj.create_table(
                "my_table2", {"c1": "vector,1024,float"}, None)
            assert table_obj is not None

            table_obj = db_obj.get_table("my_table2")
            assert table_obj

            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c1",
                                                          index.IndexType.IVFFlat,
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], None)
            assert res.success

            res = table_obj.drop_index("my_index")
            assert res.success

            res = db_obj.drop_table("my_table2")
            assert res.success

            # insert
            db_obj.drop_table("my_table3", if_exists=True)
            table_obj = db_obj.create_table(
                "my_table3", {"c1": "int, primary key", "c2": "float"}, None)
            assert table_obj is not None

            table_obj = db_obj.get_table("my_table3")
            assert table_obj

            res = table_obj.insert(
                [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
            assert res.success
            # search
            res = table_obj.output(["c1 + 0.1"]).to_df()
            pd.testing.assert_frame_equal(res,
                                          pd.DataFrame({'(c1 + 0.100000)': (1.1, 2.1)}).astype(
                                              {'(c1 + 0.100000)': dtype('float64')}))

            res = table_obj.output(
                ["*"]).filter("c1 > 1").to_df()
            pd.testing.assert_frame_equal(res,
                                          pd.DataFrame({'c1': (2,), 'c2': (2.2,)}).astype(
                                              {'c1': dtype('int32'), 'c2': dtype('float32')}))

            res = db_obj.drop_table("my_table3")
            assert res.success

            # import
            db_obj.drop_table("my_table4", if_exists=True)
            table_obj = db_obj.create_table(
                "my_table4", {"c1": "int", "c2": "vector,3,int"}, None)
            assert table_obj is not None
            table_obj = db_obj.get_table("my_table4")
            assert table_obj

            parent_dir = os.path.dirname(os.path.dirname(os.getcwd()))
            test_csv_dir = parent_dir + "/test/data/csv/embedding_int_dim3.csv"
            assert os.path.exists(test_csv_dir)

            res = table_obj.import_data(test_csv_dir, None)
            assert res.success

            # search
            res = table_obj.output(
                ["c1"]).filter("c1 > 1").to_df()
            print(res)
            res = db_obj.drop_table("my_table4")
            assert res.success

            # disconnect
            res = infinity_obj.disconnect()
            assert res.success

    def test_traverse_conditions(self):
        res = traverse_conditions(condition("c1 > 1 and c2 < 2 or c3 = 3.3"))
        print(res)
        res = traverse_conditions(condition("c1 = 1"))
        print(res)
        res = traverse_conditions(condition("-8 < c1 and c1 <= -7"))
        print(res)
        res = traverse_conditions(
            condition("(-7 < c1 or 9 <= c1) and (c1 = 3)"))
        print(res)
