import pytest
from common import common_values
from internal.test_basic import TestCase

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
    request.cls.test_infinity_obj = TestCase(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    # def test_version(self):
    #     self.test_infinity_obj._test_version()
    def test_connection(self):
        self.test_infinity_obj._test_connection()
    def test_create_db_with_invalid_name(self):
        self.test_infinity_obj._test_create_db_with_invalid_name()

    @pytest.mark.parametrize("check_data", [{"file_name": "embedding_int_dim3.csv",
                                             "data_dir": common_values.TEST_TMP_DIR}], indirect=True)
    def test_basic(self, check_data):
        self.test_infinity_obj._test_basic(check_data)