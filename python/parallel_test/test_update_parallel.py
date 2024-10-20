import time
from threading import Thread
import infinity.index as index
import pytest
import random
from infinity.common import ConflictType
from infinity.connection_pool import ConnectionPool
from infinity.errors import ErrorCode

from test_sdkbase import TestSdk

total_row_count = 1000
thread_count = 8

class TestUpdateParallel(TestSdk):
    def test_update_parallel(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("update_parallel_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("update_parallel_test", {
            "id": {"type": "int64"},
            "value": {"type": "int64"}
        }, ConflictType.Error)

        initial_data = [{"id": i, "value": 0} for i in range(total_row_count)]
        table_obj.insert(initial_data)
        connection_pool.release_conn(infinity_obj)

    def update_worker(conn_pool, thread_id):
        infinity_obj = conn_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("update_parallel_test")
        for _ in range(1000):
            row_id = random.randint(0, total_row_count - 1)
            new_value = random.randint(1, 100)
            table_obj.update({"value": new_value}).filter(f"id = {row_id}").execute()
            print(f"thread {thread_id}: update id = {row_id}, value = {new_value}")
        conn_pool.release_conn(infinity_obj)

    threads = []
    for i in range(thread_count):
        threads.append(Thread(target=update_worker, args=(connection_pool, i)))

    for t in threads:
        t.start()
    for t in threads:
        t.join()

    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    table_obj = db_obj.get_table("update_parallel_test")
    res = table_obj.output(["*"]).to_df()
    print(res.head())

    res = db_obj.drop_table("update_parallel_test", ConflictType.Error)
    assert res.error_code == ErrorCode.OK
    connection_pool.release_conn(infinity_obj)