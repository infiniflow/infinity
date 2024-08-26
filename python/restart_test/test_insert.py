from abc import abstractmethod
import functools
import pytest
from common import common_values
from infinity.common import ConflictType
from infinity_runner import InfinityRunner
import time
import threading
import csv


@pytest.mark.slow
class TestInsert:
    def simple_embedding_generator(insert_n: int):
        for i in range(insert_n):
            yield i, [0.1, 0.2, 0.3, 0.4]

    def simple_varchar_generator(insert_n: int):
        for i in range(insert_n):
            if i % 2 == 0:
                yield i, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
            else:
                yield i, "test"

    def simple_tensor_generator(insert_n: int):
        for i in range(insert_n):
            if i % 2 == 0:
                yield i, [0.1, 0.2, 0.3, 0.4]
            else:
                yield i, [[0.1, 0.2, 0.3, 0.4], [0.5, 0.6, 0.7, 0.8]]

    def enwiki_generator(enwiki_path: str, insert_n: int):
        for i in range(insert_n):
            with open(enwiki_path, "r") as f:
                reader = csv.reader(f, delimiter="\t")
                for row in reader:
                    title = row[0]
                    date = row[1]
                    body = row[2]
                    yield title, date, body

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
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
                simple_embedding_generator,
            ),
            (
                {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
                simple_varchar_generator,
            ),
            (
                {"c1": {"type": "int"}, "c2": {"type": "tensor,4,float"}},
                simple_tensor_generator,
            ),
            (
                {
                    "title": {"type": "varchar"},
                    "date": {"type": "varchar"},
                    "body": {"type": "varchar"},
                },
                functools.partial(enwiki_generator, "test/data/csv/enwiki-10w.csv"),
            ),
        ],
    )
    def test_insert(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        insert_n = 100000
        data_gen = data_gen_factory(insert_n)

        stop_n = 10
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        infinity_runner.init(config)

        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.create_table("test_insert", columns, ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()

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
                if (
                    gen_finished
                    or cur_insert_n - last_shutdown_insert_n >= insert_n // stop_n
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

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_insert", ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()


if __name__ == "__main__":
    pass
