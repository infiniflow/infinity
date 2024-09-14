import importlib
import sys
import os
import pytest
from infinity.errors import ErrorCode
from common import common_values
import infinity
import infinity_embedded
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.common import ConflictType, InfinityException
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http

@pytest.mark.usefixtures("local_infinity")
@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity, http):
        if local_infinity:
            module = importlib.import_module("infinity_embedded.common")
            func = getattr(module, 'ConflictType')
            globals()['ConflictType'] = func
            func = getattr(module, 'InfinityException')
            globals()['InfinityException'] = func
            self.uri = common_values.TEST_LOCAL_PATH
            self.infinity_obj = infinity_embedded.connect(self.uri)
        elif http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)
        assert self.infinity_obj

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_to_pl(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_to_pl"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_to_pl"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_to_pl"+suffix)
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res = table_obj.output(["c1", "c2"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_pl()
        print(res)
        db_obj.drop_table("test_to_pl"+suffix, ConflictType.Error)
    def test_to_pa(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_to_pa"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_to_pa"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_to_pa"+suffix)
        table_obj.insert([{"c1": 1, "c2": 2.0}])
        print()
        res = table_obj.output(["c1", "c2"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_arrow()
        print(res)
        db_obj.drop_table("test_to_pa"+suffix, ConflictType.Error)
    def test_to_df(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_to_df"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_to_df"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)

        table_obj = db_obj.get_table("test_to_df"+suffix)
        table_obj.insert([{"c1": 1, "c2": 2.0}])
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        print(res)
        db_obj.drop_table("test_to_df"+suffix, ConflictType.Error)

    @pytest.mark.usefixtures("skip_if_http")
    def test_without_output_select_list(self, suffix):
        #from infinity_embedded.common import ConflictType, InfinityException
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_without_output_select_list"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_without_output_select_list"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)

        table_obj.insert([{"c1": 1, "c2": 2.0}])

        with pytest.raises(InfinityException) as e:
            insert_res_df = table_obj.output([]).to_df()
            insert_res_arrow = table_obj.output([]).to_arrow()
            insert_res_pl = table_obj.output([]).to_pl()
            print(insert_res_df, insert_res_arrow, insert_res_pl)

        assert e.value.args[0] == ErrorCode.EMPTY_SELECT_FIELDS

        db_obj.drop_table("test_without_output_select_list"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("condition_list", ["c1 > 0.1 and c2 < 3.0",
                                                "c1 > 0.1 and c2 < 1.0",
                                                "c1 < 0.1 and c2 < 1.0",
                                                "c1",
                                                "c1 = 0",
                                                "_row_id",
                                                "*"])
    def test_convert_test_with_valid_select_list_output(self, condition_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_valid_select_list_output"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_valid_select_list_output"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])

        insert_res_df = table_obj.output(["c1", condition_list]).to_pl()
        print(insert_res_df)

        db_obj.drop_table("test_with_valid_select_list_output"+suffix, ConflictType.Error)

    @pytest.mark.parametrize("condition_list", [pytest.param("c1 + 0.1 and c2 - 1.0", ),
                                                pytest.param("c1 * 0.1 and c2 / 1.0", ),
                                                pytest.param("c1 > 0.1 %@#$sf c2 < 1.0", ),
                                                ])
    def test_convert_test_with_invalid_select_list_output(self, condition_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_with_invalid_select_list_output"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_with_invalid_select_list_output"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])

        with pytest.raises(Exception):
            insert_res_df = table_obj.output(["c1", condition_list]).to_pl()
            print(insert_res_df)

        db_obj.drop_table("test_with_invalid_select_list_output"+suffix, ConflictType.Error)

    # skipped tests using InfinityThriftQueryBuilder which is incompatible with local infinity
    @pytest.mark.usefixtures("skip_if_http")
    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_convert_test_output_with_valid_filter_function(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_output_with_valid_filter_function"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_output_with_valid_filter_function"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        insert_res_df = InfinityThriftQueryBuilder(table_obj).output(["*"]).filter(filter_list).to_pl()
        print(str(insert_res_df))

        db_obj.drop_table("test_output_with_valid_filter_function"+suffix, ConflictType.Error)

    @pytest.mark.usefixtures("skip_if_http")
    @pytest.mark.usefixtures("skip_if_local_infinity")
    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_convert_test_output_with_invalid_filter_function(self, filter_list, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_output_with_invalid_filter_function"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_output_with_invalid_filter_function"+suffix, {
            "c1": {"type": "int"}, "c2": {"type": "float"}}, ConflictType.Error)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        with pytest.raises(Exception) as e:
            insert_res_df = InfinityThriftQueryBuilder(table_obj).output(["*"]).filter(filter_list).to_pl()
            print(str(insert_res_df))

        print(e.type)

        db_obj.drop_table("test_output_with_invalid_filter_function"+suffix, ConflictType.Error)