import queue
import random
import time
import pytest
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from common import common_values
from infinity.common import ConflictType
from infinity import index
from infinity.errors import ErrorCode
from restart_util import *
from util import RtnThread


@pytest.mark.slow
class TestFullText:
    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_fulltext/1.toml",
            "test/data/config/restart_test/test_fulltext/2.toml",
            "test/data/config/restart_test/test_fulltext/3.toml",
        ],
    )
    def test_fulltext(self, infinity_runner: InfinityRunner, config: str):
        # should add symbolic link in advance
        enwiki_path = "test/data/benchmark/enwiki-10w.csv"
        enwiki_size = 100000

        table_name = "test_fulltext"
        gt_table_name = "test_fulltext_gt"
        matching_text = "American"
        search_after_insert = 2
        shutdown_interval = 10
        shutdown = False

        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory(config, uri, infinity_runner, shutdown_out=True)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            gt_table_obj = db_obj.create_table(
                gt_table_name,
                {
                    "id": {"type": "int"},
                    "title": {"type": "varchar"},
                    "date": {"type": "varchar"},
                    "body": {"type": "varchar"},
                },
                ConflictType.Error,
            )
            table_obj = db_obj.create_table(
                table_name,
                {
                    "id": {"type": "int"},
                    "title": {"type": "varchar"},
                    "date": {"type": "varchar"},
                    "body": {"type": "varchar"},
                },
                ConflictType.Error,
            )
            enwiki_gen1 = EnwikiGenerator.gen_factory(enwiki_path)(enwiki_size)
            for id, (title, date, body) in enumerate(enwiki_gen1):
                gt_table_obj.insert(
                    [{"id": id, "title": title, "date": date, "body": body}]
                )

            res = gt_table_obj.create_index(
                "ft_index",
                index.IndexInfo("body", index.IndexType.FullText),
                ConflictType.Error,
            )
            assert res.error_code == ErrorCode.OK

            res = table_obj.create_index(
                "ft_index",
                index.IndexInfo("body", index.IndexType.FullText),
                ConflictType.Error,
            )
            assert res.error_code == ErrorCode.OK

        part1()

        cur_insert_n = 0
        enwiki_gen2 = EnwikiGenerator.gen_factory(enwiki_path)(enwiki_size)
        end = False
        qu = queue.Queue()

        def work_func(table_obj, gt_table_obj):
            nonlocal cur_insert_n, end, qu
            while not end:
                r = random.randint(0, 500 - 1)
                try:
                    if r < 499:
                        res = next(enwiki_gen2, None)
                        if res is None:
                            print("End")
                            end = True
                            break
                        title, date, body = res
                        try:
                            table_obj.insert(
                                [
                                    {
                                        "id": cur_insert_n,
                                        "title": title,
                                        "date": date,
                                        "body": body,
                                    }
                                ]
                            )
                        except UnicodeDecodeError:
                            print(
                                "UnicodeDecodeError\n"
                            )  # unknown reason for this error, skip it
                            continue
                        cur_insert_n += 1
                    else:
                        gt_res = (
                            gt_table_obj.output(["id"])
                            .match_text(
                                fields="body",
                                matching_text=matching_text,
                                topn=10,
                                extra_options=None,
                            )
                            .filter("id<{}".format(cur_insert_n))
                            .to_result()
                        )

                        def t1():
                            print(f"search at {cur_insert_n}")

                            res = (
                                table_obj.output(["id"])
                                .match_text(
                                    fields="body",
                                    matching_text=matching_text,
                                    topn=10,
                                    extra_options=None,
                                )
                                .to_result()
                            )

                            data_dict, _, _ = res
                            gt_data_dict, _, _ = gt_res
                            if data_dict != gt_data_dict:
                                print(f"diff: {data_dict} {gt_data_dict}")
                            else:
                                print("same")

                        search_time = time.time() + search_after_insert
                        qu.put((t1, search_time))

                except Exception as e:
                    if not shutdown:
                        raise
                    break

        def search_thread():
            nonlocal qu, shutdown
            while not shutdown:
                try:
                    (f, search_time) = qu.get(timeout=1)
                except queue.Empty:
                    continue
                while time.time() < search_time:
                    time.sleep(0.1)
                try:
                    f()
                except Exception as e:
                    if not shutdown:
                        raise
                    break

        def shutdown_func():
            nonlocal shutdown
            shutdown = False
            time.sleep(shutdown_interval)

            shutdown = True
            infinity_runner.uninit()
            print(f"cur_insert_n: {cur_insert_n}")
            print("shutdown infinity")

        @decorator2
        def part2(infinity_obj):
            nonlocal shutdown

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            gt_table_obj = db_obj.get_table(gt_table_name)

            t1 = RtnThread(target=shutdown_func)
            t2 = RtnThread(target=search_thread)
            try:
                t1.start()
                t2.start()

                work_func(table_obj, gt_table_obj)

                t2.join()
                t1.join()
            except Exception:
                shutdown = True
                t2.join()
                t1.join()
                raise

        while not end:
            part2()

        @decorator
        def part3(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Error)
            db_obj.drop_table(gt_table_name, ConflictType.Error)

        part3()


if __name__ == "__main__":
    pass
