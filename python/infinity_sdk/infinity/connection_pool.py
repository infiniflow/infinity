from threading import Lock, Condition
import infinity
from infinity.common import NetworkAddress
import logging
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection


class ConnectionPool(object):


    def __init__(self, uri = NetworkAddress("127.0.0.1", 23817), min_size=4, max_size=16, timeout=10.0):
        assert (min_size <= max_size)
        self.uri_ = uri
        self.min_size_ = min_size
        self.max_size_ = max_size
        self.curr_size_ = 0
        self.timeout_ = timeout
        self.free_pool_ = []
        self.created_conns_ = []
        self.cond_ = Condition()

        while(self.curr_size_ < self.min_size_):
            self._create_conn()


    def _create_conn(self):
        infinity_coon = infinity.connect(self.uri_)
        self.curr_size_ += 1
        self.free_pool_.append(infinity_coon)
        self.created_conns_.append(infinity_coon)


    def get_conn(self) -> RemoteThriftInfinityConnection:
        with self.cond_:
            while True:
                is_waken = self.cond_.wait_for(lambda: len(self.free_pool_) != 0 or self.curr_size_ < self.max_size_, self.timeout_)
                if is_waken:
                    break
                logging.warn("connection waiting...")
            if (len(self.free_pool_) == 0):
                self._create_conn()
            conn = self.free_pool_.pop()
            logging.debug("get_conn")
            return conn


    def release_conn(self, conn):
        with self.cond_:
            if(not self.created_conns_.count(conn)):
                raise Exception("the connection is unknown")
            if(self.free_pool_.count(conn)):
                raise Exception("the connection has been released")
            self.curr_size_ -= 1
            self.free_pool_.append(conn)
            self.cond_.notify_all()
            logging.debug("release_conn")


    def destroy(self):
        for conn in iter(self.created_conns_):
            conn.disconnect()