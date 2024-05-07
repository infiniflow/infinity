import time

import infinity.index as index
import pytest
import random
from threading import Thread, Lock
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool

batch_size = 500
max_count = batch_size * 200
lock = Lock()
deleting_list = []
kNumThread = 8


class TestInsertDeleteParallel:
    def test_insert_and_delete_parallel(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("insert_delete_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(
            "insert_delete_test", {"id": {"type": "int64"}, "text": {"type": "varchar"}}, ConflictType.Error
        )
        table_obj.create_index(
            "text_index", [index.IndexInfo("text", index.IndexType.FullText, [])]
        )
        connection_pool.release_conn(infinity_obj)

        count_num = [0]
        threads = []
        for i in range(kNumThread):
            threads.append(
                Thread(target=worker_thread, args=[connection_pool, count_num, i])
            )
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("insert_delete_test")
        res = table_obj.output(["*"]).to_df()
        assert len(res) == 0

        res = db_obj.drop_table("insert_delete_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


def worker_thread(connection_pool: ConnectionPool, count_num, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    table_obj = db_obj.get_table("insert_delete_test")
    while True:
        choice = random.randint(0, 1)
        start_i = 0
        lock.acquire()
        if count_num[0] < max_count and random.randint(0, 1) == 0:
            start_i = count_num[0]
            count_num[0] += batch_size
            lock.release()
            value = []
            for i in range(start_i, start_i + batch_size):
                value.append({"id": i, "text": str(random.uniform(0, 10))})
            table_obj.insert(value)
            lock.acquire()
            deleting_list.append(start_i)
            lock.release()
        else:
            if len(deleting_list) == 0:
                lock.release()
                if count_num[0] == max_count:
                    break
                else:
                    continue
            else:
                delete_index = random.randint(0, len(deleting_list) - 1)
                delete_id = deleting_list[delete_index]
                deleting_list.pop(delete_index)
                lock.release()
                try:
                    table_obj.delete(
                        f"id > {delete_id - 1} and id < {delete_id + batch_size}"
                    )
                except Exception as e:
                    return

    connection_pool.release_conn(infinity_obj)
