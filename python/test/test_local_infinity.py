import pytest

from common import common_values
from src.test_basic import TestCase
from src.test_condition import TestCondition
from src.test_connect import TestConnection
from src.test_connection_pool import TestConnectionPool
from src.test_convert import TestConvert

class TestLocalInfinity():
    def test_basic(self):
        test_case_obj = TestCase(common_values.TEST_LOCAL_PATH)
        test_case_obj._test_version()
        test_case_obj._test_connection()
        test_case_obj._test_create_db_with_invalid_name()
        test_case_obj._test_basic()

    def test_condition(self):
        test_condition_obj = TestCondition(common_values.TEST_LOCAL_PATH)
        test_condition_obj._test_traverse_conditions()

    def test_connect(self):
        test_connection_obj = TestConnection(common_values.TEST_LOCAL_PATH)
        test_connection_obj._test_connect_and_disconnect_ok()
        test_connection_obj._test_repeat_connect()
        test_connection_obj._test_multiple_connect()
        test_connection_obj._test_repeat_disconnect()

    def test_connection_pool(self):
        test_connection_pool_obj = TestConnectionPool(common_values.TEST_LOCAL_PATH)
        test_connection_pool_obj._test_basic()
        # test_connection_pool_obj._test_time_out()

    def test_convert(self):
        test_convert_obj = TestConvert(common_values.TEST_LOCAL_PATH)
        test_convert_obj._test_to_pl()
        test_convert_obj._test_to_pa()
        test_convert_obj._test_to_df()
        test_convert_obj._test_without_output_select_list()

    @pytest.mark.parametrize("condition_list", ["c1 > 0.1 and c2 < 3.0",
                                                "c1 > 0.1 and c2 < 1.0",
                                                "c1 < 0.1 and c2 < 1.0",
                                                "c1",
                                                "c1 = 0",
                                                "_row_id",
                                                "*"])
    def test_convert_test_with_valid_select_list_output(self, condition_list):
        test_convert_obj = TestConvert(common_values.TEST_LOCAL_PATH)
        test_convert_obj._test_with_valid_select_list_output(condition_list)

    @pytest.mark.parametrize("condition_list", [pytest.param("c1 + 0.1 and c2 - 1.0", ),
                                                pytest.param("c1 * 0.1 and c2 / 1.0", ),
                                                pytest.param("c1 > 0.1 %@#$sf c2 < 1.0", ),
                                                ])
    def test_convert_test_with_invalid_select_list_output(self, condition_list):
        test_convert_obj = TestConvert(common_values.TEST_LOCAL_PATH)
        test_convert_obj._test_with_invalid_select_list_output(condition_list)

    # skipped tests using InfinityThriftQueryBuilder which is incompatible with local infinity todo modify
    @pytest.mark.skip()
    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_convert_test_output_with_valid_filter_function(self, filter_list):
        test_convert_obj = TestConvert(common_values.TEST_LOCAL_PATH)
        test_convert_obj._test_output_with_valid_filter_function(filter_list)

    @pytest.mark.skip()
    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_convert_test_output_with_invalid_filter_function(self, filter_list):
        test_convert_obj = TestConvert(common_values.TEST_LOCAL_PATH)
        test_convert_obj._test_output_with_invalid_filter_function(filter_list)



