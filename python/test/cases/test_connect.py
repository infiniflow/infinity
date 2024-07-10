import pytest

from common import common_values
from infinity.common import ConflictType

from internal.test_basic import TestCase
from internal.test_condition import TestCondition
from internal.test_connect import TestConnection


@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.test_infinity_obj = TestConnection(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_connect_and_disconnect_ok(self):
        self.test_infinity_obj._test_connect_and_disconnect_ok()

    def test_repeat_connect(self):
        self.test_infinity_obj._test_repeat_connect()

    def test_multiple_connect(self):
        self.test_infinity_obj._test_multiple_connect()

    def test_repeat_disconnect(self):
        self.test_infinity_obj._test_repeat_disconnect()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_connect_invalid_address(self):
        self.test_infinity_obj._test_connect_invalid_address()
