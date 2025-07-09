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
kInsertThreadNum = 8
kRuningTime = 60 * 60 * 10
kThreadNum = 8


class TestIndexParallel(TestSdk):
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
                    # print(f"thread {thread_id}: index {index_name} created")
                    self.logger.info(f"thread {thread_id}: index {index_name} created")
                else:
                    # print(f"thread {thread_id}: create_index {index_name} failed: {res.error_msg}")
                    self.logger.info(f"thread {thread_id}: create_index {index_name} failed: {res.error_msg}")
                time.sleep(0.5)
                res = table_obj.drop_index(index_name, ConflictType.Ignore)
                if res.error_code == ErrorCode.OK:
                    # print(f"thread {thread_id}: index {index_name} deleted")
                    self.logger.info(f"thread {thread_id}: index {index_name} deleted")
                else:
                    # print(f"thread {thread_id}: delete_index {index_name} failed: {res.error_msg}")
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
                # print(f"thread {thread_id}: put data")
                self.logger.info(f"thread {thread_id}: put data")
                time.sleep(1)

            connection_pool.release_conn(infinity_obj)

        def query_worker(connection_pool: ConnectionPool, table_name, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            while time.time() < end_time:
                try:
                    res, extra_result = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match_text(
                        "body^5", "harmful chemical", 3).to_pl()
                    # print(f"thread {thread_id}: check result:\n{res}")
                    self.logger.info(f"thread {thread_id}: check result:\n{res}")
                except Exception as e:
                    # print(f"thread {thread_id}: check failed: {e}")
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
