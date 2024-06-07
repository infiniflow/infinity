import pytest
from common import common_values
from src.test_infinity import TestInfinity

class TestLocalInfinity():
    def test_infinity(self):
        test_infinity_obj = TestInfinity(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_list_database()