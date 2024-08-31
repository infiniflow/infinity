import threading
import time
import pytest
from infinity_runner import InfinityRunner
from restart_util import *
from common import common_values
from infinity.common import ConflictType
import random


class TestInsertImport:
    def insert_import_inner(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
        import_file: str,
        import_size: int,
        import_options: dict,
    ):
        uri = common_values.TEST_LOCAL_HOST

        total_n = 1000000
        data_gen = data_gen_factory(total_n)

        stop_n = 10
        uri = common_values.TEST_LOCAL_HOST

        cur_n = 0
        gen_finished = False
        insert_finish = False

        def insert_import_func(table_obj):
            nonlocal cur_n, gen_finished, insert_finish
            insert_batch_size = 10

            import_rate = insert_batch_size / import_size

            while cur_n < total_n and not gen_finished:
                r = random.randint(0, 100)
                if_import = r < import_rate * 100
                if insert_finish:
                    print("insert finished")
                if not if_import and not insert_finish:
                    try:
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
                        table_obj.insert(insert_data)
                    except Exception as e:
                        break
                    cur_n += insert_batch_size
                else:
                    try:
                        table_obj.import_data(import_file, import_options)
                    except Exception as e:
                        break
                    cur_n += import_size
                if cur_n >= total_n:
                    gen_finished = True

        shutdown_time = 0

        def shutdown_func():
            nonlocal cur_n, shutdown_time
            last_shutdown_n = cur_n
            while True:
                if gen_finished or (
                    stop_n != 0 and cur_n - last_shutdown_n >= total_n // stop_n
                ):
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    shutdown_time += 1
                    return
                print(f"cur_n: {cur_n}")
                time.sleep(0.1)

        while not gen_finished:
            infinity_runner.init(config)
            infinity_obj = InfinityRunner.connect(uri)

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert")

            data_dict, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == cur_n
            print(f"cur_n: {cur_n}")

            t1 = threading.Thread(target=shutdown_func)
            t1.start()
            insert_import_func(table_obj)
            t1.join()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_insert/4.toml",
            "test/data/config/restart_test/test_insert/5.toml",
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
    def test_data(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
        import_file: str,
        import_size: int,
        import_options: dict,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        infinity_runner.init(config)

        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.create_table("test_insert", columns, ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()

        self.insert_import_inner(
            infinity_runner,
            config,
            columns,
            data_gen_factory,
            import_file,
            import_size,
            import_options,
        )

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert", ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_insert/4.toml",
            "test/data/config/restart_test/test_insert/5.toml",
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
                "test/data/benchmark/sift_1m/sift_learn.fvecs",
                128,
                {"file_type": "fvecs"},
            ),
            (
                LChYDataGenerato.columns(),
                LChYDataGenerato.index(),
                LChYDataGenerato.gen_factory("test/data/jsonl/test_table.jsonl"),
                "test/data/jsonl/test_table.jsonl",
                10,
                {"file_type": "jsonl"},
            ),
        ],
    )
    def test_index(
        self,
        infinity_runner: InfinityRunner,
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

        infinity_runner.init(config)

        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.create_table("test_insert", columns, ConflictType.Error)
        for idx in indexes:
            table_obj.create_index(f"idx_{idx.column_name}", idx)
        infinity_obj.disconnect()
        infinity_runner.uninit()

        self.insert_import_inner(
            infinity_runner,
            config,
            columns,
            data_gen_factory,
            import_file,
            import_size,
            import_options,
        )

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert", ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()
