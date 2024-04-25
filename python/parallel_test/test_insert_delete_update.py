import time

import pytest
import random
from threading import Thread
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool
from infinity.table import Table

kRunningTime = 30
kNumThread = 4


class TestInsertDeleteUpdate:
    @pytest.mark.skip(
        reason="#issue 1087 Decrease row count exceed actual row count@src/storage/meta/entry/segment_entry.cppm:184")
    def test_insert_delete_update_parallel_vec(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("insert_delete_update_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("insert_delete_update_test",
                                        {"tag": {"type": "int"}, "c1": {"type": "vector,4,float"}}, ConflictType.Error)
        connection_pool.release_conn(infinity_obj)

        threads = []
        end_time = time.time() + kRunningTime
        for i in range(kNumThread):
            threads.append(Thread(target=random_exec, args=[connection_pool, end_time, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("insert_delete_update_test")
        res = table_obj.output(['*']).to_df()
        print(res)


def insert(table_obj: Table):
    try:
        value = []
        for i in range(100):
            value.append({"tag": random.randint(0, 9),
                          "c1": [random.random(), random.random(), random.random(), random.random()]})
        table_obj.insert(value)
    except Exception as e:
        print(e)


def delete(table_obj: Table):
    try:
        table_obj.delete(f"tag = {random.randint(0, 9)}")
    except Exception as e:
        print(e)


def updata(table_obj: Table):
    try:
        table_obj.update(f"tag = {random.randint(0, 9)}", [{"tag": 0, "c1": [0.0, 0.0, 0.0, 0.0]}])
    except Exception as e:
        print(e)


def search(table_obj: Table):
    res = table_obj.output(['*']).to_df()
    print(res)


def random_exec(connection_pool: ConnectionPool, end_time, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    table_obj = db_obj.get_table("insert_delete_update_test")
    while time.time() < end_time:
        rand_v = random.randint(0, 4)
        if rand_v == 0:
            print(thread_id, "insert")
            insert(table_obj)
        elif rand_v == 1:
            print(thread_id, "delete")
            delete(table_obj)
        elif rand_v == 2:
            print(thread_id, "update")
            updata(table_obj)
        elif rand_v == 3:
            print(thread_id, "search")
            search(table_obj)
    connection_pool.release_conn(infinity_obj)
