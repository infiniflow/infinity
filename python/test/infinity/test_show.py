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
    def test_show(self):
        test_show_obj = TestDescribe(self.uri)
        test_show_obj._test_show_table()
        test_show_obj._test_show_columns()
        test_show_obj._test_show_big_databases()