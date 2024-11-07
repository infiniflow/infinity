import logging
import os
import random
import time
from threading import Thread

import infinity.index as index
import pandas
import pytest
from infinity.common import ConflictType
from infinity.connection_pool import ConnectionPool
from infinity.errors import ErrorCode

from test_sdkbase import TestSdk
from util import RtnThread

TEST_DATA_DIR = "/test/data/"
kInsertThreadNum = 4
kRuningTime = 10
kThreadNum = 4


class TestIndexParallel(TestSdk):

    @pytest.mark.parametrize("file_format", ["csv"])
    def test_fulltext_index_rw_parallel(self, get_infinity_connection_pool, file_format):

        def write_worker(connection_pool: ConnectionPool, data, file_path, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_fulltext_index_parallel")

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

        def read_worker(connection_pool: ConnectionPool, end_time):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_fulltext_index_parallel")

            while time.time() < end_time:
                res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match_text(
                    "body^5", "harmful chemical", 3).to_pl()
                print(res)
                time.sleep(0.1)

            connection_pool.release_conn(infinity_obj)

        # prepare data for insert
        column_names = ["doctitle", "docdate", "body"]
        file_path = os.getcwd() + TEST_DATA_DIR + file_format + \
                    "/enwiki_99." + file_format
        df = pandas.read_csv(file_path,
                             delimiter="\t",
                             header=None,
                             names=column_names)
        data = {key: list(value.values())
                for key, value in df.to_dict().items()}

        # create index
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_fulltext_index_parallel", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("test_fulltext_index_parallel", {
            "doctitle": {"type": "varchar"},
            "docdate": {"type": "varchar"}, "body": {"type": "varchar"}}, ConflictType.Error)
        table_obj.import_data(file_path, {"delimiter": "\t"})
        res = table_obj.create_index("body_index",
                                     index.IndexInfo("body",
                                                     index.IndexType.FullText))
        assert res.error_code == ErrorCode.OK

        threads = []
        end_time = time.time() + kRuningTime
        for i in range(kInsertThreadNum):
            threads.append(Thread(target=write_worker, args=[
                connection_pool, data, file_path, end_time, i]))
        threads.append(Thread(target=read_worker, args=[
            connection_pool, end_time]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        res = db_obj.drop_table(
            "test_fulltext_index_parallel", ConflictType.Error)
        connection_pool.release_conn(infinity_obj)

    @pytest.mark.parametrize("index_type", [index.IndexType.Hnsw])
    @pytest.mark.parametrize("index_column_name", ["gender_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2"])
    @pytest.mark.parametrize("knn_distance_type", ["l2"])
    @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("running_time", [30])
    def test_vector_index_single_thread(self, get_infinity_connection_pool, index_type, index_column_name,
                                        knn_column_name,
                                        index_distance_type, knn_distance_type, file_format, running_time):
        file_path = os.getcwd() + TEST_DATA_DIR + file_format + \
                    "/pysdk_test_knn." + file_format

        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_vector_index_parallel", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_vector_index_parallel", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        res = table_obj.create_index("my_index",
                                     index.IndexInfo(index_column_name,
                                                     index_type,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": index_distance_type
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        end_time = time.time() + running_time
        while time.time() < end_time:
            print("begin import")
            table_obj.import_data(file_path)
            print("import complete")
        res = table_obj.output(["variant_id"]).match_dense(
            knn_column_name, [1] * 4, "float", knn_distance_type, 5).to_pl()
        print(res)

        res = db_obj.drop_table(
            "test_vector_index_parallel", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)

    # @pytest.mark.parametrize("index_column_name", ["gender_vector",
    #                                                "color_vector",
    #                                                "category_vector",
    #                                                "tag_vector",
    #                                                "other_vector"])
    # @pytest.mark.parametrize("knn_column_name", ["gender_vector",
    #                                              "color_vector",
    #                                              "category_vector",
    #                                              "tag_vector",
    #                                              "other_vector"])
    # @pytest.mark.parametrize("index_distance_type", ["l2", "ip"])
    # @pytest.mark.parametrize("knn_distance_type", ["l2", "ip"])
    # @pytest.mark.parametrize("file_format", ["csv"])
    @pytest.mark.parametrize("index_type", [index.IndexType.Hnsw])
    @pytest.mark.parametrize("index_column_name", ["gender_vector"])
    @pytest.mark.parametrize("knn_column_name", ["gender_vector"])
    @pytest.mark.parametrize("index_distance_type", ["l2"])
    @pytest.mark.parametrize("knn_distance_type", ["l2"])
    @pytest.mark.parametrize("file_format", ["csv"])
    def test_vector_index_rw_parallel(self, get_infinity_connection_pool, index_type, index_column_name,
                                      knn_column_name,
                                      index_distance_type, knn_distance_type, file_format):

        def write_worker(connection_pool: ConnectionPool, file_path, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_vector_index_parallel")

            while time.time() < end_time:
                print(f"thread {thread_id}: begin import")
                table_obj.import_data(file_path)
                print(f"thread {thread_id}: import complete")

            connection_pool.release_conn(infinity_obj)

        def read_worker(connection_pool: ConnectionPool, end_time, knn_column_name, knn_distance_type):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_vector_index_parallel")

            while time.time() < end_time:
                res = table_obj.output(["variant_id"]).match_dense(
                    knn_column_name, [1] * 4, "float", knn_distance_type, 5).to_pl()
                print(res)
                time.sleep(0.1)

            connection_pool.release_conn(infinity_obj)

        file_path = os.getcwd() + TEST_DATA_DIR + file_format + \
                    "/pysdk_test_knn." + file_format

        # create index
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table(
            "test_vector_index_parallel", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table("test_vector_index_parallel", {
            "variant_id": {"type": "varchar"},
            "gender_vector": {"type": "vector,4,float"},
            "color_vector": {"type": "vector,4,float"},
            "category_vector": {"type": "vector,4,float"},
            "tag_vector": {"type": "vector,4,float"},
            "other_vector": {"type": "vector,4,float"},
            "query_is_recommend": {"type": "varchar"},
            "query_gender": {"type": "varchar"},
            "query_color": {"type": "varchar"},
            "query_price": {"type": "float"}
        }, ConflictType.Error)
        table_obj.import_data(file_path, None)
        res = table_obj.create_index("my_index",
                                     index.IndexInfo(index_column_name,
                                                     index_type,
                                                     {
                                                         "M": "16",
                                                         "ef_construction": "50",
                                                         "metric": index_distance_type
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        kInsertThreadNum = 4
        kRuningTime = 10
        threads = []
        end_time = time.time() + kRuningTime
        for i in range(kInsertThreadNum):
            threads.append(Thread(target=write_worker, args=[
                connection_pool, file_path, end_time, i]))
        threads.append(Thread(target=read_worker, args=[
            connection_pool, end_time, knn_column_name, knn_distance_type]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()

        res = db_obj.drop_table(
            "test_vector_index_parallel", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)

    def test_index_creation_deletion_parallel(self, get_infinity_connection_pool):
        self.logger = logging.getLogger("run_parallel_test")

        def index_worker(connection_pool: ConnectionPool, table_name, column_name, index_name, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            while time.time() < end_time:
                res = table_obj.create_index(index_name,
                                             index.IndexInfo(column_name,
                                                             index.IndexType.FullText),
                                             ConflictType.Ignore)
                if res.error_code == ErrorCode.OK:
                    self.logger.info(f"thread {thread_id}: index {index_name} created")
                else:
                    self.logger.info(f"thread {thread_id}: create_index {index_name} failed: {res.error_msg}")
                time.sleep(0.5)
                res = table_obj.drop_index(index_name, ConflictType.Ignore)
                if res.error_code == ErrorCode.OK:
                    self.logger.info(f"thread {thread_id}: index {index_name} deleted")
                else:
                    self.logger.info(f"thread {thread_id}: delete_index {index_name} failed: {res.error_msg}")
                time.sleep(0.5)

            connection_pool.release_conn(infinity_obj)

        def insert_worker(connection_pool: ConnectionPool, table_name, data, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            while time.time() < end_time:
                value = []
                for i in range(len(data["doctitle"])):
                    value.append({"doctitle": data["doctitle"][i],
                                  "docdate": data["docdate"][i], "body": data["body"][i]})
                table_obj.insert(value)
                self.logger.info(f"thread {thread_id}: put data")
                time.sleep(0.1)

            connection_pool.release_conn(infinity_obj)

        def query_worker(connection_pool: ConnectionPool, table_name, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            while time.time() < end_time:
                try:
                    res = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match_text(
                        "body^5", "harmful chemical", 3).to_pl()
                    self.logger.info(f"thread {thread_id}: check result:\n{res}")
                except Exception as e:
                    self.logger.info(f"thread {thread_id}: check failed: {e}")
                time.sleep(0.5)

            connection_pool.release_conn(infinity_obj)

        column_names = ["doctitle", "docdate", "body"]
        file_path = os.getcwd() + TEST_DATA_DIR + "csv" + "/enwiki_99.csv"
        df = pandas.read_csv(file_path,
                             delimiter="\t",
                             header=None,
                             names=column_names)
        data = {key: list(value.values())
                for key, value in df.to_dict().items()}

        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_name = "test_index_creation_deletion_parallel"
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table(table_name, {
            "doctitle": {"type": "varchar"},
            "docdate": {"type": "varchar"},
            "body": {"type": "varchar"}}, ConflictType.Error)
        assert table_obj is not None

        threads = []
        end_time = time.time() + kRuningTime

        for i in range(kThreadNum):
            threads.append(RtnThread(target=index_worker, args=[
                connection_pool, table_name, "body", "body_index", end_time, i]))

        for i in range(kThreadNum):
            threads.append(RtnThread(target=insert_worker, args=[
                connection_pool, table_name, data, end_time, i + kThreadNum]))

        for i in range(kThreadNum):
            threads.append(RtnThread(target=query_worker, args=[
                connection_pool, table_name, end_time, i + 2 * kThreadNum]))

        try:
            self.logger.info("test_index_creation_deletion_parallel start")
            for t in threads:
                t.start()

            for t in threads:
                t.join()
        except Exception as e:
            self.logger.error(f"test_index_creation_deletion_parallel failed: {e}")
            raise e
        else:
            self.logger.info("test_index_creation_deletion_parallel end")

        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)

    def test_table_creation_deletion_parallel(self, get_infinity_connection_pool):

        def create_table_worker(connection_pool: ConnectionPool, table_name_prefix, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_counter = 0
            while time.time() < end_time:
                table_name = f"{table_name_prefix}_{thread_id}_{table_counter}"
                table_counter += 1
                db_obj.create_table(table_name, {"id": {"type": "int"}, "value": {"type": "varchar"}},
                                    ConflictType.Ignore)
                print(f"thread {thread_id}: table {table_name} created")
                time.sleep(0.5)
                res = db_obj.drop_table(table_name, ConflictType.Ignore)
                if res.error_code == ErrorCode.OK:
                    print(f"thread {thread_id}: table {table_name} deleted")
                else:
                    print(f"thread {thread_id}: delete table {table_name} failed: {res.error_msg}")
                time.sleep(0.5)
            connection_pool.release_conn(infinity_obj)

        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        table_name_prefix = "test_table_creation_deletion"

        threads = []
        end_time = time.time() + kRuningTime
        for i in range(kThreadNum):
            threads.append(Thread(target=create_table_worker, args=[
                connection_pool, table_name_prefix, end_time, i]))
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        connection_pool.release_conn(infinity_obj)
