import time

import infinity.index as index
import pytest
import random
from threading import Thread, Lock
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool

max_count = 500 * 20
kNumThread = 8
lock = Lock()

class TestInsertParallel:
    def test_insert_parallel(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("parallel_insert_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("parallel_insert_test", {
            "id": "int64", "text": "varchar"}, ConflictType.Error)
        table_obj.create_index("text_index", [index.IndexInfo("text",
                                                              index.IndexType.FullText, [])])
        connection_pool.release_conn(infinity_obj)

        count_num = [0]
        threads = []
        for i in range(kNumThread):
            threads.append(Thread(target=worker_thread, args=[connection_pool, max_count // kNumThread, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default")
        table_obj = db_obj.get_table("parallel_insert_test")
        res = table_obj.output(['*']).to_df()
        print(res)
        assert len(res) == max_count

        res = db_obj.drop_table("parallel_insert_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


def worker_thread(connection_pool: ConnectionPool, count_num, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default")
    table_obj = db_obj.get_table("parallel_insert_test")

    value = []
    for i in range(count_num):
        value.append({"id": i, "text": str(random.uniform(0, 10))})
        table_obj.insert(value)
        value.clear()

    connection_pool.release_conn(infinity_obj)
