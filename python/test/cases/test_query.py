import pytest
from common import common_values
from internal.test_query import TestQuery
from infinity.common import ConflictType
from internal.utils import trace_expected_exceptions
from infinity.errors import ErrorCode
import infinity.index as index

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestQuery(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_query(self):
        self.test_infinity_obj._test_query()
    def test_query_builder(self):
        # connect
        self.test_infinity_obj._test_query_builder()