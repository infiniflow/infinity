import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
import pytest
import infinity
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from common import common_values
from infinity_http import infinity_http


@pytest.mark.usefixtures("local_infinity")
@pytest.mark.usefixtures("http")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity, http):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.infinity_obj = infinity.connect(self.uri)
        if http:
            self.infinity_obj = infinity_http()
        assert self.infinity_obj

    def teardown(self):
        self.infinity_obj.disconnect()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_get_infinity(self):
        infinity_obj = ThriftInfinityClient(common_values.TEST_LOCAL_HOST)
        database_res = infinity_obj.get_database("default_db")
        print(type(database_res))
        assert database_res.error_code == ErrorCode.OK
        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_list_infinity(self):
        database_res = self.infinity_obj.list_databases()
        assert "default_db" in database_res.db_names
