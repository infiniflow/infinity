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
import signal
import time


import pandas as pd
import pytest
from numpy import dtype

from common import common_values
import infinity
import infinity.index as index
from infinity.errors import ErrorCode
from utils import start_infinity_service_in_subporcess


class TestInsert:

    def test_version(self):
        print(infinity.__version__)

    def test_insert_basic(self):
        """
        target: test table insert apis
        method:
        1. create tables
            - 'table_2'
                - c1 int primary key
                - c2 int null
        2. insert
            - insert into table_2 (c1, c2) values(1, 2)     √
            - insert into table_2 (c2, c1) values(1, 2)     √
            - insert into table_2 (c1) values(3)            √
        3. select all
            - 1, 2
            - 2, 1
            - 3, null
        4. drop tables
            - 'table_2'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table(table_name="table_2", if_exists=True)
        # infinity
        table_obj = db_obj.create_table(
            "table_2", {"c1": "int, primary key, not null", "c2": "int, not null"}, None)
        assert table_obj is not None

        res = table_obj.insert([{"c1": 0, "c2": 0}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c1": 1, "c2": 1}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert({"c2": 2, "c1": 2})
        assert res.error_code == ErrorCode.OK

        res = table_obj.insert([{"c2": 3, "c1": 3}, {"c1": 4, "c2": 4}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (0, 1, 2, 3, 4), 'c2': (0, 1, 2, 3, 4)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32')}))

        res = db_obj.drop_table("table_2")
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    def test_insert_varchar(self):
        """
        target: test insert varchar column
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_varchar", True)
        table_obj = db_obj.create_table("test_insert_varchar", {
            "c1": "varchar"}, None)
        assert table_obj

        res = table_obj.insert([{"c1": "test_insert_varchar"}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": " test insert varchar "}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": "^789$ test insert varchar"}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ("test_insert_varchar", " test insert varchar ",
                                                                "^789$ test insert varchar")}))
        db_obj.drop_table("test_insert_varchar")

    def test_insert_big_varchar(self):
        """
        target: test insert varchar with big length
        method: create table with varchar column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_varchar", True)
        table_obj = db_obj.create_table("test_insert_big_varchar", {
            "c1": "varchar"}, None)
        assert table_obj
        for i in range(100):
            res = table_obj.insert([{"c1": "test_insert_big_varchar" * 1000}])
            assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ["test_insert_big_varchar" * 1000] * 100}))

        db_obj.drop_table("test_insert_big_varchar")

        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_insert_embedding(self):
        """
        target: test insert embedding column
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_embedding", True)
        table_obj = db_obj.create_table("test_insert_embedding", {
            "c1": "vector,3,int"}, None)
        assert table_obj
        res = table_obj.insert([{"c1": [1, 2, 3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4, 5, 6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7, 8, 9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))
        res = table_obj.insert([{"c1": [1, 2, 3]}, {"c1": [4, 5, 6]}, {
            "c1": [7, 8, 9]}, {"c1": [-7, -8, -9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': ([1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9],
                                                                [1, 2, 3], [4, 5, 6], [7, 8, 9], [-7, -8, -9])}))

        db_obj.drop_table("test_insert_embedding_2", True)
        db_obj.create_table("test_insert_embedding_2", {
            "c1": "vector,3,float"}, None)
        table_obj = db_obj.get_table("test_insert_embedding_2")
        assert table_obj
        res = table_obj.insert([{"c1": [1.1, 2.2, 3.3]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4.4, 5.5, 6.6]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7.7, 8.8, 9.9]}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-7.7, -8.8, -9.9]}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': ([1.1, 2.2, 3.3], [4.4, 5.5, 6.6], [7.7, 8.8, 9.9], [-7.7, -8.8, -9.9])}))

        db_obj.drop_table("test_insert_embedding2")

    @pytest.mark.skip("Unexpected error.")
    def test_insert_big_embedding(self):
        """
        target: test insert embedding with big dimension
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_embedding", True)
        table_obj = db_obj.create_table("test_insert_big_embedding", {
            "c1": "vector,65535,int"}, None)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [4] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [7] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 65535}])
        assert res.error_code == ErrorCode.OK

    def test_insert_big_embedding_float(self):
        """
        target: test insert embedding float with big dimension
        method: create table with embedding column
        expected: ok
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_embedding_float", True)
        table_obj = db_obj.create_table("test_insert_big_embedding_float", {
            "c1": "vector,65535,float"}, None)
        assert table_obj
        res = table_obj.insert([{"c1": [1] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [1.1] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = table_obj.insert([{"c1": [-9999999.988] * 65535}])
        assert res.error_code == ErrorCode.OK
        res = db_obj.drop_table("test_insert_big_embedding_float")
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip("Unexpected error.")
    @pytest.mark.parametrize("types", ["vector,65535,int", "vector,65535,float"])
    @pytest.mark.parametrize("types_examples", [[{"c1": [1] * 65535}],
                                                [{"c1": [4] * 65535}],
                                                [{"c1": [-9999999] * 65535}],
                                                [{"c1": [1.1] * 65535}],
                                                [{"c1": [-9999999.988] * 65535}],
                                                ])
    def test_insert_big_embedding_various_type(self, types, types_examples):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_big_embedding", True)
        table_obj = db_obj.create_table("test_insert_big_embedding", {
            "c1": types}, None)
        res = table_obj.insert(types_examples)
        assert res.error_code == ErrorCode.OK

    def test_insert_exceed_block_size(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_exceed_block_size", True)
        table_obj = db_obj.create_table("test_insert_exceed_block_size", {
            "c1": "float"}, None)
        assert table_obj
        values = [{"c1": 1} for _ in range(8193)]

        with pytest.raises(Exception,
                           match=".*Insert values row count 8193 is larger than default block capacity 8192*"):
            table_obj.insert(values)

    # insert primitive data type not aligned with table definition
    @pytest.mark.parametrize("types", common_values.types_array)
    @pytest.mark.parametrize("types_example", common_values.types_example_array)
    def test_insert_data_not_aligned_with_table_definition(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_data_not_aligned_with_table_definition")
        table_obj = db_obj.create_table("test_insert_data_not_aligned_with_table_definition",
                                        {"c1": "int", "c2": types}, None)

        # insert
        values = [{"c1": 1, "c2": types_example}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert large varchar which exceeds the limit to table
    # insert embedding data which type info isn't match with table definition
    # insert data into non-existent table, dropped table
    def test_insert_data_into_non_existent_table(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_data_into_non_existent_table")

        # create and drop table
        table_obj = db_obj.create_table("test_insert_data_into_non_existent_table",
                                        {"c1": "int", "c2": "int"}, None)
        db_obj.drop_table("test_insert_data_into_non_existent_table")

        # insert
        values = [{"c1": 1, "c2": 1}]
        # check whether throw exception TABLE_NOT_EXIST
        with pytest.raises(Exception, match="ERROR:3022*"):
            table_obj.insert(values)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert empty into table
    @pytest.mark.parametrize("types", common_values.types_array)
    def test_insert_empty_into_table(self, types):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_empty_into_table")
        table_obj = db_obj.create_table("test_insert_empty_into_table",
                                        {"c1": "int", "c2": types}, None)

        # insert
        with pytest.raises(Exception, match=".*input value count mismatch*"):
            values = [{}]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert data into index created table
    def test_insert_data_into_index_created_table(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_data_into_index_created_table")
        table_obj = db_obj.create_table("test_insert_data_into_index_created_table",
                                        {"c1": "vector,1024,float"}, None)

        # create index
        table_obj.create_index("my_index_1",
                               [index.IndexInfo("c1",
                                                index.IndexType.Hnsw,
                                                [
                                                    index.InitParameter("M", "16"),
                                                    index.InitParameter("ef_construction", "50"),
                                                    index.InitParameter("ef", "50"),
                                                    index.InitParameter("metric", "l2")
                                                ])], None)

        table_obj.create_index("my_index_2",
                               [index.IndexInfo("c1",
                                                index.IndexType.IVFFlat,
                                                [index.InitParameter("centroids_count", "128"),
                                                 index.InitParameter("metric", "l2")])], None)

        # insert
        values = [{"c1": [1.1 for _ in range(1024)]}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert table with 10000 columns.
    def test_insert_table_with_10000_columns(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_table_with_10000_columns")
        table_obj = db_obj.create_table("test_insert_table_with_10000_columns", {"c1": "int", "c2": "int"}, None)

        # insert
        for i in range(100):
            values = [{"c1": i*100+j, "c2": i*100+j+1} for j in range(100)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # @pytest.mark.skip(reason="TODO")
    def test_read_after_shutdown(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        table_obj = db_obj.get_table("test_insert_table_with_10000_columns")
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert table with columns isn't matched (more and less)
    @pytest.mark.parametrize("values", [[{"c1": 1}], [{"c1": 1, "c2": 1, "c3": 1}]])
    def test_insert_with_not_matched_columns(self, values):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_with_not_matched_columns")
        table_obj = db_obj.create_table("test_insert_with_not_matched_columns",
                                        {"c1": "int", "c2": "int"}, None)

        # insert
        with pytest.raises(Exception, match=".*input value count mismatch*"):
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # insert table with column value exceeding invalid value range
    @pytest.mark.parametrize("values", [[{"c1": pow(2, 63) - 1, "c2": pow(2, 63) - 1}]])
    def test_insert_with_exceeding_invalid_value_range(self, values):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_with_exceeding_invalid_value_range")
        table_obj = db_obj.create_table("test_insert_with_exceeding_invalid_value_range",
                                        {"c1": "int", "c2": "int32"}, None)

        # insert
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_pl()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # batch insert, within limit
    @pytest.mark.parametrize("batch", [10, 1024, 2048])
    def test_batch_insert_within_limit(self, batch):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_batch_insert_within_limit")
        table_obj = db_obj.create_table("test_batch_insert_within_limit", {"c1": "int", "c2": "int"}, None)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(batch)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # batch insert, batch size limit? 8192?
    def test_batch_insert(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_batch_insert")
        table_obj = db_obj.create_table("test_batch_insert", {"c1": "int", "c2": "int"}, None)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # batch insert, with invalid data type inside.
    @pytest.mark.parametrize("batch", [10, 1024])
    @pytest.mark.parametrize("types", [(1, False), (1.1, False), ("1#$@!adf", False), ([1, 2, 3], True)])
    def test_insert_with_invalid_data_type(self, batch, types):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_with_invalid_data_type")
        table_obj = db_obj.create_table("test_insert_with_invalid_data_type",
                                        {"c1": "int", "c2": "vector,3,int"}, None)

        # insert
        for i in range(5):
            values = [{"c1": 1, "c2": types[0]} for _ in range(batch)]
            if not types[1]:
                with pytest.raises(Exception, match=".*ERROR:3032*"):
                    table_obj.insert(values)
            else:
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # batch insert, with invalid column count
    @pytest.mark.parametrize("batch", [10, 1024])
    def test_batch_insert_with_invalid_column_count(self, batch):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_with_invalid_column_count")
        table_obj = db_obj.create_table("test_insert_with_invalid_column_count", {"c1": "int"}, None)

        # insert
        with pytest.raises(Exception, match=".*input value count mismatch*"):
            for i in range(5):
                values = [{"c1": 1, "c2": 1} for _ in range(batch)]
                table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize('column_types', ["varchar"])
    @pytest.mark.parametrize('column_types_example', [[1, 2, 3], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]])
    def test_various_insert_types(self, column_types, column_types_example):
        # connect

        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_various_insert_types")
        db_obj.create_table("test_various_insert_types", {"c1": column_types}, None)

        table_obj = db_obj.get_table("test_various_insert_types")

        values = [{"c1": column_types_example} for _ in range(5)]
        table_obj.insert(values)

        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.complex
    @pytest.mark.skip(reason="TODO")
    def test_insert_and_shutdown_output(self):

        os.system("rm -fr /tmp/infinity")
        # start service
        infinity_service_1 = start_infinity_service_in_subporcess()

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_and_shutdown_output")
        table_obj = db_obj.create_table("test_insert_and_shutdown_output", {"c1": "int"}, None)

        for i in range(10):
            values = [{"c1": 1} for _ in range(100)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # shutdown service
        os.kill(infinity_service_1.pid, signal.SIGINT)

        time.sleep(1)

        # restart
        infinity_service_2 = start_infinity_service_in_subporcess()

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        table_obj = db_obj.get_table("test_insert_and_shutdown_output")
        new_insert_res = table_obj.output(["*"]).to_df()
        print(new_insert_res)

        # shutdown service
        os.kill(infinity_service_2.pid, signal.SIGINT)

    def test_insert_zero_column(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_zero_column")
        table_obj = db_obj.create_table("test_insert_zero_column", {"c1": "int"}, None)

        with pytest.raises(Exception, match="ERROR:3065*"):
            table_obj.insert([])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("column_name", [
        "c2",
        "$%#$sadf",
        # 1,
        # 2.2,
        # [1],
        # (1, "adsf"),
        # {"1": 1}
    ])
    def test_insert_no_match_column(self, column_name):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_insert_no_match_column")
        table_obj = db_obj.create_table("test_insert_no_match_column", {"c1": "int"}, None)

        with pytest.raises(Exception, match="ERROR:3024*"):
            table_obj.insert([{column_name: 1}])
            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
