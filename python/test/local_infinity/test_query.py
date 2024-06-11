import pytest
from common import common_values
from src.test_query import TestQuery
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index


class TestLocalInfinity():
    def test_query_builder(self):
        # connect
        test_infinity_obj = TestQuery(common_values.TEST_LOCAL_PATH)
        test_infinity_obj._test_query_builder()