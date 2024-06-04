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
import pytest
from numpy import dtype

from common import common_values
import infinity
import infinity.index as index
from infinity.errors import ErrorCode
from infinity.common import ConflictType
from utils import copy_data
from test_sdkbase import TestSdk

test_csv_file = "embedding_int_dim3.csv"

class TestCase(TestSdk):
    def test_version(self):
        print(infinity.__version__)

    def test_connection(self):

        """
        target: test connect and disconnect server ok
        method: connect server
        expect: connect and disconnect successfully
        """
        ports = [9090, 23817, 9070]
        for port in ports:
            infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
            assert infinity_obj
            assert infinity_obj.disconnect()

        local_infinity_obj = infinity.connect(common_values.TEST_LOCAL_PATH)
        assert local_infinity_obj
        assert local_infinity_obj.disconnect()

    def create_db_with_invalid_name(self, infinity_obj):
        assert infinity_obj

        db_name = ""
        with pytest.raises(Exception,
                           match=f"DB name '{db_name}' is not valid. It should start with a letter and can contain only letters, numbers and underscores"):
            db = infinity_obj.create_database("")

    def test_create_db_with_invalid_name(self):
        """
        target: test db name limitation
        method: create db with empty name
        expect: create db fail with error message
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        self.create_db_with_invalid_name(infinity_obj)
        assert infinity_obj.disconnect()

        local_infinity_obj = infinity.connect(common_values.TEST_LOCAL_PATH)
        self.create_db_with_invalid_name(local_infinity_obj)
        assert local_infinity_obj.disconnect()

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_local_basic(self, check_data):
        infinity_obj = infinity.connect(common_values.TEST_LOCAL_PATH)
        assert infinity_obj

        res = infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK
        for db_name in res.db_names:
            if db_name != "default_db":
                infinity_obj.drop_database(db_name, ConflictType.Error)

        # infinity
        db_obj = infinity_obj.create_database("my_db", ConflictType.Error)
        assert db_obj is not None

        res = infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for db in res.db_names:
            assert db in ['my_db', "default_db"]

        res = infinity_obj.drop_database("my_db", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("my_table1", ConflictType.Ignore)

        table_obj = db_obj.create_table(
            "my_table1", {"c1": {"type": "int", "constraints": ["primary key"]}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK
        print(res)

        res = db_obj.drop_table("my_table1")
        assert res.error_code == ErrorCode.OK

        # index
        db_obj.drop_table("my_table2", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table2", {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        table_obj = db_obj.get_table("my_table2")
        assert table_obj

        res = table_obj.create_index("my_index",
                                     [index.IndexInfo("c1",
                                                      index.IndexType.IVFFlat,
                                                      [index.InitParameter("centroids_count", "128"),
                                                       index.InitParameter("metric", "l2")])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = table_obj.drop_index("my_index")
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("my_table2")
        assert res.error_code == ErrorCode.OK

        # insert
        db_obj.drop_table("my_table3", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table3", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert table_obj is not None

        table_obj = db_obj.get_table("my_table3")
        assert table_obj

        res = table_obj.insert(
            [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
        assert res.error_code == ErrorCode.OK
        # search
        res = table_obj.output(["c1"]).to_result()
        print(res)
        result_data = res.result_data
        print(result_data)
        for row in result_data:
            for col in row:
                print(str(col))
                print(col.column_name, col.column_data)

        # pd.testing.assert_frame_equal(res,
        #                               pd.DataFrame({'(c1 + 0.100000)': (1.1, 2.1)}).astype(
        #                                   {'(c1 + 0.100000)': dtype('float64')}))
        #
        # res = table_obj.output(
        #     ["*"]).filter("c1 > 1").to_df()
        # pd.testing.assert_frame_equal(res,
        #                               pd.DataFrame({'c1': (2,), 'c2': (2.2,)}).astype(
        #                                   {'c1': dtype('int32'), 'c2': dtype('float32')}))
        #
        res = db_obj.drop_table("my_table3")
        assert res.error_code == ErrorCode.OK
        #
        # # import
        db_obj.drop_table("my_table4", ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "my_table4", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
        assert table_obj is not None
        table_obj = db_obj.get_table("my_table4")
        assert table_obj

        print("check_data:", check_data)
        if not check_data:
            print("test csv file: ", test_csv_file)
            copy_data(test_csv_file)

        assert os.path.exists(common_values.TEST_TMP_DIR + test_csv_file)

        res = table_obj.import_data(common_values.TEST_TMP_DIR + test_csv_file)
        assert res.error_code == ErrorCode.OK

        # search
        res = table_obj.output(
            ["c1"]).to_result()
        print(res)
        res = db_obj.drop_table("my_table4")
        assert res.error_code == ErrorCode.OK

        assert infinity_obj.disconnect()
    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_basic(self, check_data):
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
        ports = [23817]
        for port in ports:
            infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
            assert infinity_obj

            res = infinity_obj.list_databases()
            assert res.error_code == ErrorCode.OK
            for db_name in res.db_names:
                if db_name != "default_db":
                    infinity_obj.drop_database(db_name, ConflictType.Error)

            # infinity
            db_obj = infinity_obj.create_database("my_db", ConflictType.Error)
            assert db_obj is not None

            res = infinity_obj.list_databases()
            assert res.error_code == ErrorCode.OK

            for db in res.db_names:
                assert db in ['my_db', "default_db"]

            res = infinity_obj.drop_database("my_db", ConflictType.Error)
            assert res.error_code == ErrorCode.OK

            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("my_table1", ConflictType.Ignore)
            table_obj = db_obj.create_table(
                "my_table1", {"c1": {"type": "int", "constraints": ["primary key"]}}, ConflictType.Error)
            assert table_obj is not None

            res = db_obj.list_tables()
            assert res.error_code == ErrorCode.OK

            res = db_obj.drop_table("my_table1")
            assert res.error_code == ErrorCode.OK

            # index
            db_obj.drop_table("my_table2", ConflictType.Ignore)
            table_obj = db_obj.create_table(
                "my_table2", {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
            assert table_obj is not None

            table_obj = db_obj.get_table("my_table2")
            assert table_obj

            res = table_obj.create_index("my_index",
                                         [index.IndexInfo("c1",
                                                          index.IndexType.IVFFlat,
                                                          [index.InitParameter("centroids_count", "128"),
                                                           index.InitParameter("metric", "l2")])], ConflictType.Error)
            assert res.error_code == ErrorCode.OK

            res = table_obj.drop_index("my_index")
            assert res.error_code == ErrorCode.OK

            res = db_obj.drop_table("my_table2")
            assert res.error_code == ErrorCode.OK

            # insert
            db_obj.drop_table("my_table3", ConflictType.Ignore)
            table_obj = db_obj.create_table(
                "my_table3", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
            assert table_obj is not None

            table_obj = db_obj.get_table("my_table3")
            assert table_obj

            res = table_obj.insert(
                [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}])
            assert res.error_code == ErrorCode.OK
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
            assert res.error_code == ErrorCode.OK

            # import
            db_obj.drop_table("my_table4", ConflictType.Ignore)
            table_obj = db_obj.create_table(
                "my_table4", {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}}, ConflictType.Error)
            assert table_obj is not None
            table_obj = db_obj.get_table("my_table4")
            assert table_obj

            if not check_data:
                copy_data(test_csv_file)

            assert os.path.exists(common_values.TEST_TMP_DIR + test_csv_file)

            res = table_obj.import_data(common_values.TEST_TMP_DIR + test_csv_file)
            assert res.error_code == ErrorCode.OK

            # search
            res = table_obj.output(
                ["c1"]).filter("c1 > 1").to_df()
            print(res)
            res = db_obj.drop_table("my_table4")
            assert res.error_code == ErrorCode.OK

            # disconnect
            res = infinity_obj.disconnect()
            assert res.error_code == ErrorCode.OK
