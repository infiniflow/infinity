import pytest
from common import common_values
from src.test_basic import TestCase

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.test_infinity_obj = TestCase(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

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