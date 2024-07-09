import pytest
from common import common_values
from internal.test_condition import TestCondition

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    def test_condition(self):
        test_condition = TestCondition()
        test_condition._test_traverse_conditions()
