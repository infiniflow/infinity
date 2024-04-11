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

TEST_DATA_DIR = "/test/data/"


class TestIndexParallel:

    @pytest.mark.xfail(reason="AddressSanitizer: heap-use-after-free & deadlock when import parallelly")
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_insert_data_fulltext_index_search_parallel(self, get_infinity_connection_pool, file_format):

        def insert_worker(connection_pool:ConnectionPool, data, file_path, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            print(infinity_obj)
            db_obj = infinity_obj.get_database("default")
            table_obj = db_obj.get_table("test_insert_data_fulltext_index_search")

            while time.time() < end_time:
                operation = random.randint(0, 1)
                if operation == 0:
                    value = []
                    for i in range(len(data["doctitle"])):
                        value.append({"doctitle": data["doctitle"][i],
                                    "docdate": data["docdate"][i], "body": data["body"][i]})
                    table_obj.insert(value)
                    print(f"thread {thread_id}: insert complete")
                if operation == 1:
                    print(f"thread {thread_id}: begin import")
                    table_obj.import_data(file_path, {"delimiter": "\t"})
                    print(f"thread {thread_id}: import complete")
            
            connection_pool.release_conn(infinity_obj)

        def search_worker(connection_pool:ConnectionPool, end_time):
            time.sleep(5)
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default")
            table_obj = db_obj.get_table("test_insert_data_fulltext_index_search")
            
            while time.time() < end_time:
                res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match(
                "body^5", "harmful chemical", "topn=3").to_pl()
                print(res)
                time.sleep(0.1)
            
            connection_pool.release_conn(infinity_obj)


        # prepare data for insert
        column_names = ["doctitle", "docdate", "body"]
        file_path = os.getcwd() + TEST_DATA_DIR + file_format + "/enwiki_99." + file_format
        df = pandas.read_csv(file_path,
                             delimiter="\t",
                             header=None,
                             names=column_names)
        data = {key: list(value.values())
                for key, value in df.to_dict().items()}

        # create index
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default")
        res = db_obj.drop_table("test_insert_data_fulltext_index_search", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_insert_data_fulltext_index_search", {
            "doctitle": "varchar",
            "docdate": "varchar", "body": "varchar"}, ConflictType.Error)
        res = table_obj.create_index("body_index",
                                     [index.IndexInfo("body",
                                                      index.IndexType.FullText,
                                                      [])])
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)


        kInsertThreadNum = 4
        kRuningTime = 10
        threads = []
        status = [True]
        end_time = time.time()+kRuningTime
        for i in range(kInsertThreadNum):
            threads.append(Thread(target=insert_worker, args=[connection_pool, data, file_path, end_time, i]))
        threads.append(Thread(target=search_worker, args=[connection_pool, end_time]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()
        assert status[0]
        
