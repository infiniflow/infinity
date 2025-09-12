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
        import_num = 1000

        @decorator
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table("test_compact", ConflictType.Ignore)
            table_obj = db_obj.create_table("test_compact", columns)
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            table_obj.import_data(import_file, {"file_type": "jsonl"})
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == import_num * 4
            assert len(table_obj.show_segments()) == 4
            infinity_obj.flush_data()
            table_obj.compact()
            for index_info in indexes:
                table_obj.create_index(f"idx_{index_info.column_name}", index_info)

            infinity_obj.cleanup()

        part1()

        @decorator
        def part2(infinity_obj):
            table_obj = infinity_obj.get_database("default_db").get_table("test_compact")
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == import_num * 4
            assert len(table_obj.show_segments()) == 1

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
            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()

            infinity_obj.flush_data()

            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()
            infinity_obj.flush_data()

            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.import_data(dataset_path, import_options)
            table_obj.compact()

        part1()
        import_time = 10

        @decorator
        def part2(infinity_obj):
            table_obj = infinity_obj.get_database("default_db").get_table(table_name)
            data_dict, data_type_dict, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == 9 * import_time
            assert len(table_obj.show_segments()) == 1

        part2()

    def test_compact_restart_repeatedly(self, infinity_runner: InfinityRunner):
        config1 = "test/data/config/restart_test/test_compact/1.toml"
        config2 = "test/data/config/restart_test/test_compact/2.toml"

        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        decorator1 = infinity_runner_decorator_factory(config1, uri, infinity_runner)
        decorator2 = infinity_runner_decorator_factory(config2, uri, infinity_runner)

        table_name = "test_compact2"
        import_path = "test/data/csv/embedding_int_dim3.csv"
        import_num = 1000
        import_options = None
        kill_num = 1
        file_lines = 3

        @decorator1
        def part1(infinity_obj):
            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name, ConflictType.Ignore)
            table_obj = db_obj.create_table(
                table_name,
                {
                    "col1": {"type": "int"},
                    "col2": {"type": "vector, 3, float"},
                },
            )

        part1()

        import_n = 0

        @decorator2
        def part2(infinity_obj):
            nonlocal import_n
            table_obj = infinity_obj.get_database("default_db").get_table(table_name)
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == import_n * file_lines

            # Background compact is executed regularly
            for i in range(import_num):
                table_obj.import_data(import_path, import_options)
                import_n += 1

        for i in range(kill_num):
            part2()

        @decorator1
        def part3(infinity_obj):
            nonlocal import_n
            table_obj = infinity_obj.get_database("default_db").get_table(table_name)
            data_dict, _, _ = table_obj.output(["count(*)"]).to_result()
            count_star = data_dict["count(star)"][0]
            assert count_star == import_n * file_lines

            db_obj = infinity_obj.get_database("default_db")
            db_obj.drop_table(table_name)

        part3()