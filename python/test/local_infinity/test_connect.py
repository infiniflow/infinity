import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_basic import TestCase
from src.test_condition import TestCondition
from src.test_connect import TestConnection


class TestLocalInfinity():
    def test_connect(self):
        test_connection_obj = TestConnection(common_values.TEST_LOCAL_PATH)
        test_connection_obj._test_connect_and_disconnect_ok()
        test_connection_obj._test_repeat_connect()
        test_connection_obj._test_multiple_connect()
        test_connection_obj._test_repeat_disconnect()