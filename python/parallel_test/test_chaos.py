import os
import time

import infinity.index as index
import pandas
import pytest
import random
from threading import Thread
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool
from infinity.table import Table


TEST_DATA_DIR = "/test/data/"
fulltext_file_path = os.getcwd() + TEST_DATA_DIR + "csv/enwiki_99.csv"
vector_file_path = os.getcwd() + TEST_DATA_DIR + "csv/pysdk_test_knn.csv"
kRunningTime = 30
kNumThread = 4
data_size = 100000
insert_delete_size = 100

class TestIndexParallel:
    
    @pytest.mark.skip(reason="No such chunk in heap when delete, and update vector fail due to 'Not support to convert Embedding to Embedding'")
    def test_chaos(self, get_infinity_connection_pool):
        data = read_out_data()
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default")

        res = db_obj.drop_table("chaos_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("chaos_test", {
            "index":"int", "body": "varchar", "other_vector": "vector,4,float"}, ConflictType.Error)
        res = table_obj.create_index("body_index",
                                    [index.IndexInfo("body",
                                                    index.IndexType.FullText,
                                                    [])])
        assert res.error_code == ErrorCode.OK
        res = table_obj.create_index("other_index",
                                    [index.IndexInfo("other_vector", index.IndexType.Hnsw,
                                        [index.InitParameter("M", "16"), index.InitParameter("ef_construction", "50"),
                                        index.InitParameter("ef", "50"), index.InitParameter("metric", "l2")
                                        ])], ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)

        threads = []
        end_time = time.time()+kRunningTime
        for i in range(kNumThread):
            threads.append(Thread(target=random_exec, args=[connection_pool, data, end_time, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()
        
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("chaos_test", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)
    
def read_out_data():
    column_names = ["doctitle", "docdate", "body"]
    df_fulltext = pandas.read_csv(fulltext_file_path, delimiter="\t", header=None, names=column_names)["body"]
    df_vector = pandas.DataFrame({"other_vector" : [[0.0, 0.0, 0.0, 0.0], [1.1, 1.1, 1.1, 1.1], [2.2, 2.2, 2.2, 2.2], 
                                                    [3.3, 3.3, 3.3, 3.3], [4.4, 4.4, 4.4, 4.4], [5.5, 5.5, 5.5, 5.5], 
                                                    [6.6, 6.6, 6.6, 6.6], [7.7, 7.7, 7.7, 7.7], [8.8, 8.8, 8.8, 8.8],
                                                    [9.9, 9.9, 9.9, 9.9]]})
    print(df_vector)
    df_fulltext = pandas.concat([df_fulltext] * int(data_size/10), ignore_index=True)
    df_vector = pandas.concat([df_vector] * int(data_size/10), ignore_index=True)
    df_fulltext = df_fulltext.reset_index()
    df_vector = df_vector.reset_index()
    df = pandas.merge(df_vector, df_fulltext, on='index')
    df["body"] = df["body"].astype(str)
    print(df.dtypes)
    data = {key: list(value.values())
             for key, value in df.to_dict().items()}
    return data


def search_fulltext(table_obj:Table):
    res = table_obj.output(["index", "body", "other_vector", "_row_id", "_score"]).match(
    "body^5", "harmful chemical", "topn=3").to_pl()
    print(res)

def search_vector(table_obj:Table):
    res = table_obj.output(["*"]).knn("other_vector", [2] * 4, "float", "l2", 3).to_pl()
    print(res)

def insert(table_obj:Table, data):
    try:
        pos = random.randint(0, int(data_size/insert_delete_size) -1)
        value = []
        for i in range(insert_delete_size):
            value.append({"index": data["index"][i+pos],"body": data["body"][i+pos], "other_vector":data["other_vector"][i+pos]})
        table_obj.insert(value)
    except Exception as e:
        print(e)

def delete(table_obj:Table):
    pos = random.randint(0, int(data_size/insert_delete_size) -1)
    try:
        table_obj.delete(f"index >= {pos} and index < {pos + insert_delete_size}")
    except Exception as e:
        print(e)

def updata(table_obj:Table):
    pos = random.randint(0, data_size-1)
    try:
        table_obj.update(f"index = {pos}", [{"index":pos, "body":"infinity", "other_vector":[0.0, 0.0, 0.0, 0.0]}])
    except Exception as e:
        print(e)
        

def random_exec(connection_pool:ConnectionPool, data, end_time, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default")
    table_obj = db_obj.get_table("chaos_test")
    while time.time() < end_time:
        rand_v = random.randint(0, 4)
        if rand_v == 0:
            print(thread_id, "insert")
            insert(table_obj, data)
        elif rand_v == 1:
            print(thread_id, "delete")
            delete(table_obj)
        elif rand_v == 2:
            print(thread_id, "update")
            updata(table_obj)
        elif rand_v == 3:
            print(thread_id, "search fulltext")
            search_fulltext(table_obj)
        else:
            print(thread_id, "search vector")
            search_vector(table_obj)
    connection_pool.release_conn(infinity_obj)
