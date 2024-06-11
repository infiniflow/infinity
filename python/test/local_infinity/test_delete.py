import pytest
from common import common_values
from src.test_delete import TestDelete
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
import numpy as np
import pandas as pd

class TestLocalInfinity():
    def test_delete(self):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_version()
        test_infinity_obj._test_delete()

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_empty_table(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_empty_table(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_non_existent_table(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_non_existent_table(get_infinity_db)

    @trace_expected_exceptions
    @pytest.mark.parametrize('column_types', common_values.types_array)
    @pytest.mark.parametrize('column_types_example', common_values.types_example_array)
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_all_row_met_the_condition(self, get_infinity_db, column_types, column_types_example):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_table_all_row_met_the_condition(get_infinity_db, column_types, column_types_example)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_no_rows_met_condition(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_table_no_rows_met_condition(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_with_one_block(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_table_with_one_block(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_with_one_segment(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_table_with_one_segment(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_with_one_block_and_one_segment(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_select_before_after_delete(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_table_with_one_block_and_one_segment(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_insert_data(get_infinity_db)

    @pytest.mark.slow
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_inserted_long_before_data(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_inserted_long_before_data(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_dropped_table(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_dropped_table(get_infinity_db)

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
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_various_expression_in_where_clause(self, get_infinity_db, column_types, column_types_example):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_various_expression_in_where_clause(get_infinity_db, column_types, column_types_example)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_one_block_without_expression(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_one_block_without_expression(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_delete_one_segment_without_expression(self, get_infinity_db):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_delete_one_segment_without_expression(get_infinity_db)

    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_filter_with_valid_expression(self, get_infinity_db, filter_list):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_filter_with_valid_expression(get_infinity_db, filter_list)
    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_LOCAL_PATH], indirect=True)
    def test_filter_with_invalid_expression(self, get_infinity_db, filter_list):
        test_infinity_obj = TestDelete(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_filter_with_invalid_expression(get_infinity_db, filter_list)
