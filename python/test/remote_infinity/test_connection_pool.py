import pytest
from common import common_values
from src.test_connection_pool import TestConnectionPool

class TestRemoteInfinity():
    def test_connection_pool(self):
        test_connection_pool_obj = TestConnectionPool(common_values.TEST_REMOTE_HOST)
        test_connection_pool_obj._test_basic()
        # test_connection_pool_obj._test_time_out()