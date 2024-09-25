import os
import time
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
import pytest
from common import common_values
from infinity.common import ConflictType
from restart_util import *
import infinity
import pathlib


# Test with cleanuped data but meta data not cleanuped
class TestCleanup:
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
    def test_cleanuped_data(
        self,
        infinity_runner: InfinityRunner,
        columns: dict,
        data_gen_factory,
    ):
        config = "test/data/config/restart_test/test_cleanup/1.toml"
        data_dir = "/var/infinity/data"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        table_name = "test_cleanup"
        table_name2 = "test2_cleanup"

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

            infinity_obj.cleanup()

            # check
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*{table_name}*")
            assert len(list(dropped_dirs)) == 0

            db_obj.drop_table(table_name2, ConflictType.Error)

        part1()

        @decorator
        def part2(infinity_obj):
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*{table_name2}*")
            assert len(list(dropped_dirs)) == 0

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
        index_name = "idx1"

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

            data_gen = data_gen_factory(insert_n)

            for data in data_gen:
                data_line = {}
                for column_name, column_data in zip(columns.keys(), data):
                    data_line[column_name] = column_data
                table_obj.insert([data_line])

            res = table_obj.drop_index(index_name)
            assert res.error_code == infinity.ErrorCode.OK

            infinity_obj.cleanup()

            # check
            dropped_dirs = pathlib.Path(data_dir).rglob(f"*{index_name}*")
            assert len(list(dropped_dirs)) == 0

        part1()

        @decorator
        def part2(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)
            data_dict, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == insert_n

            db_obj.drop_table(table_name, ConflictType.Error)

            infinity_obj.cleanup()

            dropped_dirs = pathlib.Path(data_dir).rglob(f"*{table_name}*")
            assert len(list(dropped_dirs)) == 0

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
            table_obj.create_index(drop_index_name, index.IndexInfo("c1", index.IndexType.FullText))
            table_obj.create_index("idx2", index.IndexInfo("c2", index.IndexType.FullText))

            res = (
                table_obj.output(["c1"])
                .match_text(fields="c1", matching_text="text1", topn=1)
                .to_result()
            )

            table_obj.drop_index(drop_index_name)

            infinity_obj.cleanup()
            dropped_index_dirs = pathlib.Path("/var/infinity/data").rglob(f"*{drop_index_name}*")
            assert len(list(dropped_index_dirs)) == 0

            db_obj.drop_table(table_name)

        part1()