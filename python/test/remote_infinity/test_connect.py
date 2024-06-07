import pytest
from common import common_values
from src.test_connect import TestConnection

class TestRemoteInfinity():
    def test_connect(self):
        test_connection_obj = TestConnection(common_values.TEST_REMOTE_HOST)
        test_connection_obj._test_connect_and_disconnect_ok()
        test_connection_obj._test_connect_invalid_address()
        test_connection_obj._test_repeat_connect()
        test_connection_obj._test_multiple_connect()
        test_connection_obj._test_repeat_disconnect()