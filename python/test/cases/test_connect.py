import pytest

from common import common_values
from infinity.common import ConflictType

from internal.test_connect import TestConnection

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
    request.cls.test_infinity_obj = TestConnection(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def test_connect_and_disconnect_ok(self):
        self.test_infinity_obj._test_connect_and_disconnect_ok()

    def test_repeat_connect(self):
        self.test_infinity_obj._test_repeat_connect()

    def test_multiple_connect(self):
        self.test_infinity_obj._test_multiple_connect()

    def test_repeat_disconnect(self):
        self.test_infinity_obj._test_repeat_disconnect()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_connect_invalid_address(self):
        self.test_infinity_obj._test_connect_invalid_address()
