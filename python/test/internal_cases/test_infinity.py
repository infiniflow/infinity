import pytest
import infinity
from infinity.errors import ErrorCode
from infinity.remote_thrift.client import ThriftInfinityClient
from common import common_values


@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_LOCAL_HOST
        self.infinity_obj = infinity.connect(self.uri)
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
