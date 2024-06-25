import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_connection_pool import TestConnectionPool

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.test_infinity_obj = TestConnectionPool(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_connection_pool(self):
        self.test_infinity_obj._test_basic()
        # self.test_infinity_obj._test_time_out()