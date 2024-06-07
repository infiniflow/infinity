from common import common_values
from src.test_basic import TestCase
from src.test_condition import TestCondition
from src.test_connect import TestConnection

class TestRemoteInfinity():
    def test_case(self):
        test_case_obj = TestCase(common_values.TEST_REMOTE_HOST)
        test_case_obj._test_version()
        test_case_obj._test_connection()
        test_case_obj._test_create_db_with_invalid_name()
        test_case_obj._test_basic()
    def test_condition(self):
        test_condition_obj = TestCondition(common_values.TEST_REMOTE_HOST)
        test_condition_obj._test_traverse_conditions()

    def test_connect(self):
        test_connection_obj = TestConnection(common_values.TEST_REMOTE_HOST)
        test_connection_obj._test_connect_and_disconnect_ok()
        test_connection_obj._test_connect_invalid_address()
        test_connection_obj._test_repeat_connect()
        test_connection_obj._test_multiple_connect()
        test_connection_obj._test_repeat_disconnect()