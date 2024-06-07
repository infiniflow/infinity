import pytest
from common import common_values
from src.test_condition import TestCondition

class TestLocalInfinity():
    def test_condition(self):
        test_condition_obj = TestCondition(common_values.TEST_LOCAL_PATH)
        test_condition_obj._test_traverse_conditions()
