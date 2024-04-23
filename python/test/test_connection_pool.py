from common import common_values
import time
from infinity.connection_pool import ConnectionPool
from infinity.common import ConflictType
from test_sdkbase import TestSdk

class TestConnectionPool(TestSdk):
    #test whether the connection get from the pool could work properly and the management of pool is correct.
    def test_basic(self):
        connction_pool = ConnectionPool(uri=common_values.TEST_REMOTE_HOST, min_size=4, max_size=8)
        assert len(connction_pool.free_pool_) == 4

        infinity_obj = connction_pool.get_conn()
        assert infinity_obj
        assert len(connction_pool.free_pool_) == 3

        infinity_obj.drop_database("my_database", conflict_type=ConflictType.Ignore)
        db = infinity_obj.create_database("my_database")
        assert db

        res = infinity_obj.list_databases()
        print(res.db_names)
        assert res is not None

        res.db_names.sort()
        assert res.db_names[0] == "default"
        assert res.db_names[1] == 'my_database'

        connction_pool.release_conn(infinity_obj)
        assert len(connction_pool.free_pool_) == 4

        try:
            connction_pool.release_conn(infinity_obj)
        except Exception as e:
            print(e)
        else:
            assert "no exception when double release" == 0
        connction_pool.destroy()

    def test_time_out(self):
        #test timeout is ok
        connction_pool = ConnectionPool(uri=common_values.TEST_REMOTE_HOST, min_size=4, max_size=8, timeout=5.0)
        begin_time = time.time()
        try:
            while True:
                connction_pool.get_conn()
        except Exception as e:
            print(e)
        assert time.time() - begin_time < 10
