import pytest
from common import common_values
from src.test_delete import TestDelete
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
import numpy as np
import pandas as pd

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestDelete(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    def test_delete(self):
        # self.test_infinity_obj._test_version()
        self.test_infinity_obj._test_delete()

    def test_delete_empty_table(self):
        self.test_infinity_obj._test_delete_empty_table()

    def test_delete_non_existent_table(self):
        self.test_infinity_obj._test_delete_non_existent_table()

    @trace_expected_exceptions
    @pytest.mark.parametrize('column_types', common_values.types_array)
    @pytest.mark.parametrize('column_types_example', common_values.types_example_array)
    def test_delete_table_all_row_met_the_condition(self, column_types, column_types_example):
        self.test_infinity_obj._test_delete_table_all_row_met_the_condition(column_types, column_types_example)

    def test_delete_table_no_rows_met_condition(self):
        self.test_infinity_obj._test_delete_table_no_rows_met_condition()

    def test_delete_table_with_one_block(self):
        self.test_infinity_obj._test_delete_table_with_one_block()

    def test_delete_table_with_one_segment(self):
        self.test_infinity_obj._test_delete_table_with_one_segment()

    def test_delete_table_with_one_block_and_one_segment(self):
        self.test_infinity_obj._test_select_before_after_delete()

    def test_delete_table_with_one_block_and_one_segment(self):
        self.test_infinity_obj._test_delete_insert_data()

    @pytest.mark.slow
    def test_delete_inserted_long_before_data(self):
        self.test_infinity_obj._test_delete_inserted_long_before_data()

    def test_delete_dropped_table(self):
        self.test_infinity_obj._test_delete_dropped_table()

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
    def test_various_expression_in_where_clause(self, column_types, column_types_example):
        self.test_infinity_obj._test_various_expression_in_where_clause(column_types, column_types_example)

    def test_delete_one_block_without_expression(self):
        self.test_infinity_obj._test_delete_one_block_without_expression()

    def test_delete_one_segment_without_expression(self):
        self.test_infinity_obj._test_delete_one_segment_without_expression()

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_filter_with_valid_expression(self, filter_list):
        self.test_infinity_obj._test_filter_with_valid_expression(filter_list)
    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_filter_with_invalid_expression(self, filter_list):
        self.test_infinity_obj._test_filter_with_invalid_expression(filter_list)
