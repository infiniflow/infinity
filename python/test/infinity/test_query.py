import pytest
from common import common_values
from src.test_query import TestQuery
from infinity.common import ConflictType
from src.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index


@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
        self.test_infinity_obj = TestQuery(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_query(self):
        self.test_infinity_obj._test_query()
    def test_query_builder(self):
        # connect
        self.test_infinity_obj._test_query_builder()