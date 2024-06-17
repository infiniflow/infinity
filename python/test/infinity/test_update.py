import pytest
from common import common_values
from src.test_update import TestUpdate
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index
import os

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_REMOTE_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestUpdate(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def test_version(self):
        self.test_infinity_obj._test_version()
    def test_update(self):
        self.test_infinity_obj._test_update()
    @trace_expected_exceptions
    def test_update_empty_table(self):
        self.test_infinity_obj._test_update_empty_table()
    def test_update_non_existent_table(self):
        self.test_infinity_obj._test_update_non_existent_table()
    @trace_expected_exceptions
    def test_update_no_row_is_met_the_condition(self):
        self.test_infinity_obj._test_update_no_row_is_met_the_condition()
    @trace_expected_exceptions
    def test_update_all_row_is_met_the_condition(self):
        self.test_infinity_obj._test_update_all_row_is_met_the_condition()
    def test_update_table_with_one_block(self):
        self.test_infinity_obj._test_update_table_with_one_block()
    def test_update_table_with_one_segment(self):
        self.test_infinity_obj._test_update_table_with_one_segment()
    def test_update_before_delete(self):
        self.test_infinity_obj._test_update_before_delete()
    def test_update_inserted_data(self):
        self.test_infinity_obj._test_update_inserted_data()
    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST")!="1", reason="Taking too much time.")
    def test_update_inserted_long_before(self):
        self.test_infinity_obj._test_update_inserted_long_before()
    def test_update_dropped_table(self):
        self.test_infinity_obj._test_update_dropped_table()
    @pytest.mark.parametrize("types", ["varchar"])
    @pytest.mark.parametrize("types_example", [[1, 2, 3]])
    def test_update_invalid_value(self, types, types_example):
        self.test_infinity_obj._test_update_invalid_value(types, types_example)
    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        1,
        1.333,
        "1",
    ])
    def test_update_new_value(self, types, types_example):
        self.test_infinity_obj._test_update_new_value(types, types_example)
    @pytest.mark.parametrize("types", ["int", "float"])
    @pytest.mark.parametrize("types_example", [
        pytest.param([1, 2, 3])
    ])
    def test_update_invalid_value(self, types, types_example):
        self.test_infinity_obj._test_update_invalid_value(types, types_example)
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
        self.test_infinity_obj._test_valid_filter_expression(filter_list, types_example)
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
        self.test_infinity_obj._test_invalid_filter_expression(filter_list, types_example)
