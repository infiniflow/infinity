import pytest
from common import common_values
from src.test_show import TestDescribe

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
        self.test_infinity_obj = TestDescribe(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_show(self):
        self.test_infinity_obj._test_show_table()
        self.test_infinity_obj._test_show_columns()
        self.test_infinity_obj._test_show_big_databases()