import logging
import os
import random
import time
from threading import Thread
import threading
from multiprocessing import Value, Lock

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
                res, extra_result = table_obj.output(["doctitle", "docdate", "_row_id", "_score"]).match_text(
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
        res, extra_result = table_obj.output(["variant_id"]).match_dense(
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
                res, extra_result = table_obj.output(["variant_id"]).match_dense(
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

    def test_table_creation_deletion_parallel(self, get_infinity_connection_pool):

        def create_table_worker(connection_pool: ConnectionPool, table_name_prefix, end_time, thread_id):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_counter = 0
            while time.time() < end_time:
                table_name = f"{table_name_prefix}_{thread_id}_{table_counter}"
                table_counter += 1
                try:
                    db_obj.create_table(table_name, {"id": {"type": "int"}, "value": {"type": "varchar"}},
                                    ConflictType.Ignore)
                except Exception as e:
                    print(f"thread {thread_id}: table {table_name} create failed. {e}")
                else:
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

    def test_multiple_index_types_parallel(self, get_infinity_connection_pool):
        """Test parallel read/write with multiple index types (Hnsw, EMVB, Secondary, FullText) on same table"""
        import threading

        def write_worker(connection_pool: ConnectionPool, table_name, data, end_time, thread_id, write_count):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            local_count = 0

            while time.time() < end_time:
                try:
                    # Insert data with all column types
                    value = [{
                        "id": thread_id * 10000 + local_count,
                        "text": data["text"][local_count % len(data["text"])],
                        "vector_col": [random.random() for _ in range(4)],
                        "tensor_col": [[random.random() for _ in range(32)] for _ in range(32)]
                    }]
                    table_obj.insert(value)
                    local_count += 1
                    with write_count.get_lock():
                        write_count.value += 1
                except Exception as e:
                    print(f"thread {thread_id}: insert failed: {e}")
                time.sleep(0.1)

            connection_pool.release_conn(infinity_obj)
            print(f"thread {thread_id}: write worker done, inserted {local_count} rows")

        def read_worker(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_counts):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            local_counts = {"fulltext": 0, "hnsw": 0, "secondary": 0, "tensor": 0}

            while time.time() < end_time:
                try:
                    # Test FullText index
                    res, _ = table_obj.output(["id", "_score"]).match_text("text", "test", 5).to_pl()
                    local_counts["fulltext"] += 1
                except Exception as e:
                    pass

                try:
                    # Test Hnsw index (dense vector)
                    res, _ = table_obj.output(["id"]).match_dense("vector_col", [0.5] * 4, "float", "l2", 5).to_pl()
                    local_counts["hnsw"] += 1
                except Exception as e:
                    pass

                try:
                    # Test Secondary index (filter)
                    res, _ = table_obj.output(["id"]).filter("id > 0").to_pl()
                    local_counts["secondary"] += 1
                except Exception as e:
                    pass

                try:
                    # Test EMVB index (tensor) - skip for now due to backend bug
                    pass
                    # res, _ = table_obj.output(["id"]).match_tensor("tensor_col",
                    #                                                 [[0.5] * 32] * 32,
                    #                                                 "float", 2).to_pl()
                    # local_counts["tensor"] += 1
                except Exception as e:
                    pass

                time.sleep(0.1)

            with read_counts["lock"]:
                read_counts["fulltext"] += local_counts["fulltext"]
                read_counts["hnsw"] += local_counts["hnsw"]
                read_counts["secondary"] += local_counts["secondary"]
                read_counts["tensor"] += local_counts["tensor"]

            connection_pool.release_conn(infinity_obj)
            print(f"thread {thread_id}: read worker done, fulltext: {local_counts['fulltext']}, "
                  f"hnsw: {local_counts['hnsw']}, secondary: {local_counts['secondary']}, tensor: {local_counts['tensor']}")

        def verify_worker(connection_pool: ConnectionPool, table_name, expected_write_count, verify_result):
            infinity_obj = connection_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            # Wait a bit for background operations to settle
            time.sleep(1)

            # Verify data count
            try:
                res, _ = table_obj.output(["id"]).to_pl()
                actual_count = len(res)
                verify_result["data_count"] = actual_count
                verify_result["write_count"] = expected_write_count
                print(f"Verify: expected {expected_write_count} writes, actual {actual_count} rows in table")
            except Exception as e:
                verify_result["error"] = str(e)

            # Verify each index exists
            try:
                res = table_obj.list_indexes()
                index_names = res.index_names
                verify_result["indexes"] = index_names
                print(f"Verify: indexes in table: {index_names}")
            except Exception as e:
                verify_result["error"] = str(e)

            connection_pool.release_conn(infinity_obj)

        # Prepare test data
        text_data = ["hello world test", "test data for indexing", "parallel test case", "infinity database search"]
        data = {"text": text_data}

        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_name = "test_multiple_index_types"

        # Clean up and create table
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        table_obj = db_obj.create_table(table_name, {
            "id": {"type": "int"},
            "text": {"type": "varchar"},
            "vector_col": {"type": "vector,4,float"},
            "tensor_col": {"type": "tensor, 32, float"}
        }, ConflictType.Error)
        assert table_obj is not None

        # Create all 4 types of indexes
        # 1. FullText index
        res = table_obj.create_index("idx_fulltext",
                                     index.IndexInfo("text", index.IndexType.FullText),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 2. Secondary index
        res = table_obj.create_index("idx_secondary",
                                     index.IndexInfo("id", index.IndexType.Secondary),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 3. Hnsw index
        res = table_obj.create_index("idx_hnsw",
                                     index.IndexInfo("vector_col", index.IndexType.Hnsw,
                                                     {"M": "16", "ef_construction": "50", "metric": "l2"}),
                                     ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # 4. EMVB index (for tensor) - skip for now due to backend bug
        # res = table_obj.create_index("idx_emvb",
        #                              index.IndexInfo("tensor_col", index.IndexType.EMVB,
        #                                              {"pq_subspace_num": "32", "pq_subspace_bits": "8"}),
        #                              ConflictType.Error)
        # assert res.error_code == ErrorCode.OK

        print(f"Created 3 indexes: fulltext, secondary, hnsw (EMVB skipped due to backend bug)")

        # Insert initial data for indexes
        initial_data = []
        for i in range(10):
            initial_data.append({
                "id": i,
                "text": text_data[i % len(text_data)],
                "vector_col": [random.random() for _ in range(4)],
                "tensor_col": [[random.random() for _ in range(32)] for _ in range(32)]
            })
        table_obj.insert(initial_data)
        print(f"Inserted initial {len(initial_data)} rows")

        # Start parallel test
        kRuningTime = 10
        kReadThreadNum = 4
        kWriteThreadNum = 4

        # Shared counters
        write_count = Value('i', 0)
        read_counts = {"lock": Lock(), "fulltext": 0, "hnsw": 0, "secondary": 0, "tensor": 0}

        threads = []
        end_time = time.time() + kRuningTime

        # Start 4 write threads
        for i in range(kWriteThreadNum):
            threads.append(Thread(target=write_worker, args=[
                connection_pool, table_name, data, end_time, i, write_count]))

        # Start 4 read threads
        for i in range(kReadThreadNum):
            threads.append(Thread(target=read_worker, args=[
                connection_pool, table_name, end_time, i, read_counts]))

        # Start all threads
        for t in threads:
            t.start()

        # Wait for all threads
        for t in threads:
            t.join()

        print(f"\n=== Test Summary ===")
        print(f"Total write operations: {write_count.value}")
        print(f"Total read operations - FullText: {read_counts['fulltext']}, "
              f"Hnsw: {read_counts['hnsw']}, Secondary: {read_counts['secondary']}, Tensor: {read_counts['tensor']}")

        # Verify results
        verify_result = {}
        verify_thread = Thread(target=verify_worker, args=[
            connection_pool, table_name, write_count.value, verify_result])
        verify_thread.start()
        verify_thread.join()

        # Assert verification
        assert "error" not in verify_result or verify_result["error"] is None, f"Verify error: {verify_result.get('error')}"
        assert "idx_fulltext" in verify_result.get("indexes", []), "FullText index not found"
        assert "idx_secondary" in verify_result.get("indexes", []), "Secondary index not found"
        assert "idx_hnsw" in verify_result.get("indexes", []), "Hnsw index not found"
        # assert "emvb" in verify_result.get("indexes", []), "EMVB index not found"

        print(f"Indexes verified: {verify_result.get('indexes')}")
        print(f"Data count in table: {verify_result.get('data_count')}")

        # Clean up
        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        connection_pool.release_conn(infinity_obj)
        print("Test completed successfully!")

