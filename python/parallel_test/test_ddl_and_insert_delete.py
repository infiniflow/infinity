import time
import infinity.index as index
import pytest
import random
from threading import Thread
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool
from infinity.table import Table
from infinity.db import Database

kRunningTime = 30
kNumThread = 4

table_name = "insert_delete_ddl_test"
index_name = "c1 verctor index"


class TestInsertDeleteUpdate:
    def test_insert_delete_ddl_parallel(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool

        threads = []
        end_time = time.time() + kRunningTime
        for i in range(kNumThread):
            threads.append(Thread(target=random_exec, args=[connection_pool, end_time, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()


def create_table(db_obj: Database):
    try:
        db_obj.create_table(table_name, {"tag": {"type": "int"}, "c1": {"type": "vector,4,float"}}, ConflictType.Error)
    except Exception as e:
        return


def drop_table(db_obj: Database):
    try:
        db_obj.drop_table(table_name, ConflictType.Ignore)
    except Exception as e:
        return


def create_index(db_obj: Database):
    try:
        table_obj = db_obj.get_table(table_name)
        table_obj.create_index(index_name,
                               [index.IndexInfo("c1", index.IndexType.Hnsw,
                                                [index.InitParameter("M", "16"),
                                                 index.InitParameter("ef_construction", "50"),
                                                 index.InitParameter("ef", "50"), index.InitParameter("metric", "l2")
                                                 ])], ConflictType.Error)
    except Exception as e:
        return


def drop_index(db_obj: Database):
    try:
        table_obj = db_obj.get_table(table_name)
        table_obj.drop_index(index_name)
    except Exception as e:
        return


def insert(db_obj: Database):
    try:
        table_obj = db_obj.get_table(table_name)
        value = []
        for i in range(100):
            value.append({"tag": random.randint(0, 9),
                          "c1": [random.random(), random.random(), random.random(), random.random()]})
        table_obj.insert(value)
        res = table_obj.output(['*']).to_df()
        print(res)
    except Exception as e:
        return


def delete(db_obj: Database):
    try:
        table_obj = db_obj.get_table(table_name)
        table_obj.delete(f"tag = {random.randint(0, 9)}")
    except Exception as e:
        return


def update(db_obj: Database):
    try:
        table_obj = db_obj.get_table(table_name)
        table_obj.update(f"tag = {random.randint(0, 9)}", [{"tag": 0, "c1": [0.0, 0.0, 0.0, 0.0]}])
    except Exception as e:
        return


def random_exec(connection_pool: ConnectionPool, end_time, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    while time.time() < end_time:
        rand_v = random.randint(0, 99)
        if rand_v == 0:
            create_table(db_obj)
        elif rand_v == 1:
            drop_table(db_obj)
        elif 2 <= rand_v < 4:
            create_index(db_obj)
        elif 4 <= rand_v < 6:
            drop_index(db_obj)
        elif 6 <= rand_v < 48:
            insert(db_obj)
        elif 48 <= rand_v < 90:
            delete(db_obj)
        else:
            update(db_obj)
    connection_pool.release_conn(infinity_obj)
