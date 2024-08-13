import time

import infinity.index as index
import pytest
import random
from threading import Thread
from infinity.common import ConflictType
from infinity.connection_pool import ConnectionPool
from infinity.remote_thrift.infinity import RemoteThriftInfinityConnection

kRunningTime = 30
kNumThread = 8

db_names = ["dll_parallel_a", "dll_parallel_b", "dll_parallel_c", "dll_parallel_d", "dll_parallel_e"]
table_names = ["vec1", "vec2", "vec3", "fulltext1", "fulltext2", "fulltext3"]
vec_columns = [{"c1": {"type": "vector,1024,float"}, "c2": {"type": "vector,512,int"}, "c3": {"type": "int"}},
               {"c1": {"type": "vector,512,float"}, "c2": {"type": "vector,1024,int"}, "c3": {"type": "float"}}]
fulltext_columns = [{"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "varchar"}},
                    {"doctitle": {"type": "varchar"}, "docdate": {"type": "varchar"}, "body": {"type": "int"}}]
vec_indexes_on = ["c1", "c2"]
fulltext_indexes_on = ["doctitle", "docdate"]
vector_index_type = [index.IndexType.IVFFlat, index.IndexType.Hnsw]
vector_index_option = [[index.InitParameter("centroids_count", "128"), index.InitParameter("metric", "l2")],
                       [
                           index.InitParameter("M", "16"),
                           index.InitParameter("ef_construction", "50"),
                           index.InitParameter("ef", "50"),
                           index.InitParameter("metric", "l2")
                       ]]


class TestDDLParallel:
    def test_dll_parallel(self, get_infinity_connection_pool):
        connection_pool = get_infinity_connection_pool
        threads = []
        end_time = time.time() + kRunningTime
        for i in range(kNumThread):
            threads.append(Thread(target=random_exec, args=[connection_pool, end_time, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        infinity_obj = connection_pool.get_conn()
        infinity_obj.get_database("default_db")
        databases = infinity_obj.list_databases().db_names
        print(databases)
        for db_name in databases:
            if db_name != "default_db" and db_name.startswith('dll_parallel'):
                infinity_obj.drop_database(db_name, conflict_type=ConflictType.Ignore)
        print(infinity_obj.list_databases().db_names)


def random_exec(connection_pool: ConnectionPool, end_time, thread_id):
    infinity_obj = connection_pool.get_conn()
    while time.time() < end_time:
        rand_v = random.randint(0, 99)
        if 0 <= rand_v < 1:
            create_database(infinity_obj)
        elif 1 <= rand_v < 2:
            drop_database(infinity_obj)
        elif 2 <= rand_v < 6:
            create_table(infinity_obj)
        elif 6 <= rand_v < 10:
            drop_table(infinity_obj)
        elif 10 <= rand_v < 55:
            create_index(infinity_obj)
        else:
            drop_index(infinity_obj)
        time.sleep(0.2)
    connection_pool.release_conn(infinity_obj)


def create_database(infinity_obj: RemoteThriftInfinityConnection):
    db_name = db_names[random.randint(0, len(db_names) - 1)]
    infinity_obj.create_database(db_name, conflict_type=ConflictType.Ignore)
    res = infinity_obj.show_database()
    print(res)


def drop_database(infinity_obj: RemoteThriftInfinityConnection):
    try:
        db_name = db_names[random.randint(0, len(db_names) - 1)]
        infinity_obj.drop_database(db_name, conflict_type=ConflictType.Ignore)
        res = infinity_obj.show_database()
        print(res)
    except Exception as e:
        return


def create_table(infinity_obj: RemoteThriftInfinityConnection):
    db_name = db_names[random.randint(0, len(db_names) - 1)]
    try:
        db_obj = infinity_obj.get_database(db_name)
        table_name = table_names[random.randint(0, len(table_names) - 1)]
        if (table_name[0:3] == "vec"):
            column = vec_columns[random.randint(0, len(vec_columns) - 1)]
        else:
            column = fulltext_columns[random.randint(0, len(fulltext_columns) - 1)]
        db_obj.create_table(table_name, column, conflict_type=ConflictType.Ignore)
        res = db_obj.list_tables()
        print(res)
    except Exception as e:
        return


def drop_table(infinity_obj: RemoteThriftInfinityConnection):
    db_name = db_names[random.randint(0, len(db_names) - 1)]
    try:
        db_obj = infinity_obj.get_database(db_name)
        exist_tables = db_obj.list_tables().table_names
    except Exception as e:
        return
    if (len(exist_tables) > 0):
        table_name = exist_tables[random.randint(0, len(exist_tables) - 1)]
        db_obj.drop_table(table_name, ConflictType.Ignore)
        res = db_obj.list_tables()
        print(res)


def create_index(infinity_obj: RemoteThriftInfinityConnection):
    db_name = db_names[random.randint(0, len(db_names) - 1)]
    try:
        db_obj = infinity_obj.get_database(db_name)
        exist_tables = db_obj.list_tables().table_names
    except Exception as e:
        return
    if (len(exist_tables) > 0):
        table_name = exist_tables[random.randint(0, len(exist_tables) - 1)]
        try:
            table_obj = db_obj.get_table(table_name)
            if table_name[0:3] == "vec":
                index_on = vec_indexes_on[random.randint(0, len(vec_indexes_on) - 1)]
                index_type_choice = random.randint(0, len(vector_index_type) - 1)
                index_type = vector_index_type[index_type_choice]
                index_option = vector_index_option[index_type_choice]
            else:
                index_on = fulltext_indexes_on[random.randint(0, len(fulltext_indexes_on) - 1)]
                index_type = index.IndexType.FullText
                index_option = None
            table_obj.create_index(index_on, index.IndexInfo(index_on, index_type, index_option),
                                   conflict_type=ConflictType.Ignore)
            res = table_obj.list_indexes()
            print(res)
        except Exception as e:
            pass


def drop_index(infinity_obj: RemoteThriftInfinityConnection):
    db_name = db_names[random.randint(0, len(db_names) - 1)]
    try:
        db_obj = infinity_obj.get_database(db_name)
        exist_tables = db_obj.list_tables().table_names
    except Exception as e:
        return
    if (len(exist_tables) > 0):
        table_name = exist_tables[random.randint(0, len(exist_tables) - 1)]
        try:
            table_obj = db_obj.get_table(table_name)
            exist_indexes = table_obj.list_indexes().index_names
            if (len(exist_indexes) > 0):
                index_name = exist_indexes[random.randint(0, len(exist_indexes) - 1)]
                table_obj.drop_index(index_name, conflict_type=ConflictType.Ignore)
                res = table_obj.list_indexes()
                print(res)
        except Exception as e:
            pass
