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
import time

import numpy as np
import pandas as pd
import pytest
from numpy import dtype
from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException
from utils import trace_expected_exceptions
from test_sdkbase import TestSdk


class TestDelete(TestSdk):

    def test_version(self):
        print(infinity.__version__)

    def test_delete(self):
        self._test_delete(common_values.TEST_REMOTE_HOST)
        # self._test_delete(common_values.TEST_LOCAL_PATH)
    def _test_delete(self, uri):
        """
        target: test table delete apis
        method:
        1. create tables
            - 'table_3'
                - c1 int primary key
                - c2 int
                - c3 int
        2. insert
            - (1, 10, 100)
            - (2, 20, 200)
            - (3, 30, 300)
            - (4, 40, 400)
        3. delete
            - delete from table_3 where c1 = 1
            - after:
                - (2, 20, 200)
                - (3, 30, 300)
                - (4, 40, 400)
            - delete from table_3
            - after: empty
        4. drop tables
            - 'table_3'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(uri)
        db_obj = infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table(table_name="test_delete", conflict_type=ConflictType.Ignore)
        res = db_obj.create_table(
            "test_delete",
            {"c1": {"type": "int", "constraints": ["primary key", "not null"]},
             "c2": {"type": "int"}, "c3": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_delete")

        res = table_obj.insert(
            [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
             {"c1": 4, "c2": 40, "c3": 400}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.delete("c1 = 1")
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (2, 3, 4), 'c2': (20, 30, 40), 'c3': (200, 300, 400)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32'), 'c3': dtype('int32')}))

        res = table_obj.delete()
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame({'c1': (), 'c2': (), 'c3': ()})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32'), 'c3': dtype('int32')}))

        res = db_obj.drop_table("test_delete")
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    # delete empty table
    def test_delete_empty_table(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db

        tb = db_obj.drop_table("test_delete_empty_table", ConflictType.Ignore)
        assert tb

        tb = db_obj.create_table("test_delete_empty_table", {"c1": {"type": "int"}}, ConflictType.Error)
        assert tb

        table_obj = db_obj.get_table("test_delete_empty_table")

        try:
            res = table_obj.delete("c1 = 1")
        except Exception as e:
            print(e)

        db_obj.drop_table("test_delete_empty_table", ConflictType.Error)

    # delete non-existent table
    def test_delete_non_existent_table(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db

        table_obj = db_obj.drop_table("test_delete_non_existent_table", ConflictType.Ignore)
        assert table_obj

        with pytest.raises(InfinityException) as e:
            db_obj.get_table("test_delete_non_existent_table").delete()

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    # delete table, all rows are met the condition
    @trace_expected_exceptions
    @pytest.mark.parametrize('column_types', common_values.types_array)
    @pytest.mark.parametrize('column_types_example', common_values.types_example_array)
    def test_delete_table_all_row_met_the_condition(self, get_infinity_db, column_types, column_types_example):
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_table_all_row_met_the_condition", ConflictType.Ignore)
        db_obj.create_table("test_delete_table_all_row_met_the_condition", {"c1": {"type": column_types}},
                            ConflictType.Error)
        table_obj = db_obj.get_table("test_delete_table_all_row_met_the_condition")
        try:
            table_obj.insert([{"c1": column_types_example}])
            print("insert c1 = " + str(column_types_example))
        except Exception as e:
            print(e)
        try:
            table_obj.delete("c1 = " + str(column_types_example))
            print("delete c1 = " + str(column_types_example))
        except Exception as e:
            print(e)

        db_obj.drop_table("test_delete_table_all_row_met_the_condition", ConflictType.Error)

    # delete table, no row is met the condition
    def test_delete_table_no_rows_met_condition(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_delete_table_no_rows_met_condition" + str(i))

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_delete_table_no_rows_met_condition" + str(i),
                                     {"c1": {"type": common_values.types_array[i]}}, ConflictType.Error)
            assert tb

            table_obj = db_obj.get_table("test_delete_table_no_rows_met_condition" + str(i))
            try:
                table_obj.insert([{"c1": common_values.types_example_array[i]}])
                print("insert c1 = " + str(common_values.types_example_array[i]))
            except Exception as e:
                print(e)
            try:
                table_obj.delete("c1 = 0")
                print("delete c1 = 0")
            except Exception as e:
                print(e)

            res = table_obj.output(["*"]).to_df()
            print("{}：{}".format(common_values.types_array[i], res))
            assert tb

        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_delete_table_no_rows_met_condition" + str(i), ConflictType.Error)

    # delete table with only one block
    def test_delete_table_with_one_block(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_table_with_one_block", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_table_with_one_block", {"c1": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_table_with_one_block", ConflictType.Error)

    # delete table with multiple blocks, but only one segment
    def test_delete_table_with_one_segment(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_table_with_one_segment", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_table_with_one_segment", {"c1": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        for i in range(1024):
            table_obj.delete("c1 = " + str(i))
        delete_res = table_obj.output(["*"]).to_df()
        db_obj.drop_table("test_delete_table_with_one_segment", ConflictType.Error)

        print(delete_res)

    # select before delete, select after delete and check the change.
    def test_select_before_after_delete(self, get_infinity_db):
        db_obj = get_infinity_db
        db_obj.drop_table("test_select_before_after_delete", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_select_before_after_delete", {"c1": {"type": "int"}}, ConflictType.Error)

        # insert
        for i in range(10):
            values = [{"c1": i} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_select_before_after_delete", ConflictType.Error)

    # delete just inserted data and select to check
    def test_delete_insert_data(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_insert_data", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_insert_data", {"c1": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1} for _ in range(10)]
        table_obj.insert(values)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_insert_data", ConflictType.Error)

    # delete inserted long before and select to check
    @pytest.mark.slow
    def test_delete_inserted_long_before_data(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_inserted_long_before_data", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_inserted_long_before_data", {"c1": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(1024):
            values = [{"c1": i} for _ in range(5)]
            table_obj.insert(values)

        time.sleep(10)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_inserted_long_before_data", ConflictType.Error)

    # delete dropped table
    def test_delete_dropped_table(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db

        with pytest.raises(InfinityException) as e:
            db_obj.drop_table("test_delete_dropped_table")
            table_obj = db_obj.get_table("test_delete_dropped_table")
            table_obj.delete("c1 = 0")
            assert table_obj
            db_obj.drop_table("test_delete_dropped_table")

        assert e.type == infinity.common.InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST


    # various expression will be given in where clause, and check result correctness
    @trace_expected_exceptions
    @pytest.mark.parametrize('column_types', ["int", "int8", "int16", "int32", "int64", "integer",
                                              "float", "float32", "double", "float64",
                                              "varchar",
                                              "bool",
                                              "vector, 3, float"])
    @pytest.mark.parametrize('column_types_example', [1, 127, 32767, 2147483647, pow(2, 63) - 1, 10,
                                                      float(1.1), np.float32(1 / 3), np.double(1 / 3),
                                                      np.float64(1 / 3),
                                                      "^789$ test insert varchar",
                                                      True,
                                                      np.array([1.1, 2.2, 3.3]), [1, 2, 3]])
    def test_various_expression_in_where_clause(self, get_infinity_db, column_types, column_types_example):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_various_expression_in_where_clause", ConflictType.Ignore)
        db_obj.create_table("test_various_expression_in_where_clause", {"c1": {"type": column_types}},
                            ConflictType.Error)

        table_obj = db_obj.get_table("test_various_expression_in_where_clause")

        values = [{"c1": column_types_example} for _ in range(5)]
        try:
            table_obj.insert(values)

            insert_res = table_obj.output(["*"]).to_df()
            print(insert_res)

            table_obj.delete("c1 = " + str(column_types_example))
            delete_res = table_obj.output(["*"]).to_df()
            print(delete_res)
        except Exception as e:
            print(e)

        res = db_obj.drop_table("test_various_expression_in_where_clause", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_delete_one_block_without_expression(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_one_block_without_expression", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_one_block_without_expression", {"c1": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete()
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        res = db_obj.drop_table("test_delete_one_block_without_expression", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_delete_one_segment_without_expression(self, get_infinity_db):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_delete_one_segment_without_expression", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_one_segment_without_expression", {"c1": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(1024):
            values = [{"c1": i} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete()
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_one_segment_without_expression", ConflictType.Error)

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_filter_with_valid_expression(self, get_infinity_db, filter_list):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_filter_expression", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_filter_expression", {"c1": {"type": "int"}, "c2": {"type": "float"}},
                                        ConflictType.Error)

        # insert
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete(filter_list)
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_filter_expression", ConflictType.Error)

    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_filter_with_invalid_expression(self, get_infinity_db, filter_list):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_filter_expression", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_filter_expression", {"c1": {"type": "int"}, "c2": {"type": "float"}},
                                        ConflictType.Error)

        # insert
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        # TODO: Detailed error information check
        with pytest.raises(Exception):
            table_obj.delete(filter_list)
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_filter_expression", ConflictType.Error)
