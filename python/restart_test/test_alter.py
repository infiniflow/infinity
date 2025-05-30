from functools import wraps
import pathlib
import time
import pytest
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode
import pandas as pd
from numpy import dtype
import infinity.index as index


class TestAlter:

    @pytest.mark.parametrize(
        "flush, flush_mid",
        [
            (False, False),
            (True, False),
            (True, True),
        ],
    )
    def test_alter_complex(
        self, infinity_runner: InfinityRunner, flush: bool, flush_mid: bool
    ):
        config = "test/data/config/restart_test/test_alter/1.toml"
        table_name = "test_alter2"

        infinity_runner.clear()
        uri = common_values.TEST_LOCAL_HOST

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(
                table_name,
                {
                    "c1": {"type": "int"},
                    "c2": {"type": "int"},
                    "c3": {"type": "varchar"},
                },
            )
            res = table_obj.create_index(
                "index1", index.IndexInfo("c3", index.IndexType.FullText)
            )
            assert res.error_code == ErrorCode.OK

            table_obj.insert([{"c1": 1, "c2": 2, "c3": "test"}])

            if flush and flush_mid:
                infinity_obj.flush_delta()

            res = table_obj.add_columns({"c4": {"type": "varchar", "default": "tttt"}})
            assert res.error_code == ErrorCode.OK

            res = table_obj.drop_columns(["c2"])
            assert res.error_code == ErrorCode.OK

            res = table_obj.add_columns({"c2": {"type": "float", "default": 0.0}})
            assert res.error_code == ErrorCode.OK

            table_obj.insert([{"c1": 2, "c3": "test2", "c4": "tttt2", "c2": 1.0}])

            res = table_obj.add_columns({"c5": {"type": "int", "default": 0}})
            assert res.error_code == ErrorCode.OK

            if flush:
                infinity_obj.flush_delta()

        @decorator
        def part2(infinity_obj):
            time.sleep(1) # wait some time or match_text is not available

            db_obj = infinity_obj.get_database("default_db")
            table_obj = db_obj.get_table(table_name)

            data_dict, _, _ = (
                table_obj.output(["c1"])
                .match_text(fields="c3", matching_text="test", topn=1)
                .to_result()
            )
            assert data_dict == {"c1": [1]}

            db_obj.drop_table(table_name)

        part1()
        part2()
