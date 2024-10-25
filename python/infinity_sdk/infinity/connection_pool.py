from threading import Lock
import infinity
from infinity.common import NetworkAddress
import logging


class ConnectionPool(object):
    def __init__(self, uri = NetworkAddress("127.0.0.1", 23817), max_size=16):
        self.uri_ = uri
        self.max_size_ = max_size
        self.free_pool_ = []
        self.lock_ = Lock()
        for i in range(max_size):
            self._create_conn()


    def _del__(self):
        self.destroy()

    def _create_conn(self):
        infinity_coon = infinity.connect(self.uri_)
        self.free_pool_.append(infinity_coon)


    def get_conn(self):
        with self.lock_:
            if (len(self.free_pool_) == 0):
                self._create_conn()
            conn = self.free_pool_.pop()
            logging.debug("get_conn")
            return conn


    def release_conn(self, conn):
        """
        Note: User is allowed to release a connection not created by ConnectionPool, or not releasing(due to exception or some other reasons) a connection created by ConnectionPool.
        """
        with self.lock_:
            if(self.free_pool_.count(conn)):
                raise Exception("the connection has been released")
            if (len(self.free_pool_) < self.max_size_):
                self.free_pool_.append(conn)
            logging.debug("release_conn")


    def destroy(self):
        for conn in iter(self.free_pool_):
            conn.disconnect()
        self.free_pool_.clear()