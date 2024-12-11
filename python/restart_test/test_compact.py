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

    def test_restart_compact_index(self, infinity_runner: InfinityRunner):
        config = "test/data/config/restart_test/test_compact/1.toml"
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator = infinity_runner_decorator_factory(config, uri, infinity_runner)

        table_name = "test_compact1"
        dataset_path = "test/data/csv/enwiki_9.csv"
        import_options = {"delimiter": "\t", "file_type": "csv"}

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
            )
            table_obj.create_index(
                "ft_index", index.IndexInfo("body", index.IndexType.FullText)
            )
            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()

            infinity_obj.flush_data()

            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()
            infinity_obj.flush_delta()

            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()


        part1()
        import_time = 4

        @decorator
        def part2(infinity_obj):
            table_obj = infinity_obj.get_database("default_db").get_table(table_name)
            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == 9 * import_time

        part2()
