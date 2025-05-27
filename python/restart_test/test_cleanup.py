import os
import time
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
import pytest
from common import common_values
from infinity.common import ConflictType
from restart_util import *
import infinity
import pathlib
from infinity.errors import ErrorCode


# Test with cleanuped data but meta data not cleanuped
class TestCleanup:
    def file_num(self, path: str):
        cnt = 0
        for p in pathlib.Path(path).rglob("*"):
            if not p.is_dir():
                cnt += 1
        return cnt

    def files_num(self, path_list: list[str]):
        cnt = 0
        for path in path_list:
            cnt += self.file_num(path)
        return cnt

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
        ],
    )
    @pytest.mark.parametrize("cleanup_flush", [True, False])
    def test_cleanuped_data(
        self,
        infinity_runner: InfinityRunner,
        columns: dict,
        data_gen_factory,
        cleanup_flush: bool,
    ):
        config = "test/data/config/restart_test/test_cleanup/1.toml"
        data_dir = "/var/infinity/data"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        table_name = "test_cleanup"
        table_name2 = "test2_cleanup"
        table_id = 0
        table2_id = 1

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            db_obj.drop_table(table_name2, ConflictType.Ignore)
            table_obj = db_obj.create_table(table_name, columns, ConflictType.Error)
            table_obj2 = db_obj.create_table(table_name2, columns, ConflictType.Error)

            insert_n = 100
            data_gen = data_gen_factory(insert_n)

            for data in data_gen:
                data_line = {}
                for column_name, column_data in zip(columns.keys(), data):
                    data_line[column_name] = column_data
                table_obj.insert([data_line])
                table_obj2.insert([data_line])

            db_obj.drop_table(table_name, ConflictType.Error)

            if cleanup_flush:
                infinity_obj.cleanup()
                infinity_obj.flush_data()
            else:
                infinity_obj.flush_data()
                infinity_obj.cleanup()

            # check
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*tbl_{table_id}*")
            assert len(list(dropped_dirs)) == 0

            dropped_dirs = pathlib.Path(data_dir).rglob(f"*tbl_{table2_id}*")
            assert len(list(dropped_dirs)) == 1

            db_obj.drop_table(table_name2, ConflictType.Error)

        part1()

        @decorator
        def part2(infinity_obj):
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*tbl_{table2_id}*")
            if len(list(dropped_dirs)) != 0:
                infinity_runner.logger.warning(
                    f"cleanup {table_name2} failed. Not implemented"
                )
            # assert len(list(dropped_dirs)) == 0

        part2()

    @pytest.mark.parametrize(
        "columns, indexes, data_gen_factory",
        [
            (
                SimpleEmbeddingGenerator.columns(),
                SimpleEmbeddingGenerator.index(),
                SimpleEmbeddingGenerator.gen_factory(),
            ),
            (
                SimpleVarcharGenerator.columns(),
                SimpleVarcharGenerator.index(),
                SimpleVarcharGenerator.gen_factory(),
            ),
        ],
    )
    def test_cleanuped_index(
        self,
        infinity_runner: InfinityRunner,
        columns: dict,
        indexes: list[index.IndexInfo],
        data_gen_factory,
    ):
        config = "test/data/config/restart_test/test_cleanup/1.toml"
        data_dir = "/var/infinity/data"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        table_name = "test_cleanup_index"
        index_name = "idx0"
        index_name2 = "idx1"
        table_id = 0
        index_id = 1
        index2_id = 2

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)
        insert_n = 100

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(table_name, columns, ConflictType.Error)

            for idx in indexes:
                res = table_obj.create_index(index_name, idx)
                assert res.error_code == infinity.ErrorCode.OK

                res = table_obj.create_index(index_name2, idx)
                assert res.error_code == infinity.ErrorCode.OK

            data_gen = data_gen_factory(insert_n)

            for data in data_gen:
                data_line = {}
                for column_name, column_data in zip(columns.keys(), data):
                    data_line[column_name] = column_data
                table_obj.insert([data_line])

            res = table_obj.dump_index(index_name)
            res = table_obj.dump_index(index_name2)

            assert res.error_code == infinity.ErrorCode.OK
            res = table_obj.drop_index(index_name)
            assert res.error_code == infinity.ErrorCode.OK

            infinity_obj.cleanup()

            # check
            dropped_dirs = []
            dropped_dirs_gen = pathlib.Path(data_dir).rglob(f"*idx_{index_id}*")
            for dir in dropped_dirs_gen:
                dropped_dirs.append(dir)

            assert self.files_num(dropped_dirs) == 0

            dropped_dirs = []
            dropped_dirs_gen = pathlib.Path(data_dir).rglob(f"*idx_{index2_id}*")
            for dir in dropped_dirs_gen:
                dropped_dirs.append(dir)

            assert self.files_num(dropped_dirs) > 0

            res = table_obj.drop_index(index_name2)

        part1()

        @decorator
        def part2(infinity_obj):
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*idx_{index2_id}*")
            if self.files_num(list(dropped_dirs)) != 0:
                infinity_runner.logger.warning(
                    f"cleanup {index_name2} failed. Not implemented"
                )

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == insert_n

            db_obj.drop_table(table_name, ConflictType.Error)

            infinity_obj.cleanup()

            dropped_dirs = pathlib.Path(data_dir).rglob(f"*tbl_{table_id}*")
            assert self.files_num(list(dropped_dirs)) == 0

        part2()

    def test_invalidate_fulltext_cache(self, infinity_runner: InfinityRunner):
        table_name = "test_invalid_fulltext_cache"
        config = "test/data/config/restart_test/test_cleanup/1.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(
                table_name,
                {
                    "c1": {"type": "varchar"},
                    "c2": {"type": "varchar"},
                },
            )
            table_obj.insert([{"c1": "text1", "c2": "text2"}])

            drop_index_name = "idx1_todrop"
            drop_index_id = 0

            table_obj.create_index(
                drop_index_name, index.IndexInfo("c1", index.IndexType.FullText)
            )
            table_obj.create_index(
                "idx2", index.IndexInfo("c2", index.IndexType.FullText)
            )

            res = (
                table_obj.output(["c1"])
                .match_text(fields="c1", matching_text="text1", topn=1)
                .to_result()
            )

            table_obj.drop_index(drop_index_name)

            infinity_obj.cleanup()
            dropped_index_dirs = pathlib.Path("/var/infinity/data").rglob(
                f"*idx_{drop_index_id}*"
            )
            assert self.files_num(list(dropped_index_dirs)) == 0

            db_obj.drop_table(table_name)

        part1()

    def test_invalidate_fulltext_cache2(self, infinity_runner: InfinityRunner):
        table_name = "test_invalid_fulltext_cache2"
        index_name = "body_index"

        data_dir = "/var/infinity/data"
        config = "test/data/config/restart_test/test_cleanup/2.toml"
        uri = common_values.TEST_LOCAL_HOST
        import_file = "test/data/csv/enwiki_9.csv"
        abs_import_file = os.path.abspath(import_file)

        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(
                table_name,
                {
                    "doctitle": {"type": "varchar"},
                    "docdate": {"type": "varchar"},
                    "body": {"type": "varchar"},
                },
                ConflictType.Error,
            )
            res = table_obj.create_index(
                index_name, index.IndexInfo("body", index.IndexType.FullText)
            )
            assert res.error_code == ErrorCode.OK

            # compaction need 4 segment in default config
            for i in range(4):
                table_obj.import_data(abs_import_file, {"delimiter": "\t"})

            res = table_obj.output(["*"]).match_text("body^5", "America", 3).to_pl()

            # wait for compaction
            time.sleep(3)

            infinity_obj.cleanup()
            time.sleep(1)

            index_id = 1
            dropped_index_dirs = []
            dropped_index_dirs_gen = pathlib.Path(data_dir).rglob(f"*idx_{index_id}*")
            for dir in dropped_index_dirs_gen:
                dropped_index_dirs.append(dir)

            assert self.files_num(dropped_index_dirs) == 3

            db_obj.drop_table(table_name)

        part1()
