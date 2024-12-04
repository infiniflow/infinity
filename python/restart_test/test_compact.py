from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
from common import common_values
from restart_util import *
from infinity.common import ConflictType


class TestCompact:
    def test_restart_after_compact_and_cleanup(self, infinity_runner: InfinityRunner):
        config = "test/data/config/restart_test/test_compact/1.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        columns = LChYDataGenerato.columns()
        indexes = LChYDataGenerato.index()
        import_file = LChYDataGenerato.import_file()

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_compact", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_compact", columns)
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            infinity_obj.flush_delta()
            table_obj.compact()
            for index_info in indexes:
                table_obj.create_index(f"idx_{index_info.column_name}", index_info)

            infinity_obj.cleanup()

        part1()

        @decorator
        def part2(infinity_obj):
            pass

        part2()
