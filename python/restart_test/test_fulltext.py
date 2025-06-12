import queue
import time
import pytest
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory, infinity_runner_decorator_factory2
from common import common_values
from infinity.common import ConflictType
from infinity import index
from infinity.errors import ErrorCode
from restart_util import *
from util import RtnThread
import pickle
import logging


class TestFullText:
    @pytest.mark.slow
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
        enwiki_size = 10000

        table_name = "test_fulltext"
        gt_table_name = "test_fulltext_gt"
        matching_text = "American"
        search_after_insert = 2
        shutdown = False

        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory2(
            config, uri, infinity_runner, shutdown_out=True
        )

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
            insert_n = 0
            for id, (title, date, body) in enumerate(enwiki_gen1):
                insert_n += 1
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

            gt_res = (
                gt_table_obj.output(["id"])
                    .match_text(
                    fields="body",
                    matching_text=matching_text,
                    topn=10,
                    extra_options=None,
                )
                .to_result()
            )
            gt_data_dict, _, _ = gt_res
            logging.info(f"part1 insert_n: {insert_n}, gt_data_dict: {gt_data_dict}")

        part1()

        cur_insert_n = 0
        enwiki_gen2 = EnwikiGenerator.gen_factory(enwiki_path)(enwiki_size)
        end = False
        qu = queue.Queue()

        def work_func(table_obj, table_obj2, gt_table_obj):
            nonlocal cur_insert_n, end, shutdown, qu
            it = 0
            while not end and not shutdown:
                try:
                    it += 1
                    if it % 500 != 0:
                        res = next(enwiki_gen2, None)
                        if res is None:
                            logging.info("enwiki_gen2 end")
                            end = True
                            # Put a special item to notify search thread to quit
                            qu.put((None, None, time.time()))
                            break
                        title, date, body = res
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
                        gt_data_dict, _, _ = gt_res
                        logging.info(f"work_func cur_insert_n: {cur_insert_n}, gt_data_dict: {gt_data_dict}")

                        def t1(gt_data_dict):
                            logging.info(f"search at {cur_insert_n}")

                            res = (
                                table_obj2.output(["id"])
                                .match_text(
                                    fields="body",
                                    matching_text=matching_text,
                                    topn=10,
                                    extra_options=None,
                                )
                                .filter("id<{}".format(cur_insert_n))
                                .to_result()
                            )
                            logging.info(f"search at {cur_insert_n} done")

                            data_dict, _, _ = res
                            if data_dict != gt_data_dict:
                                logging.info(f"diff: {data_dict} {gt_data_dict}")
                            else:
                                logging.info("same")

                        search_time = time.time() + search_after_insert
                        qu.put((t1, gt_data_dict, search_time))

                except Exception:
                    logging.exception("work_func exception, cur_insert_n: {cur_insert_n}")
                    shutdown = True
                    break

        def search_thread():
            nonlocal qu, shutdown
            while not shutdown:
                try:
                    (f, gt_data_dict, search_time) = qu.get(timeout=1)
                except queue.Empty:
                    continue
                while time.time() < search_time:
                    time.sleep(0.1)
                if f is None:
                    infinity_runner.uninit()
                    logging.info(f"shutdown infinity cur_insert_n: {cur_insert_n}")
                    break
                try:
                    f(gt_data_dict)
                except Exception:
                    logging.exception("search_thread got exception")
                    shutdown = True
                    break

        @decorator2
        def part2(infinity_pool):
            nonlocal shutdown
            infinity_obj = infinity_pool.get_conn()
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            gt_table_obj = db_obj.get_table(gt_table_name)
            infinity_obj2 = infinity_pool.get_conn()
            db_obj2 = infinity_obj2.get_database("default_db")
            table_obj2 = db_obj2.get_table(table_name)

            t2 = RtnThread(target=search_thread)
            try:
                t2.start()

                work_func(table_obj, table_obj2, gt_table_obj)

                t2.join()
            except Exception:
                logging.exception("part2 got exception")
                shutdown = True
                t2.join()
                raise
            finally:
                infinity_pool.release_conn(infinity_obj)

        while not end:
            part2()

        @decorator
        def part3(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Error)
            db_obj.drop_table(gt_table_name, ConflictType.Error)

        part3()

    def test_fulltext_realtime(self, infinity_runner: InfinityRunner):
        enwiki_path = "test/data/csv/enwiki_9999.csv"
        enwiki_size = 10000
        config = "test/data/config/restart_test/test_fulltext/1.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        matching_text = "American"
        test_num = 100

        @decorator
        def get_gt_list(infinity_obj):
            gt_res_list = []
            gt_table_name = "test_fulltext_gt"
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(gt_table_name, ConflictType.Ignore)

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
            enwiki_gen1 = EnwikiGenerator.gen_factory(enwiki_path)(enwiki_size)
            for id, (title, date, body) in enumerate(enwiki_gen1):
                gt_table_obj.insert(
                    [{"id": id, "title": title, "date": date, "body": body}]
                )
            gt_table_obj.create_index(
                "ft_index",
                index.IndexInfo("body", index.IndexType.FullText),
                ConflictType.Error,
            )

            for i in range(test_num):
                bound = enwiki_size // test_num * (i + 1)
                gt_res = (
                    gt_table_obj.output(["id"])
                    .match_text(
                        fields="body",
                        matching_text=matching_text,
                        topn=3,
                        extra_options=None,
                    )
                    .filter("id<{}".format(bound))
                    .to_result()
                )
                gt_data_dict, _, _ = gt_res
                gt_res_list.append(gt_data_dict)

            db_obj.drop_table(gt_table_name, ConflictType.Error)
            return gt_res_list

        pickle_dir = "tmp"
        if not os.path.exists(pickle_dir):
            os.makedirs(pickle_dir)
        gt_filename = f"{pickle_dir}/gt_res_list_{enwiki_size}.pkl"
        if os.path.exists(gt_filename):
            with open(gt_filename, "rb") as f:
                gt_res_list = pickle.load(f)
        else:
            gt_res_list = get_gt_list()
            with open(gt_filename, "wb") as f:
                pickle.dump(gt_res_list, f)

        @decorator
        def test(infinity_obj):
            table_name = "test_fulltext"
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            infinity_obj.flush_data()
            # infinity_obj.cleanup()
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
            # table_obj.drop_index("ft_index")
            table_obj.create_index(
                "ft_index",
                index.IndexInfo("body", index.IndexType.FullText, {"realtime": "true"}),
                ConflictType.Error,
            )

            enwiki_gen2 = EnwikiGenerator.gen_factory(enwiki_path)(enwiki_size)
            query_everytime = False
            for id, (title, date, body) in enumerate(enwiki_gen2):
                table_obj.insert(
                    [{"id": id, "title": title, "date": date, "body": body}]
                )
                if (id + 1) % (enwiki_size // test_num) == 0 or query_everytime:
                    gt_i = (id + 1) // (enwiki_size // test_num) - 1
                    res = (
                        table_obj.output(["id"])
                        .match_text(
                            fields="body",
                            matching_text=matching_text,
                            topn=3,
                            extra_options=None,
                        )
                        .to_result()
                    )
                    data_dict, _, _ = res
                    assert data_dict == gt_res_list[gt_i]

            db_obj.drop_table(table_name, ConflictType.Error)

        test()

    def test_fulltext_offline(self, infinity_runner: InfinityRunner):
        # should add symbolic link in advance
        enwiki_path = "test/data/csv/enwiki_9999.csv"
        enwiki_size = 1000

        table_name = "test_fulltext_offline"
        matching_text = "American"
        part1_data_dict = None

        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        config = "test/data/config/restart_test/test_fulltext/1.toml"
        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            nonlocal part1_data_dict
            db_obj = infinity_obj.get_database("default_db")
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
                table_obj.insert(
                    [{"id": id, "title": title, "date": date, "body": body}]
                )

            res = table_obj.create_index(
                "ft_index",
                index.IndexInfo("body", index.IndexType.FullText),
                ConflictType.Error,
            )
            assert res.error_code == ErrorCode.OK
            logging.info(f"part1 inserted {enwiki_size} rows and created index")
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
            part1_data_dict, _, _ = res
            logging.info(f"part1 data_dict: {part1_data_dict}")

        part1()

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
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
            part2_data_dict, _, _ = res
            logging.info(f"part2 data_dict: {part2_data_dict}")
            assert part1_data_dict == part2_data_dict

        part2()


if __name__ == "__main__":
    pass
