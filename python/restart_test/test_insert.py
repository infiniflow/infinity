import functools
import pytest
from common import common_values
from infinity.common import ConflictType
from infinity_runner import InfinityRunner
import time
import threading
import itertools
from restart_util import *


@pytest.mark.slow
class TestInsert:
    def insert_inner(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST

        insert_n = 1000000
        data_gen = data_gen_factory(insert_n)

        stop_n = 10
        uri = common_values.TEST_LOCAL_HOST

        cur_insert_n = 0
        gen_finished = False

        def insert_func(table_obj):
            nonlocal cur_insert_n, gen_finished
            batch_size = 10

            while cur_insert_n < insert_n and not gen_finished:
                try:
                    insert_data = []
                    # get `batch_size` data in data_gen one time
                    for i in range(batch_size):
                        try:
                            data = next(data_gen)
                            data_line = {}
                            for col_name, col_data in zip(columns.keys(), data):
                                data_line[col_name] = col_data
                            insert_data.append(data_line)
                        except StopIteration:
                            gen_finished = True
                            break
                    table_obj.insert(insert_data)
                except Exception as e:
                    break
                cur_insert_n += batch_size
                if cur_insert_n == insert_n:
                    gen_finished = True

        shutdown_time = 0

        def shutdown_func():
            nonlocal cur_insert_n, shutdown_time
            last_shutdown_insert_n = cur_insert_n
            while True:
                if gen_finished or (
                    stop_n != 0
                    and cur_insert_n - last_shutdown_insert_n >= insert_n // stop_n
                ):
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    shutdown_time += 1
                    return
                print(f"cur_insert_n: {cur_insert_n}")
                time.sleep(0.1)

        while not gen_finished:
            infinity_runner.init(config)
            infinity_obj = InfinityRunner.connect(uri)

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert")

            data_dict, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == cur_insert_n
            print(f"cur_insert_n: {cur_insert_n}")

            t1 = threading.Thread(target=shutdown_func)
            t1.start()
            insert_func(table_obj)
            t1.join()

    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_insert/1.toml",
            "test/data/config/restart_test/test_insert/2.toml",
        ],
    )
    @pytest.mark.parametrize(
        "columns, data_gen_factory",
        [
            (
                SimpleEmbeddingGenerator.columns(),
                SimpleEmbeddingGenerator.gen_factory(),
            ),
            (
                SimpleVarcharGenerator.columns(),
                SimpleVarcharGenerator.gen_factory(),
            ),
            (
                SimpleTensorGenerator.columns(),
                SimpleTensorGenerator.gen_factory(),
            ),
            (
                EnwikiGenerator.columns(),
                EnwikiGenerator.gen_factory("test/data/benchmark/enwiki-10w.csv"),
            ),
            (
                SiftGenerator.columns(),
                SiftGenerator.gen_factory(
                    "test/data/benchmark/sift_1m/sift_base.fvecs"
                ),
            ),
            (
                LChYDataGenerato.columns(),
                LChYDataGenerato.gen_factory("test/data/jsonl/test_table.jsonl"),
            ),
        ],
    )
    def test_data(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        infinity_runner.init(config)

        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.create_table("test_insert", columns, ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()

        self.insert_inner(infinity_runner, config, columns, data_gen_factory)

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert", ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()

    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_insert/1.toml",
            "test/data/config/restart_test/test_insert/3.toml",
        ],
    )
    @pytest.mark.parametrize(
        "columns, indexes, data_gen_factory",
        [
            (
                EnwikiGenerator.columns(),
                EnwikiGenerator.index(),
                EnwikiGenerator.gen_factory("test/data/benchmark/enwiki-10w.csv"),
            ),
            (
                SiftGenerator.columns(),
                SiftGenerator.index(),
                SiftGenerator.gen_factory(
                    "test/data/benchmark/sift_1m/sift_base.fvecs"
                ),
            ),
            (
                LChYDataGenerato.columns(),
                LChYDataGenerato.index(),
                LChYDataGenerato.gen_factory("test/data/jsonl/test_table.jsonl"),
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

        self.insert_inner(infinity_runner, config, columns, data_gen_factory)

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert", ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()


if __name__ == "__main__":
    pass
