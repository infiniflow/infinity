import os
import time
import pandas as pd
import pytest
from numpy import dtype
from common import common_values
import infinity
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException
from common.utils import trace_expected_exceptions
from http_adapter import http_adapter


@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.infinity_obj = infinity.connect(uri)
    if http:
        request.cls.infinity_obj = http_adapter()
    yield
    request.cls.infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    # def test_version(self):
    #     self.test_infinity_obj._test_version()
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
        db_obj = self.infinity_obj.get_database("default_db")

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

        with pytest.raises(Exception):
            table_obj.update(None, [{"c2": 90, "c3": 900}])

        res = table_obj.output(["*"]).to_df()
        pd.testing.assert_frame_equal(res, pd.DataFrame(
            {'c1': (2, 3, 4, 1), 'c2': (20, 30, 40, 90), 'c3': (200, 300, 400, 900)})
                                      .astype({'c1': dtype('int32'), 'c2': dtype('int32'), 'c3': dtype('int32')}))

        res = db_obj.drop_table("test_update", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @trace_expected_exceptions
    def test_update_empty_table(self):

        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    def test_update_non_existent_table(self):

        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    @trace_expected_exceptions
    def test_update_no_row_is_met_the_condition(self):

        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    @trace_expected_exceptions
    def test_update_all_row_is_met_the_condition(self):

        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    def test_update_table_with_one_block(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    def test_update_table_with_one_segment(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    def test_update_before_delete(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    def test_update_inserted_data(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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

    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST")!="1", reason="Taking too much time.")
    def test_update_inserted_long_before(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_inserted_long_before", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_inserted_long_before",
                                        {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)

        # insert
        values = [{"c1": 1, "c2": 2} for _ in range(8)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        time.sleep(60)

        # update
        table_obj.update("c1 = 1", [{"c2": 21}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_inserted_long_before", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_update_dropped_table(self):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_dropped_table", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_dropped_table", {"c1": {"type": "int"}, "c2": {"type": "int"}},
                                        ConflictType.Error)
        res = db_obj.drop_table("test_update_dropped_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # update
        with pytest.raises(InfinityException) as e:
            table_obj.update("c1 = 1", [{"c2": 21}])
            update_res = table_obj.output(["*"]).to_df()
            print(update_res)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    @pytest.mark.parametrize("types", ["varchar"])
    @pytest.mark.parametrize("types_example", [[1, 2, 3]])
    def test_update_invalid_value_1(self, types, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_invalid_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)
        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_invalid_value", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        1,
        1.333,
        "1",
    ])
    def test_update_new_value(self, types, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_new_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_new_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)

        # update
        table_obj.update("c1 = 1", [{"c2": types_example}])
        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_new_value", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        pytest.param([1, 2, 3])
    ])
    def test_update_invalid_value_2(self, types, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_update_invalid_value", ConflictType.Ignore)
        table_obj = db_obj.create_table("test_update_invalid_value", {"c1": {"type": "int"}, "c2": {"type": types}},
                                        ConflictType.Error)

        # update
        with pytest.raises(InfinityException) as e:
            table_obj.update("c1 = 1", [{"c2": types_example}])

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NOT_SUPPORTED_TYPE_CONVERSION

        update_res = table_obj.output(["*"]).to_df()
        print(update_res)

        res = db_obj.drop_table("test_update_invalid_value", ConflictType.Error)
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
    def test_valid_filter_expression(self, filter_list, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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
    def test_invalid_filter_expression(self, filter_list, types_example):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
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
