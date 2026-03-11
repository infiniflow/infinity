import os
import random
import pytest
import logging
import time
from threading import Thread
from multiprocessing import Value
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory, infinity_runner_decorator_factory2
from restart_util import MultiIndexTypesGenerator
from common import common_values
from infinity.common import ConflictType, SparseVector
from infinity import index
from infinity.connection_pool import ConnectionPool


class TestMultipleIndexTypesImport:
    """Test multiple index types with import and restart scenarios"""

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "total_n, config",
        [
            (MultiIndexTypesGenerator.import_size(), "test/data/config/restart_test/test_insert/4.toml"),
        ],
    )
    @pytest.mark.parametrize(
        "columns, indexes, data_gen_factory, import_file, import_size, import_options",
        [
            (
                MultiIndexTypesGenerator.columns(),
                MultiIndexTypesGenerator.index(),
                MultiIndexTypesGenerator.gen_factory(MultiIndexTypesGenerator.import_file()),
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
        data_gen_factory,
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

            # for idx in indexes:
            #     table_obj.create_index(f"idx_{idx.target_name}", idx)
            #
            # logging.info(f"Created table and {len(indexes)} indexes")

        part1()

        # Part 2: Import data
        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            abs_import_file = os.path.abspath(import_file)
            kImportRepeat = 2

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

            for idx in indexes:
                idx_name = f"idx_{idx.target_name}"
                idx_start = time.time()
                logging.info(f"Creating index {idx_name}...")
                table_obj.create_index(idx_name, idx)
                idx_duration = time.time() - idx_start
                logging.info(f"Index {idx_name} created in {idx_duration:.2f} seconds")

            logging.info(f"Created table and {len(indexes)} indexes")

        part2()

        # Part 3: Restart 3 times - each round runs 120s with continuous insert/query
        kRunningTime = 20
        kWriteThreadNum = 4

        for round_num in range(3):
            # Track insert and query counts
            write_count = Value('i', 0)
            read_count_fulltext = Value('i', 0)
            read_count_hnsw = Value('i', 0)
            read_count_hnsw_mv = Value('i', 0)
            read_count_secondary_high = Value('i', 0)
            read_count_secondary_low = Value('i', 0)
            read_count_sparse = Value('i', 0)
            read_count_fusion_rrf = Value('i', 0)
            read_count_fusion_mv_rrf = Value('i', 0)
            read_count_fusion_weighted_sum = Value('i', 0)

            # Use decorator2 to get connection pool for multi-threading
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
                start_count = res["count(star)"][0]
                logging.info(f"Round {round_num + 1}: Start count: {start_count}")

                # Start parallel insert/query threads
                threads = []
                end_time = time.time() + kRunningTime

                def write_worker(connection_pool: ConnectionPool, table_name, end_time, thread_id, write_count):
                    infinity_obj = connection_pool.get_conn()
                    db_obj = infinity_obj.get_database("default_db")
                    table_obj = db_obj.get_table(table_name)
                    local_count = 0
                    categories = ["A", "B", "C", "D"]
                    text_words = ["apple", "banana", "cherry", "date"]

                    while time.time() < end_time:
                        try:
                            vec = [random.random() for _ in range(2048)]
                            # multi_vector: 2 vectors, each 1024-dim (total 2048 values)
                            multivec = [[random.random() for _ in range(2)] for _ in range(2)]
                            tensor_data = [[random.random() for _ in range(4)] for _ in range(3)]
                            sparse_indices = [j for j in range(100) if random.random() > 0.7]
                            if not sparse_indices:
                                sparse_indices = [0, 1, 2]
                            sparse_values = [random.random() for _ in range(len(sparse_indices))]
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
                            with write_count.get_lock():
                                write_count.value += 1
                        except Exception as e:
                            logging.warning(f"thread {thread_id}: insert failed: {e}")
                        time.sleep(0.1)

                    connection_pool.release_conn(infinity_obj)
                    logging.info(f"Round {round_num + 1} - thread {thread_id}: write done, inserted {local_count} rows")

                def read_worker_fulltext(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                def read_worker_hnsw(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                def read_worker_hnsw_mv(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
                    infinity_obj = connection_pool.get_conn()
                    db_obj = infinity_obj.get_database("default_db")
                    table_obj = db_obj.get_table(table_name)
                    local_count = 0

                    while time.time() < end_time:
                        try:
                            query_vec = [0.5] * 2048
                            result, _ = table_obj.output(["num", "multi_vector_col"]).match_dense("multi_vector_col", query_vec, "float", "l2", 5).to_pl()
                            if len(result) != 5:
                                raise Exception(f"HnswMV query expected 5 results, got {len(result)}")
                            local_count += 1
                        except Exception as e:
                            raise e
                        time.sleep(0.1)

                    connection_pool.release_conn(infinity_obj)
                    with read_count.get_lock():
                        read_count.value += local_count
                    logging.info(f"Round {round_num + 1} - thread {thread_id} (Hnsw MV): read done, {local_count} queries")

                def read_worker_secondary_high(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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


                
                def read_worker_sparse(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                def read_worker_secondary_low(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                def read_worker_fusion_rrf(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                def read_worker_fusion_mv_rrf(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
                    infinity_obj = connection_pool.get_conn()
                    db_obj = infinity_obj.get_database("default_db")
                    table_obj = db_obj.get_table(table_name)
                    local_count = 0

                    while time.time() < end_time:
                        try:
                            query_vec = [0.5] * 2048
                            query_mv = [0.5] * 2048
                            result, _ = (table_obj
                             .output(["num", "vector_col", "multi_vector_col"])
                             .match_dense("vector_col", query_vec, "float", "l2", 5)
                             .match_dense("multi_vector_col", query_mv, "float", "l2", 5)
                             .fusion(method='rrf', topn=5)
                             .to_pl())
                            if len(result) != 5:
                                raise Exception(f"FusionMV query expected 5 results, got {len(result)}")
                            local_count += 1
                        except Exception as e:
                            raise e
                        time.sleep(0.1)

                    connection_pool.release_conn(infinity_obj)
                    with read_count.get_lock():
                        read_count.value += local_count
                    logging.info(f"Round {round_num + 1} - thread {thread_id} (Fusion MV RRF): read done, {local_count} queries")

                def read_worker_fusion_weighted_sum(connection_pool: ConnectionPool, table_name, end_time, thread_id, read_count):
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

                # Start 4 write threads
                for i in range(kWriteThreadNum):
                    t = Thread(target=write_worker, args=[infinity_pool, table_name, end_time, i, write_count])
                    threads.append(t)

                # Start 10 read threads (one for each index type)
                t = Thread(target=read_worker_fulltext, args=[infinity_pool, table_name, end_time, 4, read_count_fulltext])
                threads.append(t)

                t = Thread(target=read_worker_hnsw, args=[infinity_pool, table_name, end_time, 5, read_count_hnsw])
                threads.append(t)

                t = Thread(target=read_worker_hnsw_mv, args=[infinity_pool, table_name, end_time, 6, read_count_hnsw_mv])
                threads.append(t)

                t = Thread(target=read_worker_secondary_high, args=[infinity_pool, table_name, end_time, 7, read_count_secondary_high])
                threads.append(t)

                t = Thread(target=read_worker_secondary_low, args=[infinity_pool, table_name, end_time, 8, read_count_secondary_low])
                threads.append(t)

                t = Thread(target=read_worker_sparse, args=[infinity_pool, table_name, end_time, 9, read_count_sparse])
                threads.append(t)

                t = Thread(target=read_worker_fusion_rrf, args=[infinity_pool, table_name, end_time, 10, read_count_fusion_rrf])
                threads.append(t)

                t = Thread(target=read_worker_fusion_mv_rrf, args=[infinity_pool, table_name, end_time, 11, read_count_fusion_mv_rrf])
                threads.append(t)

                t = Thread(target=read_worker_fusion_weighted_sum, args=[infinity_pool, table_name, end_time, 12, read_count_fusion_weighted_sum])
                threads.append(t)

                # Start all threads
                for t in threads:
                    t.start()

                # Wait for all threads
                for t in threads:
                    t.join()

                # Verify final count increased
                res, _, _ = table_obj.output(["count(*)"]).to_result()
                end_count = res["count(star)"][0]
                logging.info(f"Round {round_num + 1}: End count: {end_count}, inserted {write_count.value} rows")
                logging.info(f"  Read counts - FullText: {read_count_fulltext.value}, Hnsw: {read_count_hnsw.value}, "
                            f"HnswMV: {read_count_hnsw_mv.value}, "
                            f"SecondaryHigh: {read_count_secondary_high.value}, "
                            f"SecondaryLow: {read_count_secondary_low.value}, "
                            f"Sparse: {read_count_sparse.value}, "
                            f"FusionRRF: {read_count_fusion_rrf.value}, "
                            f"FusionMVRRF: {read_count_fusion_mv_rrf.value}, FusionWeighted: {read_count_fusion_weighted_sum.value}")

                # Verify at least some inserts happened
                assert end_count > start_count, f"Expected count to increase, got {start_count} -> {end_count}"

            part3_round(round_num)

        # Part 4: Cleanup
        @decorator
        def part4(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Error)
            logging.info("Cleaned up test table")

        part4()

        logging.info("Test completed successfully!")
