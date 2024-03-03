import pytest
from infinity.errors import ErrorCode

from common import common_values
import infinity
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder


class TestConvert:
    def test_to_pl(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_to_pl", True)
        db_obj.create_table("test_to_pl", {
            "c1": "int", "c2": "float"}, None)

        table_obj = db_obj.get_table("test_to_pl")
        table_obj.insert([{"c1": 1, "c2": 2}])
        print()
        res = table_obj.output(["c1", "c2"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_pl()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_pl()
        print(res)

    def test_to_pa(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_to_pa", True)
        db_obj.create_table("test_to_pa", {
            "c1": "int", "c2": "float"}, None)

        table_obj = db_obj.get_table("test_to_pa")
        table_obj.insert([{"c1": 1, "c2": 2.0}])
        print()
        res = table_obj.output(["c1", "c2"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_arrow()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_arrow()
        print(res)

    def test_to_df(self):
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_to_pa", True)
        db_obj.create_table("test_to_pa", {
            "c1": "int", "c2": "float"}, None)

        table_obj = db_obj.get_table("test_to_pa")
        table_obj.insert([{"c1": 1, "c2": 2.0}])
        print()
        res = table_obj.output(["c1", "c2"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c1"]).to_df()
        print(res)
        res = table_obj.output(["c1", "c2", "c1"]).to_df()
        print(res)

    def test_without_output_select_list(self):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_without_output_select_list", True)
        table_obj = db_obj.create_table("test_without_output_select_list", {
            "c1": "int", "c2": "float"}, None)

        table_obj.insert([{"c1": 1, "c2": 2.0}])
        with pytest.raises(Exception, match="ERROR:3050*"):
            insert_res_df = table_obj.output([]).to_df()
            insert_res_arrow = table_obj.output([]).to_arrow()
            insert_res_pl = table_obj.output([]).to_pl()
            print(insert_res_df, insert_res_arrow, insert_res_pl)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="Cause core dumped.")
    @pytest.mark.parametrize("condition_list", ["c1 > 0.1 and c2 < 3.0",
                                                "c1 > 0.1 and c2 < 1.0",
                                                "c1 < 0.1 and c2 < 1.0",
                                                "c1 + 0.1 and c2 - 1.0",
                                                "c1 * 0.1 and c2 / 1.0",
                                                "c1 > 0.1 %@#$sf c2 < 1.0",
                                                "c1",
                                                "c1 = 0",
                                                "_row_id",
                                                "*"])
    def test_with_various_select_list_output(self, condition_list):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_without_output_select_list", True)
        table_obj = db_obj.create_table("test_without_output_select_list", {
            "c1": "int", "c2": "float"}, None)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])

        insert_res_df = table_obj.output(["c1", condition_list]).to_pl()
        print(insert_res_df)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    @pytest.mark.skip(reason="Cause core dumped.")
    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1",
        "c1 = 0",
        "_row_id",
        "*",
        "#@$%@#f",
        "c1 + 0.1 and c2 - 1.0",
        "c1 * 0.1 and c2 / 1.0",
        "c1 > 0.1 %@#$sf c2 < 1.0",
    ])
    def test_output_filter_function(self, filter_list):
        # connect
        infinity_obj = infinity.connect(common_values.TEST_REMOTE_HOST)
        db_obj = infinity_obj.get_database("default")
        db_obj.drop_table("test_output_filter_function", True)
        table_obj = db_obj.create_table("test_output_filter_function", {
            "c1": "int", "c2": "float"}, None)
        table_obj.insert([{"c1": 1, "c2": 2.0},
                          {"c1": 10, "c2": 2.0},
                          {"c1": 100, "c2": 2.0},
                          {"c1": 1000, "c2": 2.0},
                          {"c1": 10000, "c2": 2.0}])
        # TODO add more filter function
        insert_res_df = InfinityThriftQueryBuilder(table_obj).output(["*"]).filter(filter_list).to_pl()
        print(str(insert_res_df))

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
