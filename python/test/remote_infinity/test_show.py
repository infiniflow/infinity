import pytest
from common import common_values
from src.test_show import TestDescribe

class TestRemoteInfinity():
    def test_show(self):
        test_show_obj = TestDescribe(common_values.TEST_REMOTE_HOST)
        test_show_obj._test_show_table()
        test_show_obj._test_show_columns()
        test_show_obj._test_show_big_databases()