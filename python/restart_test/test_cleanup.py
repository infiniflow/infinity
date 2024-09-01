import time
from infinity_runner import InfinityRunner
import pytest
from common import common_values
from infinity.common import ConflictType
from restart_util import *
import infinity


# Test with cleanuped data but meta data not cleanuped
class TestCleanup:
    @pytest.mark.parametrize(
        "config",
        ["test/data/config/restart_test/test_cleanup/1.toml"],
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
        ],
    )
    def test_cleanuped_data(
        self,
        infinity_runner: InfinityRunner,
        config: str,
        columns: dict,
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        table_name = "test_cleanup"

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)

        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, columns, ConflictType.Error)

        insert_n = 100
        data_gen = data_gen_factory(insert_n)

        for data in data_gen:
            data_line = {}
            for column_name, column_data in zip(columns.keys(), data):
                data_line[column_name] = column_data
            table_obj.insert([data_line])

        db_obj.drop_table(table_name, ConflictType.Error)

        time.sleep(2)  # sleep 2 wait for the data to be cleaned up

        infinity_obj.disconnect()
        infinity_runner.uninit()

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)

        db_obj = infinity_obj.get_database("default_db")
        try:
            table_obj = db_obj.get_table(table_name)
            assert False
        except infinity.InfinityException as e:
            assert e.error_code == infinity.ErrorCode.TABLE_NOT_EXIST

        infinity_obj.disconnect()
        infinity_runner.uninit()

    @pytest.mark.parametrize(
        "config",
        ["test/data/config/restart_test/test_cleanup/1.toml"],
    )
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
        config: str,
        columns: dict,
        indexes: list[index.IndexInfo],
        data_gen_factory,
    ):
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        table_name = "test_cleanup_index"

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)

        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, columns, ConflictType.Error)

        for idx in indexes:
            res = table_obj.create_index("idx1", idx)
            assert res.error_code == infinity.ErrorCode.OK

        insert_n = 100
        data_gen = data_gen_factory(insert_n)

        for data in data_gen:
            data_line = {}
            for column_name, column_data in zip(columns.keys(), data):
                data_line[column_name] = column_data
            table_obj.insert([data_line])

        res = table_obj.drop_index("idx1")
        assert res.error_code == infinity.ErrorCode.OK

        time.sleep(2)  # sleep 2 wait for the data to be cleaned up

        infinity_obj.disconnect()
        infinity_runner.uninit()

        infinity_runner.init(config)
        infinity_obj = InfinityRunner.connect(uri)

        db_obj = infinity_obj.get_database("default_db")
        table_obj = db_obj.get_table(table_name)
        data_dict, _ = table_obj.output(["count(*)"]).to_result()
        count_star = data_dict["count(star)"][0]
        assert count_star == insert_n

        db_obj.drop_table(table_name, ConflictType.Error)

        infinity_obj.disconnect()
        infinity_runner.uninit()