import pytest
from common import common_values
from infinity.common import ConflictType
from infinity_runner import InfinityRunner
import time
import threading


@pytest.mark.slow
class TestInsert:
    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_insert/1.toml",
            # "test/data/config/restart_test/test_insert/2.toml",
        ],
    )
    @pytest.mark.parametrize(
        "columns, data",
        [
            (
                {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}},
                [{"c1": 0, "c2": [0.1, 0.2, 0.3, 0.4]}],
            ),
            (
                {"c1": {"type": "int"}, "c2": {"type": "varchar"}},
                [
                    {"c1": 1, "c2": "test"},
                    {
                        "c1": 2,
                        "c2": "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
                    },
                ],
            ),
            (
                {"c1": {"type": "int"}, "c2": {"type": "tensor,4,float"}},
                [
                    {"c1": 0, "c2": [0.1, 0.2, 0.3, 0.4]},
                    {"c1": 1, "c2": [[0.1, 0.2, 0.3, 0.4], [0.5, 0.6, 0.7, 0.8]]},
                ],
            )
        ],
    )
    def test_insert(
        self, infinity_runner: InfinityRunner, config: str, columns: dict, data: list
    ):
        insert_n = 100000
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

        def insert_func(table_obj):
            nonlocal cur_insert_n
            batch_size = 10

            while cur_insert_n < insert_n:
                try:
                    insert_data = data * int(batch_size / len(data))
                    table_obj.insert(insert_data)
                except Exception as e:
                    break
                cur_insert_n += batch_size

        def shutdown_func():
            nonlocal cur_insert_n
            last_shutdown_insert_n = cur_insert_n
            while True:
                if (
                    cur_insert_n >= insert_n
                    or cur_insert_n - last_shutdown_insert_n >= insert_n // stop_n
                ):
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    return
                time.sleep(0.1)

        while cur_insert_n < insert_n:
            infinity_runner.init(config)
            infinity_obj = InfinityRunner.connect(uri)

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table("test_insert")

            data_dict, data_type_dict = table_obj.output(["count(*)"]).to_result()
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
    vec = [1, 2, 3, 4]
    vec1 = [*vec] * 2
    print(vec1)
