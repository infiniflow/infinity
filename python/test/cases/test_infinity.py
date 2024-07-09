import pytest
from common import common_values
from internal.test_infinity import InfinityTest

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
    request.cls.test_infinity_obj = InfinityTest(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_get_infinity(self):
        self.test_infinity_obj._test_get_database()

    def test_list_infinity(self):
        self.test_infinity_obj._test_list_database()
