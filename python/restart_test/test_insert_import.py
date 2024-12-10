import os
import threading
import time
import pytest
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from restart_util import *
from common import common_values
from infinity.common import ConflictType
import random
from util import RtnThread


class TestInsertImport:
    def insert_import_inner(
        self,
        infinity_runner: InfinityRunner,
        total_n: int,
        config: str,
        columns: dict,
        data_gen_factory,
        import_file: str,
        import_size: int,
        import_options: dict,
    ):
        uri = common_values.TEST_LOCAL_HOST

        data_gen = data_gen_factory(total_n)

        stop_n = 10
        interval_n = total_n // stop_n
        uri = common_values.TEST_LOCAL_HOST

        insert_batch_size = 10
        import_rate = insert_batch_size / (import_size + insert_batch_size)

        cur_n = 0
        insert_finish = False
        shutdown = False
        error = False

        lock = threading.Lock()

        logger = infinity_runner.logger
        write_i = 0

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner, True)

        def insert_import_func(table_obj):
            nonlocal cur_n, insert_finish, shutdown, error, write_i

            while cur_n < total_n:
                r = random.randint(0, 100)
                if_import = r < import_rate * 100
                if insert_finish:
                    logger.debug("insert finished")
                try:
                    if not if_import and not insert_finish:
                        insert_data = []
                        # get `batch_size` data in data_gen one time
                        for i in range(insert_batch_size):
                            try:
                                data = next(data_gen)
                                data_line = {}
                                for col_name, col_data in zip(columns.keys(), data):
                                    data_line[col_name] = col_data
                                insert_data.append(data_line)
                            except StopIteration:
                                insert_finish = True
                                break
                        if len(insert_data) > 0:
                            table_obj.insert(insert_data)
                        else:
                            cur_n = total_n
                        cur_n += insert_batch_size
                        logger.debug(f"{write_i}. insert {insert_batch_size} data, cur_n: {cur_n}")
                    else:
                        abs_import_file = os.path.abspath(import_file)
                        table_obj.import_data(abs_import_file, import_options)
                        cur_n += import_size
                        logger.debug(f"{write_i}. import {import_size} data, cur_n: {cur_n}")
                    write_i += 1
                except Exception as e:
                    logger.debug(f"insert/import {if_import} error at {cur_n}")
                    with lock:
                        if not shutdown:
                            error = True
                            raise e
                    break

        shutdown_time = 0

        def shutdown_func():
            nonlocal cur_n, shutdown_time, shutdown, error
            shutdown = False
            last_shutdown_n = cur_n
            while not error:
                if cur_n >= total_n or (
                    stop_n != 0 and cur_n - last_shutdown_n >= interval_n
                ):
                    with lock:
                        shutdown = True
                        infinity_runner.uninit()
                        logger.debug("shutdown infinity")
                    shutdown_time += 1
                    return
                logger.debug(f"cur_n inner: {cur_n}")
                time.sleep(0.1)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert")

            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == cur_n
            logger.debug(f"cur_n: {cur_n}")

            t1 = RtnThread(target=shutdown_func)
            t1.start()
            insert_import_func(table_obj)
            t1.join()

        while cur_n < total_n:
            part1()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "total_n, config",
        [
            (10000, "test/data/config/restart_test/test_insert/4.toml"),
            (10000, "test/data/config/restart_test/test_insert/5.toml"),
        ],
    )
    @pytest.mark.parametrize(
        "columns, data_gen_factory, import_file, import_size, import_options",
        [
            (
                SimpleEmbeddingGenerator.columns(),
                SimpleEmbeddingGenerator.gen_factory(),
                "test/data/csv/embedding_float_dim4.csv",
                4,
                {"file_type": "csv"},
            ),
            (
                SimpleVarcharGenerator.columns(),
                SimpleVarcharGenerator.gen_factory(),
                "test/data/csv/varchar.csv",
                4,
                {"file_type": "csv"},
            ),
            (
                SimpleTensorGenerator.columns(),
                SimpleTensorGenerator.gen_factory(),
                "test/data/csv/tensor_float_dim4.csv",
                4,
                {"file_type": "csv"},
            ),
        ],
    )
    def test_data_with_import(
        self,
        infinity_runner: InfinityRunner,
        total_n: int,
        config: str,
        columns: dict,
        data_gen_factory,
        import_file: str,
        import_size: int,
        import_options: dict,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.create_table("test_insert", columns, ConflictType.Error)

        part1()

        self.insert_import_inner(
            infinity_runner,
            total_n,
            config,
            columns,
            data_gen_factory,
            import_file,
            import_size,
            import_options,
        )

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert", ConflictType.Error)

        part2()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "total_n, config",
        [
            (100000, "test/data/config/restart_test/test_insert/4.toml"),
            (100000, "test/data/config/restart_test/test_insert/5.toml"),
        ],
    )
    @pytest.mark.parametrize(
        "columns, indexes, data_gen_factory, import_file, import_size, import_options",
        [
            (
                EnwikiGenerator.columns(),
                EnwikiGenerator.index(),
                EnwikiGenerator.gen_factory("test/data/benchmark/enwiki-10w.csv"),
                "test/data/csv/enwiki_99.csv",
                99,
                {"file_type": "csv", "delimiter": "\t"},
            ),
            (
                SiftGenerator.columns(),
                SiftGenerator.index(),
                SiftGenerator.gen_factory(
                    "test/data/benchmark/sift_1m/sift_base.fvecs"
                ),
                "test/data/benchmark/sift_1m/sift_query.fvecs",
                10000,
                {"file_type": "fvecs"},
            ),
            (
                LChYDataGenerato.columns(),
                LChYDataGenerato.index(),
                LChYDataGenerato.gen_factory("test/data/jsonl/test_table.jsonl"),
                LChYDataGenerato.import_file(),
                LChYDataGenerato.import_size(),
                {"file_type": "jsonl"},
            ),
        ],
    )
    def test_index_with_import(
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
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.create_table("test_insert", columns, ConflictType.Error)
            for idx in indexes:
                table_obj.create_index(f"idx_{idx.column_name}", idx)

        part1()

        self.insert_import_inner(
            infinity_runner,
            total_n,
            config,
            columns,
            data_gen_factory,
            import_file,
            import_size,
            import_options,
        )

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert", ConflictType.Error)

        part2()
