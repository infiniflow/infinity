import pytest
from common import common_values
from internal.test_export import TestExport
from infinity.common import ConflictType
from internal.utils import trace_expected_exceptions
from infinity.errors import ErrorCode


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
    request.cls.test_infinity_obj = TestExport(uri)
    yield
    request.cls.test_infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    @pytest.fixture
    def skip_setup_marker(self, request):
        request.node.skip_setup = True

    def test_export_csv(self):
        self.test_infinity_obj._test_export_csv()

    def test_export_jsonl(self):
        self.test_infinity_obj._test_export_jsonl()

    def test_export_fvecs(self):
        self.test_infinity_obj._test_export_fvecs()