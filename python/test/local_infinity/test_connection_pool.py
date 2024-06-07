import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_connection_pool import TestConnectionPool

class TestLocalInfinity():
    def test_connection_pool(self):
        test_connection_pool_obj = TestConnectionPool(common_values.TEST_LOCAL_PATH)
        test_connection_pool_obj._test_basic()
        # test_connection_pool_obj._test_time_out()