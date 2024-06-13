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
            self.uri = common_values.TEST_REMOTE_HOST
    def test_connection_pool(self):
        test_connection_pool_obj = TestConnectionPool(self.uri)
        test_connection_pool_obj._test_basic()
        # test_connection_pool_obj._test_time_out()