import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_basic import TestCase
from src.test_condition import TestCondition
from src.test_connect import TestConnection


@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
    def test_connect(self):
        test_connection_obj = TestConnection(self.uri)
        test_connection_obj._test_connect_and_disconnect_ok()
        test_connection_obj._test_repeat_connect()
        test_connection_obj._test_multiple_connect()
        test_connection_obj._test_repeat_disconnect()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_connect_invalid_address(self):
        test_connection_obj = TestConnection(self.uri)
        test_connection_obj._test_connect_invalid_address()
