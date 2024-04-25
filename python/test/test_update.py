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

import pandas as pd
import pytest
from numpy import dtype

from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType
from utils import trace_expected_exceptions
from test_sdkbase import TestSdk


class TestUpdate(TestSdk):

    def test_version(self):
        print(infinity.__version__)

    def test_update(self):
        """
        target: test table update apis
        method:
        1. create tables
            - 'table_4'
                - c1 int primary key
                - c2 int
                - c3 int
        2. insert
            - (1, 10, 100)
            - (2, 20, 200)
            - (3, 30, 300)
            - (4, 40, 400)
        3. delete
            - update table_4 set c2 = 90, c3 = 900 where c1 = 1
            - after:
                - (1, 90, 900)
                - (2, 20, 200)
                - (3, 30, 300)
                - (4, 40, 400)
            - update table_4 set c2 = 80, c3 = 800
            - after:
                - (1, 80, 800)
                - (2, 80, 800)
                - (3, 80, 800)
                - (4, 80, 800)
        4. drop tables
            - 'table_4'
        expect: all operations successfully
        """
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")

        db_obj.drop_table(table_name="test_update", conflict_type=ConflictType.Ignore)

        # infinity
        table_obj = db_obj.create_table(
            "test_update", {"c1": {"type": "int", "constraints": ["primary key", "not null"]},
                            "c2": {"type": "int"}, "c3": {"type": "int"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.insert(
            [{"c1": 1, "c2": 10, "c3": 100}, {"c1": 2, "c2": 20, "c3": 200}, {"c1": 3, "c2": 30, "c3": 300},
             {"c1": 4, "c2": 40, "c3": 400}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
        assert res.error_code == ErrorCode.OK

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (2, 3, 4, 1), 'c2': (20, 30, 40, 90), 'c3': (200, 300, 400, 900)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32'), 'c3': dtype('int32')}))

        with pytest.raises(Exception, match=r".*where_conditions_ shall not be empty*"):
            table_obj.update(None, [{"c2": 90, "c3": 900}])

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (2, 3, 4, 1), 'c2': (20, 30, 40, 90), 'c3': (200, 300, 400, 900)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32'), 'c3': dtype('int32')}))

        res = db_obj.drop_table("test_update", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update empty table
    @trace_expected_exceptions
    def test_update_empty_table(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_empty_table", ConflictType.Ignore)

        try:
            db_obj.create_table("test_update_empty_table", {}, ConflictType.Error)
        except Exception as e:
            print(e)

        tb_obj = db_obj.get_table("test_update_empty_table")

        try:
            tb_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
        except Exception as e:
            print(e)

        # res = tb_obj.output["*"].to_df()
        # print(res)

        res = db_obj.drop_table("test_update_empty_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update non-existent table
    def test_update_non_existent_table(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_non_existent_table", ConflictType.Ignore)

        db_obj.create_table("test_update_non_existent_table",
                            {"c1": {"type": "int"}, "c2": {"type": "int"}, "c3": {"type": "int"}},
                            ConflictType.Error)

        try:
            tb_obj = db_obj.get_table("test_update_empty_table")
        except Exception as e:
            print(e)

        try:
            # FIXME tb_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
            pass
        except Exception as e:
            print(e)

        res = db_obj.drop_table("test_update_non_existent_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update table, no row is met the condition
    @trace_expected_exceptions
    def test_update_no_row_is_met_the_condition(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_no_row_is_met_the_condition" + str(i), ConflictType.Ignore)

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_update_no_row_is_met_the_condition" + str(i),
                                     {"c1": {"type": common_values.types_array[i]},
                                      "c2": {"type": common_values.types_array[i]}}, ConflictType.Error)
            assert tb

            tb_obj = db_obj.get_table("test_update_no_row_is_met_the_condition" + str(i))

            try:
                tb_obj.insert([{"c1": common_values.types_example_array[i],
                                "c2": common_values.types_example_array[i]}])
                res = tb_obj.output(["*"]).to_df()
                print(res)
                print("insert c1 = " + str(common_values.types_example_array[i]) +
                      ", c2 = " + str(common_values.types_example_array[i]))
            except Exception as e:
                print(e)

            try:
                tb_obj.update("c1 = 2",
                              [{"c2": common_values.types_example_array[i]}])
                res = tb_obj.output(["*"]).to_df()
                print("update type: {} \n {}".format(common_values.types_array[i], res))

            except Exception as e:
                print(e)

        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_no_row_is_met_the_condition" + str(i), ConflictType.Error)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update table, all rows are met the condition
    @trace_expected_exceptions
    def test_update_all_row_is_met_the_condition(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_all_row_is_met_the_condition" + str(i), ConflictType.Ignore)

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_update_all_row_is_met_the_condition" + str(i),
                                     {"c1": {"type": common_values.types_array[i]},
                                      "c2": {"type": common_values.types_array[i]}}, ConflictType.Error)
            assert tb

            tb_obj = db_obj.get_table("test_update_all_row_is_met_the_condition" + str(i))

            try:
                tb_obj.insert([{"c1": common_values.types_example_array[i],
                                "c2": common_values.types_example_array[i]}])
                res = tb_obj.output(["*"]).to_df()
                print(res)
                print("insert c1 = " + str(common_values.types_example_array[i]) +
                      ", c2 = " + str(common_values.types_example_array[i]))
            except Exception as e:
                print(e)

            try:
                tb_obj.update("c1 = " + str(common_values.types_example_array[i]),
                              [{"c2": common_values.types_example_array[i]}])
                res = tb_obj.output(["*"]).to_df()
                print("update type: {} \n {}".format(common_values.types_array[i], res))

            except Exception as e:
                print(e)

        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_all_row_is_met_the_condition" + str(i), ConflictType.Error)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update table with only one block

    def test_update_table_with_one_block(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_table_with_one_block", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_table_with_one_block",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8192)]
        # values = [{"c1": 1, "c2": 2}]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # update
        table_obj.update("c1 = 1", [{"c2": 20}])
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)

        res = db_obj.drop_table("test_update_table_with_one_block", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update table with multiple blocks, but only one segment
    def test_update_table_with_one_segment(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_table_with_one_segment", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_table_with_one_segment",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        for i in range(1024):
            values = [{"c1": 1, "c2": 2} for _ in range(8)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # update
        table_obj.update("c1 = 1", [{"c2": 20}])
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)

        res = db_obj.drop_table("test_update_table_with_one_segment", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update before delete, select after delete and check the change.
    def test_update_before_delete(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_before_delete", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_before_delete", {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)

        # update
        table_obj.update("c1 = 1", [{"c2": 20}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_before_delete", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update just inserted data and select to check
    def test_update_inserted_data(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_inserted_data", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_inserted_data", {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # update
        table_obj.update("c1 = 1", [{"c2": 21}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_inserted_data", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update inserted long before and select to check
    @pytest.mark.slow
    @pytest.mark.skip(reason="Taking too much time.")
    def test_update_inserted_long_before(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_inserted_long_before", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_inserted_long_before",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        time.sleep(3600)

        # update
        table_obj.update("c1 = 1", [{"c2": 21}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_inserted_long_before", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update dropped table
    def test_update_dropped_table(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_dropped_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_dropped_table", {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)
        res = db_obj.drop_table("test_update_dropped_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # update
        with pytest.raises(Exception, match="ERROR:3022*"):
            table_obj.update("c1 = 1", [{"c2": 21}])
            update_res = table_obj.output(["*"]).to_df()
            print(update_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update new value is invalid
    @pytest.mark.parametrize("types", ["varchar"])
    @pytest.mark.parametrize("types_example", [[1, 2, 3]])
    def test_update_invalid_value(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_invalid_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)
        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_invalid_value", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update new value type is not match with table
    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        1,
        1.333,
        "1",
    ])
    def test_update_new_value(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_new_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_new_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)

        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_new_value", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        pytest.param([1, 2, 3])
    ])
    def test_update_invalid_value(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_invalid_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)

        # update
        with pytest.raises(Exception, match="ERROR:3049, Not support to convert Embedding to*"):
            table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_invalid_value", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    @pytest.mark.parametrize("types_example", [1, 1.333])
    def test_valid_filter_expression(self, get_infinity_db, filter_list, types_example):
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
        table_obj.update(filter_list, [{"c2": types_example}])
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)

        res = db_obj.drop_table("test_filter_expression", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    @pytest.mark.parametrize("types_example", [1, 1.333])
    def test_invalid_filter_expression(self, get_infinity_db, filter_list, types_example):
        # connect
        db_obj = get_infinity_db
        db_obj.drop_table("test_invalid_filter_expression", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_invalid_filter_expression",
                                        {"c1": {"type": "int"}, "c2": {"type": "float"}},
                                        ConflictType.Error)

        # insert
        for i in range(10):
            values = [{"c1": i, "c2": 3.0} for _ in range(10)]
            table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        with pytest.raises(Exception):
            table_obj.update(filter_list, [{"c2": types_example}])

        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)

        res = db_obj.drop_table("test_invalid_filter_expression", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
