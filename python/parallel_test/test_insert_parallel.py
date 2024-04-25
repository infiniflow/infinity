import time

import infinity.index as index
import pytest
import random
from threading import Thread, Lock
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool


class TestInsertParallel:
    def test_insert_parallel(self, get_infinity_connection_pool):
        total_row_count = 500 * 40
        insert_thread_count = 8
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("parallel_insert_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("parallel_insert_test", {
            "id": "int64",
            "text": "varchar",
            "c_tiny_int": "int8",
            "c_small_int": "int16",
            "c_int": "int32",
            "c_float": "float32",
            "c_double": "float64",
            "c_vector": "vector, 4, float"
        }, ConflictType.Error)
        table_obj.create_index("text_index", [index.IndexInfo("text",
                                                              index.IndexType.FullText, [])])
        connection_pool.release_conn(infinity_obj)

        threads = []
        for i in range(insert_thread_count):
            threads.append(
                Thread(target=insert_thread, args=[connection_pool, total_row_count // insert_thread_count, i]))

        start_ts = time.time()
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()
        print(f"test_insert_parallel: cost {time.time() - start_ts} s")

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("parallel_insert_test")

        res = table_obj.output(['*']).to_df()
        print(res)
        assert len(res) == total_row_count

        res = db_obj.drop_table("parallel_insert_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_insert_one_thread(self, get_infinity_connection_pool):

        total_row_count = 500 * 40
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("parallel_insert_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("parallel_insert_test", {
            "id": "int64",
            "text": "varchar",
            "c_tiny_int": "int8",
            "c_small_int": "int16",
            "c_int": "int32",
            "c_float": "float32",
            "c_double": "float64",
            "c_vector": "vector, 4, float"
        }, ConflictType.Error)
        table_obj.create_index("text_index", [index.IndexInfo("text",
                                                              index.IndexType.FullText, [])])

        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("parallel_insert_test")

        start_ts = time.time()
        value = []
        for i in range(total_row_count):
            value.append({"id": i,
                          "text": str(random.uniform(0, 10)),
                          "c_tiny_int": i,
                          "c_small_int": i,
                          "c_int": i,
                          "c_float": i,
                          "c_double": i,
                          "c_vector": [1.0] * 4})
            table_obj.insert(value)
            value.clear()
        print(f"test_insert_one_thread: cost {time.time() - start_ts} s")
        res = table_obj.output(['*']).to_df()
        print(res)
        assert len(res) == total_row_count

        res = db_obj.drop_table("parallel_insert_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        connection_pool.release_conn(infinity_obj)


    def test_insert_and_count_star_parallel(self, get_infinity_connection_pool):
        total_row_count = 500 * 40
        count_star_count = 100
        insert_thread_count = 8
        count_star_thread_count = 4
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("parallel_insert_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("parallel_insert_test", {
            "id": "int64",
            "text": "varchar",
            "c_tiny_int": "int8",
            "c_small_int": "int16",
            "c_int": "int32",
            "c_float": "float32",
            "c_double": "float64",
            "c_vector": "vector, 4, float"
        }, ConflictType.Error)
        table_obj.create_index("text_index", [index.IndexInfo("text",
                                                              index.IndexType.FullText, [])])
        connection_pool.release_conn(infinity_obj)

        threads = []
        for i in range(insert_thread_count):
            threads.append(
                Thread(target=insert_thread, args=[connection_pool, total_row_count // insert_thread_count, i]))

        for i in range(count_star_thread_count):
            threads.append(
                Thread(target=count_star_thread, args=[connection_pool, count_star_count, i]))

        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table("parallel_insert_test")

        res = table_obj.output(['*']).to_df()
        print(res)
        assert len(res) == total_row_count

        res = db_obj.drop_table("parallel_insert_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK


def insert_thread(connection_pool: ConnectionPool, count_num, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    table_obj = db_obj.get_table("parallel_insert_test")

    value = []
    for i in range(count_num):
        value.append({"id": i,
                      "text": str(random.uniform(0, 10)),
                      "c_tiny_int": i,
                      "c_small_int": i,
                      "c_int": i,
                      "c_float": i,
                      "c_double": i,
                      "c_vector": [1.0] * 4})
        table_obj.insert(value)
        value.clear()

    connection_pool.release_conn(infinity_obj)


def count_star_thread(connection_pool: ConnectionPool, loop_count, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    table_obj = db_obj.get_table("parallel_insert_test")

    for i in range(loop_count):
        table_obj.filter().output(["count(*)"]).to_pl()

    connection_pool.release_conn(infinity_obj)

