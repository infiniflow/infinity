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

from python.test.common import common_values
import infinity
from infinity.errors import ErrorCode
from utils import trace_expected_exceptions


class TestUpdate:

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
        db_obj = infinity_obj.get_database("default")

        db_obj.drop_table(table_name="table_4", if_exists=True)

        # infinity
        table_obj = db_obj.create_table(
            "table_4", {"c1": "int, primary key, not null", "c2": "int", "c3": "int"}, None)
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

        res = db_obj.drop_table("table_4")
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    # update empty table
    @trace_expected_exceptions
    def test_update_empty_table(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_empty_table")

        try:
            db_obj.create_table("test_update_empty_table", {}, None)
        except Exception as e:
            print(e)

        tb_obj = db_obj.get_table("test_update_empty_table")

        try:
            tb_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
        except Exception as e:
            print(e)

        # res = tb_obj.output["*"].to_df()
        # print(res)
        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    # update non-existent table
    def test_update_non_existent_table(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_non_existent_table")

        db_obj.create_table("test_update_non_existent_table", {"c1": "int", "c2": "int", "c3": "int"}, None)

        try:
            tb_obj = db_obj.get_table("test_update_empty_table")
        except Exception as e:
            print(e)

        try:
            # FIXME tb_obj.update("c1 = 1", [{"c2": 90, "c3": 900}])
            pass
        except Exception as e:
            print(e)

        # disconnect
        res = infinity_obj.disconnect()

        assert res.error_code == ErrorCode.OK

    # update table, no row is met the condition
    @trace_expected_exceptions
    def test_update_no_row_is_met_the_condition(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_no_row_is_met_the_condition" + str(i))

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_update_no_row_is_met_the_condition" + str(i),
                                     {"c1": common_values.types_array[i],
                                      "c2": common_values.types_array[i]}, None)
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

    # update table, all rows are met the condition
    @trace_expected_exceptions
    def test_update_all_row_is_met_the_condition(self):

        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_update_all_row_is_met_the_condition" + str(i))

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_update_all_row_is_met_the_condition" + str(i),
                                     {"c1": common_values.types_array[i],
                                      "c2": common_values.types_array[i]}, None)
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

    # update table with only one block

    def test_update_table_with_one_block(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_table_with_one_block")
        table_obj = db_obj.create_table("test_update_table_with_one_block", {"c1": "int", "c2": "int"}, None)

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

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update table with multiple blocks, but only one segment
    def test_update_table_with_one_segment(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_table_with_one_segment")
        table_obj = db_obj.create_table("test_update_table_with_one_segment", {"c1": "int", "c2": "int"}, None)

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

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update before delete, select after delete and check the change.
    def test_update_before_delete(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_before_delete")
        table_obj = db_obj.create_table("test_update_before_delete", {"c1": "int", "c2": "int"}, None)

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

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update just inserted data and select to check
    def test_update_inserted_data(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_before_delete")
        table_obj = db_obj.create_table("test_update_before_delete", {"c1": "int", "c2": "int"}, None)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # update
        table_obj.update("c1 = 1", [{"c2": 21}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update inserted long before and select to check
    @pytest.mark.skip(reason="Cost too much time.")
    def test_update_inserted_long_before(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_inserted_long_before")
        table_obj = db_obj.create_table("test_update_inserted_long_before", {"c1": "int", "c2": "int"}, None)

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

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update dropped table
    def test_update_dropped_table(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_inserted_long_before")
        table_obj = db_obj.create_table("test_update_inserted_long_before", {"c1": "int", "c2": "int"}, None)
        db_obj.drop_table("test_update_inserted_long_before")

        # update
        with pytest.raises(Exception, match="ERROR:3022*"):
            table_obj.update("c1 = 1", [{"c2": 21}])
            update_res = table_obj.output(["*"]).to_df()
            print(update_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update new value is invalid
    # @pytest.mark.skip(reason="Cast error.")
    @pytest.mark.skip(reason="When use type = varchar type-example = list, core dumped.")
    @pytest.mark.parametrize("types", ["varchar"])
    @pytest.mark.parametrize("types_example", [[1,2,3]])
    def test_update_invalid_value(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_invalid_value")
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": "int", "c2": types}, None)

        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    # update new value type is not match with table
    @pytest.mark.skip(reason="Invalid constant expression.")
    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [1, 1.333, "1", [1, 2, 3]])
    def test_update_new_value(self, types, types_example):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_update_invalid_value")
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": "int", "c2": types}, None)

        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
