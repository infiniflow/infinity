import importlib
import sys
import os
import numpy as np
import time
import pandas as pd
import pytest
from numpy import dtype
from common import common_values
import infinity
import infinity_embedded
from infinity.errors import ErrorCode
from infinity.common import ConflictType, InfinityException
from common.utils import trace_expected_exceptions
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        module = importlib.import_module("infinity_embedded.common")
        func = getattr(module, 'ConflictType')
        globals()['ConflictType'] = func
        func = getattr(module, 'InfinityException')
        globals()['InfinityException'] = func
        uri = common_values.TEST_LOCAL_PATH
        request.cls.infinity_obj = infinity_embedded.connect(uri)
    elif http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    def test_delete(self, suffix):
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
        db_obj = self.infinity_obj.get_database("default_db")

        # infinity
        db_obj.drop_table(table_name="test_delete"+suffix, conflict_type=ConflictType.Ignore)
        res = db_obj.create_table(
            "test_delete"+suffix,
            {"c1": {"type": "int", "constraints": ["primary key", "not null"]},
             "c2": {"type": "int"}, "c3": {"type": "int"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_delete"+suffix)

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

        res = db_obj.drop_table("test_delete"+suffix)
        assert res.error_code == ErrorCode.OK

    def test_delete_empty_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")

        tb = db_obj.drop_table("test_delete_empty_table"+suffix, ConflictType.Ignore)
        assert tb

        tb = db_obj.create_table("test_delete_empty_table"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        assert tb

        table_obj = db_obj.get_table("test_delete_empty_table"+suffix)

        try:
            res = table_obj.delete("c1 = 1")
        except Exception as e:
            print(e)

        db_obj.drop_table("test_delete_empty_table"+suffix, ConflictType.Error)

    def test_delete_non_existent_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")

        table_obj = db_obj.drop_table("test_delete_non_existent_table"+suffix, ConflictType.Ignore)
        assert table_obj

        with pytest.raises(InfinityException) as e:
            db_obj.get_table("test_delete_non_existent_table"+suffix).delete()

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    @trace_expected_exceptions
    @pytest.mark.parametrize('column_types', common_values.types_array)
    @pytest.mark.parametrize('column_types_example', common_values.types_example_array)
    def test_delete_table_all_row_met_the_condition(self, column_types, column_types_example, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_table_all_row_met_the_condition"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_delete_table_all_row_met_the_condition"+suffix, {"c1": {"type": column_types}},
                            ConflictType.Error)
        table_obj = db_obj.get_table("test_delete_table_all_row_met_the_condition"+suffix)
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

        db_obj.drop_table("test_delete_table_all_row_met_the_condition"+suffix, ConflictType.Error)

    def test_delete_table_no_rows_met_condition(self,suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        for i in range(len(common_values.types_array)):
            db_obj.drop_table("test_delete_table_no_rows_met_condition" + str(i)+suffix, ConflictType.Ignore)

        for i in range(len(common_values.types_array)):
            tb = db_obj.create_table("test_delete_table_no_rows_met_condition" + str(i)+suffix,
                                     {"c1": {"type": common_values.types_array[i]}}, ConflictType.Error)
            assert tb

            table_obj = db_obj.get_table("test_delete_table_no_rows_met_condition" + str(i)+suffix)
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
            db_obj.drop_table("test_delete_table_no_rows_met_condition" + str(i)+suffix, ConflictType.Error)

    def test_delete_table_with_one_block(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_table_with_one_block"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_table_with_one_block"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1} for _ in range(8192)]
        table_obj.insert(values)
        insert_res = table_obj.output(["*"]).to_df()
        print(insert_res)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_table_with_one_block"+suffix, ConflictType.Error)

    def test_delete_table_with_one_segment(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_table_with_one_segment"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_table_with_one_segment"+suffix, {"c1": {"type": "int"}},
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
        db_obj.drop_table("test_delete_table_with_one_segment"+suffix, ConflictType.Error)

        print(delete_res)

    def test_select_before_after_delete(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_select_before_after_delete"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_select_before_after_delete"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)

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
        db_obj.drop_table("test_select_before_after_delete"+suffix, ConflictType.Error)

    def test_delete_insert_data(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_insert_data"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_insert_data"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)

        # insert
        values = [{"c1": 1} for _ in range(10)]
        table_obj.insert(values)

        # delete
        table_obj.delete("c1 = 1")
        delete_res = table_obj.output(["*"]).to_df()
        print(delete_res)
        db_obj.drop_table("test_delete_insert_data"+suffix, ConflictType.Error)

    @pytest.mark.slow
    def test_delete_inserted_long_before_data(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_inserted_long_before_data"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_inserted_long_before_data"+suffix, {"c1": {"type": "int"}},
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
        db_obj.drop_table("test_delete_inserted_long_before_data"+suffix, ConflictType.Error)

    def test_delete_dropped_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")

        with pytest.raises(InfinityException) as e:
            db_obj.drop_table("test_delete_dropped_table"+suffix)
            table_obj = db_obj.get_table("test_delete_dropped_table"+suffix)
            table_obj.delete("c1 = 0")
            assert table_obj
            db_obj.drop_table("test_delete_dropped_table"+suffix)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

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
    def test_various_expression_in_where_clause(self, column_types, column_types_example, http, suffix):
        if column_types == "vector, 3, float" and column_types_example == [1, 2, 3] and http:
            pytest.skip(reason = "bus error, core dumped")
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_expression_in_where_clause"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_various_expression_in_where_clause"+suffix, {"c1": {"type": column_types}},
                            ConflictType.Error)

        table_obj = db_obj.get_table("test_various_expression_in_where_clause"+suffix)

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

        res = db_obj.drop_table("test_various_expression_in_where_clause"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_delete_one_block_without_expression(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_one_block_without_expression"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_one_block_without_expression"+suffix, {"c1": {"type": "int"}},
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
        res = db_obj.drop_table("test_delete_one_block_without_expression"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_delete_one_segment_without_expression(self,suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_delete_one_segment_without_expression"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_delete_one_segment_without_expression"+suffix, {"c1": {"type": "int"}},
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
        db_obj.drop_table("test_delete_one_segment_without_expression"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_filter_with_valid_expression(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_filter_expression"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_filter_expression"+suffix, {"c1": {"type": "int"}, "c2": {"type": "float"}},
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
        db_obj.drop_table("test_filter_expression"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_filter_with_invalid_expression(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_filter_expression"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_filter_expression"+suffix, {"c1": {"type": "int"}, "c2": {"type": "float"}},
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
        db_obj.drop_table("test_filter_expression"+suffix, ConflictType.Error)