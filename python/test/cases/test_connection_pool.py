import pytest

from common import common_values
from infinity.common import ConflictType

from internal.test_connection_pool import TestConnectionPool

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
    request.cls.test_infinity_obj = TestConnectionPool(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def test_connection_pool(self):
        self.test_infinity_obj._test_basic()
        # self.test_infinity_obj._test_time_out()