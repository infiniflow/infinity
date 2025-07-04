import random
import pytest
from common import common_values
from infinity.common import ConflictType
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
import time
import threading
from restart_util import *
from util import RtnThread


class TestInsert:
    def insert_inner(
        self,
        insert_n: int,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST
        data_gen = data_gen_factory(insert_n)

        stop_n = 10
        uri = common_values.TEST_LOCAL_HOST

        cur_insert_n = 0
        last_insert_n = 0
        shutdown = False
        error = False

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner, shutdown_out=True)

        def insert_func(table_obj):
            nonlocal cur_insert_n, last_insert_n, shutdown, error
            batch_size = 10

            while cur_insert_n < insert_n:
                insert_data = []
                try:
                    # get `batch_size` data in data_gen one time
                    for i in range(batch_size):
                        try:
                            data = next(data_gen)
                            data_line = {}
                            for col_name, col_data in zip(columns.keys(), data):
                                data_line[col_name] = col_data
                            insert_data.append(data_line)
                        except StopIteration:
                            break
                    if len(insert_data) > 0:
                        last_insert_n = len(insert_data)
                        table_obj.insert(insert_data)
                    else:
                        cur_insert_n = insert_n
                except Exception as e:
                    print(f"insert error at {cur_insert_n}")
                    if not shutdown:
                        error = True
                        raise e
                    break
                cur_insert_n += len(insert_data)

        shutdown_time = 0

        def shutdown_func():
            nonlocal cur_insert_n, shutdown_time, shutdown, error
            shutdown = False
            last_shutdown_insert_n = cur_insert_n
            while not error:
                if cur_insert_n >= insert_n or (
                    stop_n != 0
                    and cur_insert_n - last_shutdown_insert_n >= insert_n // stop_n
                ):
                    shutdown = True
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    shutdown_time += 1
                    return
                print(f"cur_insert_n inner: {cur_insert_n}")
                time.sleep(0.1)

        @decorator
        def part1(infinity_obj):
            nonlocal cur_insert_n, last_insert_n
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert")

            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]

            # insert while shutdown might fail or success
            assert count_star in {cur_insert_n, cur_insert_n + last_insert_n}
            cur_insert_n = count_star
            print(f"cur_insert_n: {cur_insert_n}")

            t1 = threading.Thread(target=shutdown_func)
            t1.start()
            insert_func(table_obj)
            t1.join()

        while cur_insert_n < insert_n:
            part1()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "insert_n, config",
        [
            (100000, "test/data/config/restart_test/test_insert/1.toml"),
            # (1000000, "test/data/config/restart_test/test_insert/1.toml"),
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
        insert_n: int,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.create_table("test_insert", columns, ConflictType.Error)
        part1()

        self.insert_inner(insert_n, infinity_runner, config, columns, data_gen_factory)

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert", ConflictType.Error)
        part2()

    @pytest.mark.slow
    @pytest.mark.parametrize(
        "insert_n,config",
        [
            (100000, "test/data/config/restart_test/test_insert/1.toml"),
            # (1000000, "test/data/config/restart_test/test_insert/1.toml"),
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
        insert_n: int,
        config: str,
        columns: dict,
        indexes: list[index.IndexInfo],
        data_gen_factory,
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

        self.insert_inner(insert_n, infinity_runner, config, columns, data_gen_factory)

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert", ConflictType.Error)
        part2()

    def _test_insert_checkpoint_inner(
        self, infinity_runner: InfinityRunner, test_n: int
    ):
        infinity_runner.clear()
        config = "test/data/config/restart_test/test_insert/6.toml"
        uri = common_values.TEST_LOCAL_HOST

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        line_num = 0

        @decorator
        def create_table(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.create_table("test_insert_checkpoint", {"c1": {"type": "int"}})

        @decorator
        def part1(infinity_obj, test_i: int):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert_checkpoint")

            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == line_num

            stop_insert = False

            def checkpoint_func(infinity_obj):
                nonlocal stop_insert
                print("start checkpoint")
                try:
                    time.sleep(0.2)
                    infinity_obj.flush_data()
                    print("checkpoint")
                    time.sleep(0.2)
                except Exception as e:
                    print(e)
                    stop_insert = True
                    raise e
                stop_insert = True
                print("end checkpoint")

            def insert_func(table_obj):
                nonlocal stop_insert, line_num
                while not stop_insert:
                    table_obj.insert([{"c1": i} for i in range(8192)])
                    line_num += 8192
                print("insert end")

            t1 = RtnThread(target=insert_func, args=(table_obj,))
            t1.start()
            checkpoint_func(infinity_obj)
            t1.join()
            print(f"join end {test_i}")

        @decorator
        def drop_table(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_insert_checkpoint", ConflictType.Error)

        create_table()
        print("create table done")
        for i in range(test_n):
            part1(i)
        drop_table()
        print("drop table done")

    def test_insert_checkpoint(self, infinity_runner: InfinityRunner):
        self._test_insert_checkpoint_inner(infinity_runner, 10)

    @pytest.mark.slow
    def test_insert_checkpoint_slow(self, infinity_runner: InfinityRunner):
        self._test_insert_checkpoint_inner(infinity_runner, 100)


if __name__ == "__main__":
    pass
