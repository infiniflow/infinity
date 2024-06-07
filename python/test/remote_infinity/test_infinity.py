import pytest
from common import common_values
from src.test_infinity import TestInfinity

class TestRemoteInfinity():
    def test_infinity(self):
        test_infinity_obj = TestInfinity(common_values.TEST_REMOTE_HOST)
        test_infinity_obj._test_get_database()
        test_infinity_obj._test_list_database()