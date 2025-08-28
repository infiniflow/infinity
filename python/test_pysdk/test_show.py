import importlib
import sys
import os
import pytest
import polars as pl
import infinity.index as index
from common import common_values
from infinity.common import ConflictType, InfinityException
from infinity.errors import ErrorCode
import infinity

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")


@pytest.fixture(scope="class")
def setup_class(request, http):
    if http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()


@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    def test_show_databases(self, suffix):
        db_count = 100

        for i in range(db_count):
            self.infinity_obj.drop_database('test_show_database' + str(i) + suffix, ConflictType.Ignore)

        for i in range(db_count):
            db = self.infinity_obj.create_database('test_show_database' + str(i) + suffix)
            assert db

        dbs = self.infinity_obj.list_databases()
        res_dbs = []
        for db_name in dbs.db_names:
            if (db_name.startswith("test_show_database") and db_name.endswith(suffix)) or db_name == "default_db":
                res_dbs.append(db_name)
        assert len(res_dbs) == (db_count + 1)
        res_dbs.sort()
        assert res_dbs[0] == "default_db"

        for i in range(db_count):
            res = self.infinity_obj.drop_database('test_show_database' + str(i) + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    @pytest.mark.slow
    def test_show_big_databases(self, suffix):
        db_count = 10000

        for i in range(db_count):
            self.infinity_obj.drop_database('test_show_big_databases' + str(i) + suffix, ConflictType.Ignore)

        for i in range(db_count):
            db = self.infinity_obj.create_database('test_show_big_databases' + str(i) + suffix)
            assert db

        dbs = self.infinity_obj.list_databases()
        res_dbs = []
        for db_name in dbs.db_names:
            if (db_name.startswith("test_show_big_databases") and db_name.endswith(suffix)) or db_name == "default_db":
                res_dbs.append(db_name)
        assert len(res_dbs) == (db_count + 1)

        for i in range(db_count):
            res = self.infinity_obj.drop_database('test_show_big_databases' + str(i) + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    def test_show_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table" + suffix, ConflictType.Ignore)
        db_obj.create_table(
            "test_show_table" + suffix,
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)

        res = db_obj.show_table("test_show_table" + suffix)
        assert res.error_code == ErrorCode.OK
        assert res.database_name == "default_db"
        assert res.table_name == "test_show_table" + suffix
        assert res.column_count == 3
        assert res.segment_count == 0

        res = db_obj.drop_table("test_show_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("table_name", [pytest.param("Invalid name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    def test_show_invalid_table(self, table_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table" + suffix, ConflictType.Ignore)
        db_obj.create_table("test_show_table" + suffix, {"c1": {"type": "int"}, "c2": {"type": "vector,3,int"}},
                            ConflictType.Error)

        with pytest.raises(InfinityException) as e:
            db_obj.show_table(table_name)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_TABLE_NAME or e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        res = db_obj.drop_table("test_show_table" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("table_name", [pytest.param("not_exist_name")])
    def test_show_not_exist_table(self, table_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        with pytest.raises(InfinityException) as e:
            db_obj.show_table(table_name)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    def test_show_index(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK
            res = table_obj.show_index("my_index_" + str(i))
            assert res.error_code == ErrorCode.OK
            assert res.index_type == "HNSW"

        for i in range(10):
            res = table_obj.drop_index(
                "my_index_" + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_show_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("index_name", [pytest.param("Invalid name"),
                                            pytest.param("not_exist_name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    def test_show_invalid_name_index(self, index_name, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_invalid_name_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_invalid_name_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.create_index("my_index",
                                     index.IndexInfo("c1",
                                                     index.IndexType.Hnsw,
                                                     {
                                                         "m": "16",
                                                         "ef_construction": "50",
                                                         "metric": "l2"
                                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        with pytest.raises(InfinityException) as e:
            table_obj.show_index(index_name)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_INDEX_NAME or e.value.args[0] == ErrorCode.INDEX_NOT_EXIST

        res = db_obj.drop_table(
            "test_show_invalid_name_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_list_index(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_list_index" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_list_index" + suffix,
            {"c1": {"type": "vector,1024,float"}}, ConflictType.Error)
        assert table_obj is not None
        for i in range(10):
            res = table_obj.create_index("my_index_" + str(i),
                                         index.IndexInfo("c1",
                                                         index.IndexType.Hnsw,
                                                         {
                                                             "m": "16",
                                                             "ef_construction": "50",
                                                             "metric": "l2"
                                                         }), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = table_obj.list_indexes()
        assert res.error_code == ErrorCode.OK
        assert res.index_names == ["my_index_" + str(i) for i in range(10)]

        for i in range(10):
            res = table_obj.drop_index(
                "my_index_" + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table("test_list_index" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_show_columns(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_columns" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_columns" + suffix,
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = table_obj.show_columns()
            print(res)
            assert res.columns == ["name", "type", "default", "comment"]
            assert len(res) == 3
            assert res["name"][0] == "num"
            assert res["type"][0] == "Integer"
            assert res["default"][0] == "Null"
            assert res["comment"][0] == ""

        res = db_obj.drop_table("test_show_columns" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_show_columns_with_comment(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_columns" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table(
            "test_show_columns" + suffix,
            {
                "num": {"type": "integer", "default": 0},
                "body": {
                    "type": "varchar",
                    "default": "default text",
                    "comment": "comment2",
                },
                "vec": {"type": "vector,5,float", "comment": "comment3"},
            },
            ConflictType.Error,
        )
        with pl.Config(fmt_str_lengths=1000):
            res: pl.DataFrame = table_obj.show_columns()
            print(res)
            # check the polars dataframe
            expected_data = [
                {"name": "num", "type": "Integer", "default": "0", "comment": ""},
                {
                    "name": "body",
                    "type": "Varchar",
                    "default": "default text",
                    "comment": "comment2",
                },
                {
                    "name": "vec",
                    "type": "Embedding(float,5)",
                    "default": "Null",
                    "comment": "comment3",
                },
            ]
            expected_df = pl.DataFrame(expected_data)
            print(expected_df)
            assert res.equals(expected_df)

        res = db_obj.drop_table("test_show_columns" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_show_current_node(self, suffix):
        res = self.infinity_obj.show_current_node()
        assert res.error_code == ErrorCode.OK
        print(res)

    @pytest.mark.usefixtures("skip_if_remote_infinity")
    def test_show_global_variable(self, suffix):
        var: dict = self.infinity_obj.show_global_variable("profile")
        assert var["error_code"] == ErrorCode.OK
        assert "profile" in var

        var = self.infinity_obj.show_global_variable("result_cache")
        assert var["error_code"] == ErrorCode.OK
        assert "result_cache" in var

        try:
            var = self.infinity_obj.show_global_variable("invalid_variable")
        except Exception as e:
            assert e.error_code == ErrorCode.NO_SUCH_SYSTEM_VAR
        else:
            raise Exception("Should raise exception")