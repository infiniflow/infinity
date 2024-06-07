import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_convert import TestConvert

class TestRemoteInfinity():
    def test_convert(self):
        test_convert_obj = TestConvert(common_values.TEST_REMOTE_HOST)
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
        test_convert_obj = TestConvert(common_values.TEST_REMOTE_HOST)
        test_convert_obj._test_with_valid_select_list_output(condition_list)

    @pytest.mark.parametrize("condition_list", [pytest.param("c1 + 0.1 and c2 - 1.0", ),
                                                pytest.param("c1 * 0.1 and c2 / 1.0", ),
                                                pytest.param("c1 > 0.1 %@#$sf c2 < 1.0", ),
                                                ])
    def test_convert_test_with_invalid_select_list_output(self, condition_list):
        test_convert_obj = TestConvert(common_values.TEST_REMOTE_HOST)
        test_convert_obj._test_with_invalid_select_list_output(condition_list)

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
        test_convert_obj = TestConvert(common_values.TEST_REMOTE_HOST)
        test_convert_obj._test_output_with_valid_filter_function(filter_list)

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
        test_convert_obj = TestConvert(common_values.TEST_REMOTE_HOST)
        test_convert_obj._test_output_with_invalid_filter_function(filter_list)
