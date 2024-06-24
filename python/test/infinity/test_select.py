import pytest
from common import common_values
from src.test_select import TestSelect
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index
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
    request.cls.test_infinity_obj = TestSelect(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    # def test_version(self):
    #     self.test_infinity_obj._test_version()
    def test_select(self):
        self.test_infinity_obj._test_select()
    def test_select_aggregate(self):
        self.test_infinity_obj._test_select_aggregate()
    def test_select_varchar(self):
        self.test_infinity_obj._test_select_varchar()
    def test_select_big(self):
        self.test_infinity_obj._test_select_big()
    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_embedding_int32(self, check_data):
        self.test_infinity_obj._test_select_embedding_int32(check_data)
    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_float_dim4.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_embedding_float(self, check_data):
        self.test_infinity_obj._test_select_embedding_float(check_data)
    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_select_big_embedding(self, check_data):
        self.test_infinity_obj._test_select_big_embedding(check_data)
    def test_select_same_output(self):
        self.test_infinity_obj._test_select_same_output()
    def test_empty_table(self):
        self.test_infinity_obj._test_empty_table()
    @pytest.mark.parametrize("filter_list", [
        "c1 > 10",
        "c2 > 1",
        "c1 > 0.1 and c2 < 3.0",
        "c1 > 0.1 and c2 < 1.0",
        "c1 < 0.1 and c2 < 1.0",
        "c1 < 0.1 and c1 > 1.0",
        "c1 = 0",
    ])
    def test_valid_filter_expression(self, filter_list):
        self.test_infinity_obj._test_valid_filter_expression(filter_list)
    @pytest.mark.parametrize("filter_list", [
        pytest.param("c1"),
        pytest.param("_row_id"),
        pytest.param("*"),
        pytest.param("#@$%@#f"),
        pytest.param("c1 + 0.1 and c2 - 1.0"),
        pytest.param("c1 * 0.1 and c2 / 1.0"),
        pytest.param("c1 > 0.1 %@#$sf c2 < 1.0"),
    ])
    def test_invalid_filter_expression(self, filter_list):
        self.test_infinity_obj._test_invalid_filter_expression(filter_list)
