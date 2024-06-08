import pytest
from common import common_values
from src.test_query import TestQuery
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index


class TestRemoteInfinity():
    def test_query(self):
        test_infinity_obj = TestQuery(common_values.TEST_REMOTE_HOST)
        test_infinity_obj._test_query()
    def test_query_builder(self):
        # connect
        test_infinity_obj = TestQuery(common_values.TEST_REMOTE_HOST)
        test_infinity_obj._test_query_builder()