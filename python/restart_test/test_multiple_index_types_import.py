import os
import random
import pytest
import logging
import time
from threading import Thread
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory, infinity_runner_decorator_factory2
from restart_util import MultiIndexTypesGenerator
from common import common_values
from infinity.common import ConflictType, SparseVector
from infinity.connection_pool import ConnectionPool

class TestMultipleIndexTypesImport:
    @pytest.mark.parametrize(
        "config, generator, kImportRepeat, kBatchCount, kRunTime",
        [
            pytest.param("test/data/config/restart_test/test_insert/5.toml", MultiIndexTypesGenerator, 50, 100, 120, marks=pytest.mark.slow),
            pytest.param("test/data/config/restart_test/test_insert/4.toml", MultiIndexTypesGenerator, 2, 2, 10, marks=pytest.mark.ubsan),
            pytest.param("test/data/config/restart_test/test_insert/5.toml", MultiIndexTypesGenerator, 2, 2, 10, marks=pytest.mark.ubsan),
            pytest.param("test/data/config/restart_test/test_insert/6.toml", MultiIndexTypesGenerator, 2, 2, 10, marks=pytest.mark.ubsan),
        ],
    )
    def test_multiple_index_types_import_restart(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        generator,
        kImportRepeat,
        kBatchCount,
        kRunTime
    ):
        # Initialize
        infinity_runner.clear()
        self.generator = generator
        table_name = "test_multi_index_types"
        uri = common_values.TEST_LOCAL_HOST
        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        # Part 1: Create table and indexes
        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)

            table_obj = db_obj.create_table(table_name, self.generator.columns(), ConflictType.Error)
            assert table_obj is not None

            for idx in self.generator.index():
                idx_name = f"idx_{idx.target_name}"
                logging.info(f"Creating index {idx_name}...")
                table_obj.create_index(idx_name, idx)

            logging.info(f" Created table and {len(self.generator.index())} indexes")

        part1()

        # Part 2: Import and insert data
        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            abs_import_file = os.path.abspath(self.generator.generate_import_file())

            for import_round in range(kImportRepeat):
                import_start = time.time()
                logging.info(f"Importing data from {abs_import_file}... (round {import_round + 1}/{kImportRepeat})")
                table_obj.import_data(abs_import_file, self.generator.import_options())
                import_duration = time.time() - import_start
                logging.info(f"Import round {import_round + 1} completed in {import_duration:.2f} seconds")

            # Verify row count after imports
            expected_count = self.generator.import_size() * kImportRepeat
            res, _, _ = table_obj.output(["count(*)"]).to_result()
            count = res["count(star)"][0]
            logging.info(f"Total rows after {kImportRepeat} imports: {count}")
            assert count == expected_count, f"Expected {expected_count} rows, got {count}"

            kRowsPerBatch = 5000
            for batch_idx in range(kBatchCount):
                batch_data = []
                for row_idx in range(kRowsPerBatch):
                    row_id = batch_idx * kRowsPerBatch + row_idx
                    batch_data.append(self.generator.generate_random_row(row_id))

                insert_start = time.time()
                table_obj.insert(batch_data)
                insert_duration = time.time() - insert_start
                logging.info(f"Inserted batch {batch_idx + 1}/{kBatchCount} ({kRowsPerBatch} rows) in {insert_duration:.2f} seconds")

            # Verify row count after inserts
            res, _, _ = table_obj.output(["count(*)"]).to_result()
            count = res["count(star)"][0]
            expected_count = self.generator.import_size() * kImportRepeat + kBatchCount * kRowsPerBatch
            logging.info(f"Total rows after batch insert: {count}, expected: {expected_count}")
            assert count == expected_count, f"Expected {expected_count} rows after batch insert, got {count}"

        # Parallel operations round (used multiple times)
        def run_parallel_round(round_num: int):
            decorator_round = infinity_runner_decorator_factory2(config, uri, infinity_runner)

            @decorator_round
            def round_worker(infinity_pool, round_num: int):
                db_obj = infinity_pool.get_conn().get_database("default_db")
                table_obj = db_obj.get_table(table_name)

                # Verify indexes exist
                res = table_obj.list_indexes()
                index_names = res.index_names
                logging.info(f"Round {round_num}: Indexes after restart: {index_names}")

                res, _, _ = table_obj.output(["count(*)"]).to_result()
                max_row_id = res["count(star)"][0]
                logging.info(f"Round {round_num}: Start count: {max_row_id}")

                workers = [
                    (self.insert_worker, 2),
                    (self.update_worker, 2),
                    (self.delete_worker, 2),
                    (self.read_worker_fulltext, 1),
                    (self.read_worker_hnsw, 1),
                    (self.read_worker_hnsw_mv, 1),
                    (self.read_worker_secondary_high, 1),
                    (self.read_worker_secondary_low, 1),
                    (self.read_worker_sparse, 1),
                    (self.read_worker_fusion_rrf, 1),
                    (self.read_worker_fusion_weighted_sum, 1),
                    (self.read_worker_fusion_mv_rrf, 1),
                ]

                end_time = time.time() + kRunTime
                thread_id = 0
                threads = []
                for worker_func, num_threads in workers:
                    for _ in range(num_threads):
                        t = Thread(target=worker_func, args=[infinity_pool, table_name, end_time, thread_id, round_num, max_row_id])
                        threads.append(t)
                        thread_id += 1

                for t in threads:
                    t.start()

                for t in threads:
                    t.join()

            round_worker(round_num)

        part2()
        run_parallel_round(1)
        run_parallel_round(2)

        @decorator
        def part3(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            # Drop all indexes
            for idx in self.generator.index():
                idx_name = f"idx_{idx.target_name}"
                table_obj.drop_index(idx_name, ConflictType.Ignore)
            logging.info("Dropped all indexes")

            # Recreate all indexes and measure time
            for idx in self.generator.index():
                idx_name = f"idx_{idx.target_name}"
                idx_start = time.time()
                logging.info(f"Recreating index {idx_name}...")
                table_obj.create_index(idx_name, idx)
                idx_duration = time.time() - idx_start
                logging.info(f"Index {idx_name} created in {idx_duration:.2f} seconds")

        part3()
        run_parallel_round(3)
        run_parallel_round(4)

        @decorator
        def part4(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")

            snapshot_name = "test_multi_index_snapshot"
            db_obj.create_table_snapshot(snapshot_name, table_name)
            logging.info(f"Created snapshot {snapshot_name}")

            db_obj.drop_table(table_name, ConflictType.Error)
            logging.info("Dropped test table")

            db_obj.restore_table_snapshot(snapshot_name)
            logging.info(f"Restored snapshot {snapshot_name}")

            # Verify table exists
            tables = db_obj.list_tables()
            assert table_name in tables.table_names, f"Table {table_name} not found after restore"
            logging.info(f"Verified: table {table_name} exists")

            # Verify all indexes exist
            table_obj = db_obj.get_table(table_name)
            indexes = table_obj.list_indexes()
            index_names = indexes.index_names
            expected_index_count = len(list(self.generator.index()))
            assert len(index_names) == expected_index_count, f"Expected {expected_index_count} indexes, got {len(index_names)}"
            logging.info(f"Verified: {len(index_names)} indexes exist")

        part4()
        run_parallel_round(5)
        run_parallel_round(6)

        @decorator
        def part5(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Error)
            logging.info("Dropped test table")

            snapshot_name = "test_multi_index_snapshot"
            infinity_obj.drop_snapshot(snapshot_name)
            logging.info(f"Dropped snapshot {snapshot_name}")

        part5()
        logging.info("Test completed successfully!")

    # Worker functions
    def insert_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0
        while time.time() < end_time:
            try:
                row_id = thread_id * 100000 + local_count
                table_obj.insert([self.generator.generate_random_row(row_id)])

                local_count += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: insert failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num} - thread {thread_id}: insert done, executed {local_count} times")

    def update_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                update_id = random.randint(0, max_row_id)
                row_data = self.generator.generate_random_row(update_id)

                # logging.info(f"thread {thread_id}: updating num={update_id}")
                table_obj.update(f"num = {update_id}", {
                    "doctitle": f"updated_title_{update_id}",
                    "vector_col": row_data["vector_col"],
                    "multi_vector_col": row_data["multi_vector_col"],
                    "sparse_col": row_data["sparse_col"]
                })

                local_count += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: update failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num} - thread {thread_id}: update done, executed {local_count} times")

    def delete_worker(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                delete_id = random.randint(0, max_row_id)
                # logging.info(f"thread {thread_id}: deleting num={delete_id}")
                table_obj.delete(f"num = {delete_id}")

                local_count += 1
            except Exception as e:
                logging.warning(f"thread {thread_id}: delete failed: {e}")
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num} - thread {thread_id}: delete done, executed {local_count} times")

    def read_worker_fulltext(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (FullText): read done, {local_count} queries")

    def read_worker_hnsw(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Hnsw): read done, {local_count} queries")

    def read_worker_hnsw_mv(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = table_obj.output(["num", "multi_vector_col"]).match_dense("multi_vector_col", [[0.5] * 1024] * 2, "float", "l2", 5).to_pl()
                if len(result) != 5:
                    raise Exception(f"HnswMV query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num} - thread {thread_id} (Hnsw MV): read done, {local_count} queries")

    def read_worker_secondary_high(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Secondary High): read done, {local_count} queries")

    def read_worker_sparse(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Sparse BMP): read done, {local_count} queries")

    def read_worker_secondary_low(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Secondary Low): read done, {local_count} queries")

    def read_worker_fusion_rrf(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Fusion RRF): read done, {local_count} queries")

    def read_worker_fusion_weighted_sum(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
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
        logging.info(f"Round {round_num} - thread {thread_id} (Fusion Weighted Sum): read done, {local_count} queries")

    def read_worker_fusion_mv_rrf(self, connection_pool: ConnectionPool, table_name, end_time, thread_id, round_num, max_row_id):
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        local_count = 0

        while time.time() < end_time:
            try:
                result, _ = (table_obj
                 .output(["num", "body", "vector_col", "multi_vector_col"])
                 .match_text("body", "test_text", 5)
                 .match_dense("vector_col", [0.5] * 2048, "float", "l2", 5)
                 .match_dense("multi_vector_col", [[0.5] * 1024] * 2, "float", "l2", 5)
                 .fusion(method='rrf', topn=5)
                 .to_pl())
                if len(result) != 5:
                    raise Exception(f"FusionMVRRF query expected 5 results, got {len(result)}")
                local_count += 1
            except Exception as e:
                raise e
            time.sleep(0.1)

        connection_pool.release_conn(infinity_obj)
        logging.info(f"Round {round_num} - thread {thread_id} (Fusion MV RRF): read done, {local_count} queries")
