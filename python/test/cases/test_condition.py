import pytest
from common import common_values
from internal.test_condition import TestCondition

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.test_infinity_obj = TestCondition(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_condition(self):
        self.test_infinity_obj._test_traverse_conditions()
