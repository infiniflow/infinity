import sys
import os
import pytest
from common import common_values
from sqlglot import condition
from infinity.remote_thrift.table import traverse_conditions
import infinity
import infinity_embedded
from infinity.errors import ErrorCode
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.mark.usefixtures("local_infinity")
@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity, http):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
            self.infinity_obj = infinity_embedded.connect(self.uri)
        elif http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)
        assert self.infinity_obj

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_condition(self):
        res = (condition("c1 > 1 and c2 < 2 or c3 = 3.3"))
        print(res)
        res = traverse_conditions(condition("c1 = 1"))
        print(res)
        res = traverse_conditions(condition("-8 < c1 and c1 <= -7"))
        print(res)
        res = traverse_conditions(
            condition("(-7 < c1 or 9 <= c1) and (c1 = 3)"))
        print(res)
