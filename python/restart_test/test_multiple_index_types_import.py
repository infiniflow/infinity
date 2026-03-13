import os
import random
import pytest
import logging
import time
import numpy as np
from threading import Thread
from multiprocessing import Value
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory, infinity_runner_decorator_factory2
from restart_util import MultiIndexTypesGenerator
from common import common_values
from infinity.common import ConflictType, SparseVector
from infinity import index
from infinity.connection_pool import ConnectionPool

# Test configuration constants
K_RUNNING_TIME_SECONDS = 20


class TestMultipleIndexTypesImport:
    """Test multiple index types with import and restart scenarios"""

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "total_n, config",
        [
            (MultiIndexTypesGenerator.import_size(), "test/data/config/restart_test/test_insert/5.toml"),
        ],
    )
    @pytest.mark.parametrize(
        "columns, indexes, import_file, import_size, import_options",
        [
            (
                MultiIndexTypesGenerator.columns(),
                MultiIndexTypesGenerator.index(),
                MultiIndexTypesGenerator.import_file(),
                MultiIndexTypesGenerator.import_size(),
                {"file_type": "csv", "delimiter": "\t"},
            ),
        ],
    )
    def test_multiple_index_types_import_restart(
        self,
        infinity_runner: InfinityRunner,
        total_n: int,
        config: str,
        columns: dict,
        indexes: list[index.IndexInfo],
        import_file: str,
        import_size: int,
        import_options: dict,
    ):
        """Test import data with multiple indexes and verify after restart with continuous insert + query"""
        uri = common_values.TEST_LOCAL_HOST
        table_name = "test_multi_index_types"

        infinity_runner.clear()

        # Ensure CSV file exists (auto-generated if not exists)
        import_file = MultiIndexTypesGenerator.import_file()
        logging.info(f"Using CSV file: {import_file}")

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        # Part 1: Create table and indexes
        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)

            table_obj = db_obj.create_table(table_name, columns, ConflictType.Error)
            assert table_obj is not None

            for idx in indexes:
                idx_name = f"idx_{idx.target_name}"
                logging.info(f"Creating index {idx_name}...")
                table_obj.create_index(idx_name, idx)

            logging.info(f"Created table and {len(indexes)} indexes")

        part1()

        # Part 2: Import data
        @decorator
        def part2(infinity_obj):
            kImportRepeat = 10
            kBatchCount = 20
            kRowsPerBatch = 5000

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            abs_import_file = os.path.abspath(import_file)

            for import_round in range(kImportRepeat):
                import_start = time.time()
                logging.info(f"Importing data from {abs_import_file}... (round {import_round + 1}/{kImportRepeat})")
                table_obj.import_data(abs_import_file, import_options)
                import_duration = time.time() - import_start
                logging.info(f"Import round {import_round + 1} completed in {import_duration:.2f} seconds")

            # Verify row count after all imports
            expected_count = total_n * kImportRepeat
            res, _, _ = table_obj.output(["count(*)"]).to_result()
            count = res["count(star)"][0]
            logging.info(f"Total rows after {kImportRepeat} imports: {count}")
            assert count == expected_count, f"Expected {expected_count} rows, got {count}"

            # Insert 10 batches, each with 5000 rows
            categories = ["A", "B", "C", "D"]
            text_words = ["apple", "banana", "cherry", "date"]

            for batch_idx in range(kBatchCount):
                batch_data = []
                for row_idx in range(kRowsPerBatch):
                    row_id = batch_idx * kRowsPerBatch + row_idx
                    vec = [random.random() for _ in range(2048)]
                    multivec = [[random.random() for _ in range(1024)] for _ in range(2)]
                    sparse_indices = [j for j in range(1024) if random.random() > 0.9]
                    if not sparse_indices:
                        sparse_indices = [0, 1, 2]
                    sparse_values = [random.randint(1, 100) for _ in range(len(sparse_indices))]
                    sparse_vec = SparseVector(indices=sparse_indices, values=sparse_values)

                    batch_data.append({
                        "doctitle": f"test_title_{row_id}",
                        "docdate": "01-JAN-2024 00:00:00.000",
                        "body": f"test_text_{row_id}_{random.choice(text_words)}",
                        "num": row_id,
                        "category": categories[row_idx % len(categories)],
                        "vector_col": vec,
                        "multi_vector_col": multivec,
                        "sparse_col": sparse_vec
                    })

                insert_start = time.time()
                table_obj.insert(batch_data)
                insert_duration = time.time() - insert_start
                logging.info(f"Inserted batch {batch_idx + 1}/{kBatchCount} ({kRowsPerBatch} rows) in {insert_duration:.2f} seconds")

            # Verify row count after insert
            res, _, _ = table_obj.output(["count(*)"]).to_result()
            count = res["count(star)"][0]
            # Previous import added total_n * kImportRepeat rows, now add kBatchCount * kRowsPerBatch rows
            expected_count = total_n * kImportRepeat + kBatchCount * kRowsPerBatch
            logging.info(f"Total rows after batch insert: {count}, expected: {expected_count}")
            assert count == expected_count, f"Expected {expected_count} rows after batch insert, got {count}"

        part2()

        # Part 3: Restart 5 times - each round runs 120s with continuous write/read
        for round_num in range(5):
            insert_count = Value('i', 0)
            read_count_fulltext = Value('i', 0)
            read_count_hnsw = Value('i', 0)
            read_count_hnsw_mv = Value('i', 0)
            read_count_secondary_high = Value('i', 0)
            read_count_secondary_low = Value('i', 0)
            read_count_sparse = Value('i', 0)
            read_count_fusion_rrf = Value('i', 0)
            read_count_fusion_mv_rrf = Value('i', 0)
            read_count_fusion_weighted_sum = Value('i', 0)
            update_count = Value('i', 0)
            delete_count = Value('i', 0)

            decorator_round = infinity_runner_decorator_factory2(config, uri, infinity_runner)

            @decorator_round
            def part3_round(infinity_pool, round_num: int):
                db_obj = infinity_pool.get_conn().get_database("default_db")
                table_obj = db_obj.get_table(table_name)

                # Verify indexes exist
                res = table_obj.list_indexes()
                index_names = res.index_names
                logging.info(f"Round {round_num + 1}: Indexes after restart: {index_names}")

                # Get current count before insert
                res, _, _ = table_obj.output(["count(*)"]).to_result()
                max_row_id = res["count(star)"][0]
                logging.info(f"Round {round_num + 1}: Start count: {max_row_id}")

                # Start parallel write/read threads
                end_time = time.time() + K_RUNNING_TIME_SECONDS
                threads = []

                workers = [
                    (self.insert_worker, insert_count, 2),
                    (self.update_worker, update_count, 1, max_row_id),
                    (self.delete_worker, delete_count, 2, max_row_id),
                    (self.read_worker_fulltext, read_count_fulltext, 1),
                    (self.read_worker_hnsw, read_count_hnsw, 1),
                    (self.read_worker_secondary_high, read_count_secondary_high, 1),
                    (self.read_worker_secondary_low, read_count_secondary_low, 1),
                    (self.read_worker_sparse, read_count_sparse, 1),
                    (self.read_worker_fusion_rrf, read_count_fusion_rrf, 1),
                    (self.read_worker_fusion_weighted_sum, read_count_fusion_weighted_sum, 1),
                ]

                thread_id = 0
                for worker_func, count, num_threads, *extra_args in workers:
                    for _ in range(num_threads):
                        t = Thread(target=worker_func, args=[infinity_pool, table_name, end_time, thread_id, count, round_num] + extra_args)
                        threads.append(t)
                        thread_id += 1

                for t in threads:
                    t.start()

                for t in threads:
                    t.join()

                res, _, _ = table_obj.output(["count(*)"]).to_result()
                end_count = res["count(star)"][0]
                logging.info(f"Round {round_num + 1}: End count: {end_count}, inserted {insert_count.value}, updated {update_count.value}, deleted {delete_count.value} rows")
                logging.info(f"  Read counts - FullText: {read_count_fulltext.value}, Hnsw: {read_count_hnsw.value}, "
                            f"HnswMV: {read_count_hnsw_mv.value}, "
                            f"SecondaryHigh: {read_count_secondary_high.value}, "
                            f"SecondaryLow: {read_count_secondary_low.value}, "
                            f"Sparse: {read_count_sparse.value}, "
                            f"FusionRRF: {read_count_fusion_rrf.value}, "
                            f"FusionMVRRF: {read_count_fusion_mv_rrf.value}, FusionWeighted: {read_count_fusion_weighted_sum.value}")

            part3_round(round_num)

        # Part 4: Cleanup
        @decorator
        def part4(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Error)
            logging.info("Cleaned up test table")

        part4()

        logging.info("Test completed successfully!")

    # Worker functions (defined as instance methods)
    def insert_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, insert_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0
        categories = ["A", "B", "C", "D"]
        text_words = ["apple", "banana", "cherry", "date"]

        while time.time() < end_time:
            try:
                vec = [random.random() for _ in range(2048)]
                multivec = np.array([[random.random() for _ in range(1024)] for _ in range(2)], dtype=np.float32)
                sparse_indices = [j for j in range(1024) if random.random() > 0.9]
                if not sparse_indices:
                    sparse_indices = [0, 1, 2]
                sparse_values = [random.randint(1, 100) for _ in range(len(sparse_indices))]
                sparse_vec = SparseVector(indices=sparse_indices, values=sparse_values)

                row_id = thread_id * 100000 + local_count
                table_obj.insert([{
                    "doctitle": f"test_title_{row_id}",
                    "docdate": "01-JAN-2024 00:00:00.000",
                    "body": f"test_text_{row_id}_{random.choice(text_words)}",
                    "num": row_id,
                    "category": categories[local_count % len(categories)],
                    "vector_col": vec,
                    "multi_vector_col": multivec,
                    "sparse_col": sparse_vec
                }])

                local_count += 1
                with insert_count.get_lock():
                    insert_count.value += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: insert failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num + 1} - thread {thread_id}: insert done, inserted {local_count} rows")

    def update_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, update_count, max_row_id, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                update_id = random.randint(0, max_row_id)
                vec = [random.random() for _ in range(2048)]
                multivec = np.array([[random.random() for _ in range(1024)] for _ in range(2)], dtype=np.float32)
                sparse_indices = [j for j in range(1024) if random.random() > 0.9]
                if not sparse_indices:
                    sparse_indices = [0, 1, 2]
                sparse_values = [random.randint(1, 100) for _ in range(len(sparse_indices))]
                sparse_vec = SparseVector(indices=sparse_indices, values=sparse_values)

                logging.info(f"thread {thread_id}: updating num={update_id}")
                table_obj.update(f"num = {update_id}", {
                    "doctitle": f"updated_title_{update_id}",
                    "vector_col": vec,
                    "multi_vector_col": multivec,
                    "sparse_col": sparse_vec
                })

                local_count += 1
                with update_count.get_lock():
                    update_count.value += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: update failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num + 1} - thread {thread_id}: update done, updated {local_count} rows")

    def delete_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, delete_count, max_row_id, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                delete_id = random.randint(0, max_row_id)
                logging.info(f"thread {thread_id}: deleting num={delete_id}")
                table_obj.delete(f"num = {delete_id}")

                local_count += 1
                with delete_count.get_lock():
                    delete_count.value += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: delete failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num + 1} - thread {thread_id}: delete done, deleted {local_count} rows")

    def read_worker_fulltext(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = table_obj.output(["num", "body"]).match_text("body", "test_text", 5).to_pl()
                if len(result) != 5:
                    raise Exception(f"FullText query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (FullText): read done, {local_count} queries")

    def read_worker_hnsw(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = table_obj.output(["num", "vector_col"]).match_dense("vector_col", [0.5] * 2048, "float", "l2", 5).to_pl()
                if len(result) != 5:
                    raise Exception(f"Hnsw query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Hnsw): read done, {local_count} queries")

    def read_worker_secondary_high(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = table_obj.output(["num"]).filter("num >= 0").to_pl()
                if len(result) == 0:
                    raise Exception(f"SecondaryHigh query returned 0 results")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Secondary High): read done, {local_count} queries")

    def read_worker_sparse(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                query_sparse = SparseVector(indices=[0, 5, 10, 15], values=[1.0, 1.0, 1.0, 1.0])
                result, _ = table_obj.output(["num", "sparse_col"]).match_sparse("sparse_col", query_sparse, "ip", 5).to_pl()
                if len(result) != 5:
                    raise Exception(f"SparseBMP query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Sparse BMP): read done, {local_count} queries")

    def read_worker_secondary_low(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = table_obj.output(["num", "category"]).filter("category = 'A'").to_pl()
                if len(result) == 0:
                    raise Exception(f"SecondaryLow query returned 0 results")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Secondary Low): read done, {local_count} queries")

    def read_worker_fusion_rrf(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = (table_obj
                 .output(["num", "body", "vector_col"])
                 .match_text("body", "test_text", 5)
                 .match_dense("vector_col", [0.5] * 2048, "float", "l2", 5)
                 .fusion(method='rrf', topn=5)
                 .to_pl())
                if len(result) != 5:
                    raise Exception(f"FusionRRF query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Fusion RRF): read done, {local_count} queries")

    def read_worker_fusion_weighted_sum(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count, round_num):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = (table_obj
                 .output(["num", "body", "vector_col"])
                 .match_text("body", "test_text", 5)
                 .match_dense("vector_col", [0.5] * 2048, "float", "l2", 5)
                 .fusion(method='weighted_sum', topn=5, fusion_params={"weights": "0.6,0.4"})
                 .to_pl())
                if len(result) != 5:
                    raise Exception(f"FusionWeighted query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        with read_count.get_lock():
            read_count.value += local_count
        logging.info(f"Round {round_num + 1} - thread {thread_id} (Fusion Weighted Sum): read done, {local_count} queries")
