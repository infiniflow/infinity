import pytest
from common import common_values
from src.test_infinity import InfinityTest

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_get_infinity(self):
        test_infinity_obj = InfinityTest(self.uri)
        test_infinity_obj._test_get_database()
    def test_list_infinity(self):
        test_infinity_obj = InfinityTest(self.uri)
        test_infinity_obj._test_list_database()
