import sys
import os
import pytest
from common import common_values
import time
from infinity.connection_pool import ConnectionPool
from infinity.common import ConflictType

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

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_connection_pool(self, suffix):
        connection_pool = ConnectionPool(uri=self.uri, min_size=4, max_size=8)
        assert len(connection_pool.free_pool_) == 4

        infinity_obj = connection_pool.get_conn()
        assert infinity_obj
        assert len(connection_pool.free_pool_) == 3

        infinity_obj.drop_database("my_database"+suffix, conflict_type=ConflictType.Ignore)
        db = infinity_obj.create_database("my_database"+suffix)
        assert db

        res = infinity_obj.list_databases()
        print(res.db_names)
        assert res is not None

        res.db_names.sort()

        assert "default_db" in res.db_names
        assert "my_database"+suffix in res.db_names

        infinity_obj.drop_database("my_database"+suffix, conflict_type=ConflictType.Error)

        connection_pool.release_conn(infinity_obj)
        assert len(connection_pool.free_pool_) == 4

        try:
            connection_pool.release_conn(infinity_obj)
        except Exception as e:
            print(e)
        else:
            assert "no exception when double release" == 0
        connection_pool.destroy()

    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_time_out(self):
        #test timeout is ok
        connection_pool = ConnectionPool(uri=self.uri, min_size=4, max_size=8, timeout=5.0)
        begin_time = time.time()
        try:
            while True:
                connection_pool.get_conn()
        except Exception as e:
            print(e)
        assert time.time() - begin_time < 10