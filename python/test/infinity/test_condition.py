import pytest
from common import common_values
from src.test_condition import TestCondition

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
    def test_condition(self):
        test_condition_obj = TestCondition(self.uri)
        test_condition_obj._test_traverse_conditions()
