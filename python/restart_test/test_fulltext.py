import queue
import random
import threading
import time
import pytest
import csv
from infinity_runner import InfinityRunner
from common import common_values
from infinity.common import ConflictType
from infinity import index
from infinity.errors import ErrorCode


@pytest.mark.slow
class TestFullText:
    def parse_enwiki(self, enwiki_path: str):
        with open(enwiki_path, "r") as f:
            reader = csv.reader(f, delimiter="\t")
            for row in reader:
                title = row[0]
                date = row[1]
                body = row[2]
                yield title, date, body

    @pytest.mark.skip(reason="not tested")
    @pytest.mark.parametrize(
        "config",
        [
            "test/data/config/restart_test/test_fulltext/1.toml",
            # "test/data/config/restart_test/test_fulltext/2.toml",
            # "test/data/config/restart_test/test_fulltext/3.toml",
        ],
    )
    def test_fulltext(self, infinity_runner: InfinityRunner, config: str):
        enwiki_path = "test/data/csv/enwiki-10w.csv"
        table_name = "test_fulltext"
        gt_table_name = "test_fulltext_gt"
        matching_text = "American"
        search_after_insert = 2
        shutdown_interval = 10
        shutdown = False

        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
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
        for id, (title, date, body) in enumerate(self.parse_enwiki(enwiki_path)):
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

        infinity_obj.disconnect()
        infinity_runner.uninit()

        cur_insert_n = 0
        enwiki_gen = self.parse_enwiki(enwiki_path)
        end = False

        def work_func(table_obj, gt_table_obj):
            nonlocal cur_insert_n
            nonlocal end
            while True:
                r = random.randint(0, 500 - 1)
                try:
                    if r < 499:
                        res = next(enwiki_gen, None)
                        if res is None:
                            print("End")
                            end = True
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
                        print(f"search at {cur_insert_n}")

                        result_queue = queue.Queue()

                        def wait_and_search():
                            time.sleep(search_after_insert)
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
                            result_queue.put(res)

                        t1 = threading.Thread(target=wait_and_search)
                        t1.start()

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
                        t1.join()
                        res = result_queue.get()

                        data_dict, _ = res
                        gt_data_dict, _ = gt_res
                        assert data_dict == gt_data_dict
                        print(data_dict)

                except Exception as e:
                    print(e)
                    assert shutdown == True
                    break

        def shutdown_func():
            nonlocal shutdown
            shutdown = True
            time.sleep(shutdown_interval)

            infinity_runner.uninit()
            print(f"cur_insert_n: {cur_insert_n}")
            print("shutdown infinity")
            assert shutdown == False
            shutdown = True
            return

        while not end:
            infinity_runner.init(config)
            infinity_obj = InfinityRunner.connect(uri)

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            gt_table_obj = db_obj.get_table(gt_table_name)

            t1 = threading.Thread(target=shutdown_func)
            t1.start()
            work_func(table_obj, gt_table_obj)
            t1.join()

            infinity_runner.uninit()

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table(table_name, ConflictType.Error)
        db_obj.drop_table(gt_table_name, ConflictType.Error)
        infinity_obj.disconnect()
        infinity_runner.uninit()


if __name__ == "__main__":
    pass
