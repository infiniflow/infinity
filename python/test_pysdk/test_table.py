import sys
import os
import concurrent.futures
import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType, InfinityException
import infinity
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions
import random

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
    def test_create_table_with_valid_options(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_table_with_valid_options" + suffix, ConflictType.Ignore)

        for valid_option in common_values.create_valid_option:
            table_obj = db_obj.create_table("test_create_table_with_valid_options" + suffix, {"c1": {"type": "int"}},
                                            valid_option)
            assert table_obj is not None
            res = db_obj.drop_table("test_create_table_with_valid_options" + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    def test_drop_table_with_valid_options(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_table_with_valid_options" + suffix, ConflictType.Ignore)

        for valid_option in common_values.drop_valid_option:
            table_obj = db_obj.create_table("test_drop_table_with_valid_options" + suffix, {"c1": {"type": "int"}},
                                            ConflictType.Error)
            assert table_obj is not None
            res = db_obj.drop_table("test_drop_table_with_valid_options" + suffix, valid_option)
            assert res.error_code == ErrorCode.OK

    def test_table_with_valid_column_names(self, suffix):
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        table_name = "test_table_with_valid_column_names" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        for column_name in common_values.valid_name_array:
            table_obj = db_obj.create_table(table_name, {column_name: {"type": "int"}}, ConflictType.Error)
            assert table_obj is not None

            res = db_obj.show_table(table_name)
            assert res.error_code == ErrorCode.OK

            # drop table
            res = db_obj.drop_table(table_name)
            assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_create_table_with_invalid_column_name_python(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_table_with_invalid_column_name_python" + suffix, ConflictType.Ignore)

        for column_name in common_values.invalid_name_array:
            with pytest.raises(InfinityException) as e:
                db_obj.create_table("test_create_table_with_invalid_column_name_python" + suffix,
                                    {column_name: {"type": "int"}}, ConflictType.Error)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        res = db_obj.drop_table("test_create_table_with_invalid_column_name_python" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.TABLE_NOT_EXIST

    def test_create_table_with_invalid_options(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_table_with_invalid_options" + suffix, ConflictType.Ignore)

        for invalid_option in common_values.create_invalid_option:
            with pytest.raises(InfinityException) as e:
                db_obj.create_table("test_create_table_with_invalid_options" + suffix, {"c1": {"type": "int"}},
                                    invalid_option)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table("test_create_table_with_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.TABLE_NOT_EXIST

    def test_drop_table_with_invalid_options(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_table_with_invalid_options" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_drop_table_with_invalid_options" + suffix, {"c1": {"type": "int"}},
                                        ConflictType.Error)
        assert table_obj is not None

        for invalid_option in common_values.create_invalid_option:
            with pytest.raises(InfinityException) as e:
                db_obj.drop_table("test_drop_table_with_invalid_options" + suffix, invalid_option)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table("test_drop_table_with_invalid_options" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @trace_expected_exceptions
    def test_create_or_drop_same_table_in_different_thread(self, suffix):
        """
        target: create/drop same table in different thread to test conflict
        methods: create table at same time for 16 times
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_or_drop_same_table_in_different_thread" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        # create table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into processpool
            futures = [executor.submit(db_obj.create_table(table_name, {"c1": {"type": "int"}}, ConflictType.Ignore), i)
                       for i in
                       range(16)]
            # wait all processes finished
            concurrent.futures.wait(futures)

        # drop table
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as executor:
            # commit task into threadpool
            futures = [executor.submit(db_obj.drop_table(table_name, ConflictType.Ignore), i) for i in range(16)]
            # wait all threads finished
            concurrent.futures.wait(futures)

        # drop table
        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

    def test_create_empty_column_table(self, suffix):
        """
        target: create empty column table
        methods: create empty column table
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_empty_column_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            db_obj.create_table(table_name, {}, ConflictType.Error)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NO_COLUMN_DEFINED

    def test_create_valid_type(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_valid_type" + suffix, ConflictType.Ignore)

        for type in common_values.types_array:
            table_obj = db_obj.create_table("test_create_valid_type" + suffix, {"c1": {"type": type}},
                                            ConflictType.Error)
            assert table_obj is not None
            res = db_obj.drop_table("test_create_valid_type" + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK

        # disconnect
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_create_same_name_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_same_name" + suffix, ConflictType.Ignore)

        # create
        table_obj = db_obj.create_table("test_create_same_name" + suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        assert table_obj is not None

        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_create_same_name" + suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_same_name" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_drop_same_name_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")

        # create
        table_obj = db_obj.create_table("test_drop_same_name" + suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        assert table_obj is not None

        # drop
        res = db_obj.drop_table("test_drop_same_name" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # drop
        res = db_obj.drop_table("test_drop_same_name" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.TABLE_NOT_EXIST

    def test_same_column_name(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_same_column_name" + suffix, ConflictType.Ignore)

        # create table with same column name, dictionary cannot have same keys, the later item will override the previous one,
        # so the column definition sent to infinity will be {"c1": {"type": "float"}}
        table_obj = db_obj.create_table("test_same_column_name" + suffix, {"c1": {"type": "int"},
                                                                           "c1": {"type": "float"}}, ConflictType.Error)
        assert table_obj is not None

        res = table_obj.show_columns()
        print("\n")
        print(res)
        assert len(res) == 1
        assert res["name"][0] == "c1"
        assert res["type"][0] == "Float"

        res = db_obj.drop_table("test_same_column_name" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_table_with_various_column_types(self, suffix):
        column_number = 10000
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_various_column_types" + suffix, ConflictType.Ignore)

        columns = {}
        for i in range(column_number):
            col_name = f"c{i}"
            col_type = random.choice(common_values.types_array)
            columns[col_name] = {"type": col_type}

        table_obj = db_obj.create_table("test_table_with_various_column_types" + suffix, columns, ConflictType.Error)
        assert table_obj is not None

        # show table
        res = db_obj.show_table("test_table_with_various_column_types" + suffix)
        assert res.error_code == ErrorCode.OK

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        table_obj = db_obj.get_table("test_table_with_various_column_types" + suffix)
        assert table_obj is not None
        print(table_obj.output(["c2"]))

        res = db_obj.drop_table("test_table_with_various_column_types" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_ignore_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_ignore_option" + suffix, ConflictType.Ignore)

        for i in range(100):
            table_obj = db_obj.create_table("test_create_duplicated_table_with_ignore_option" + suffix,
                                            {"c1": {"type": "int"}},
                                            ConflictType.Ignore)
            assert table_obj is not None

        res = db_obj.drop_table("test_create_duplicated_table_with_ignore_option" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_error_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_error_option" + suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_error_option" + suffix, {"c1": {"type": "int"}},
                                    ConflictType.Error)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_duplicated_table_with_error_option" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_replace_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_replace_option" + suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_replace_option" + suffix,
                                    {"c" + str(i): {"type": "int"}},
                                    ConflictType.Replace)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NOT_SUPPORTED

        res = db_obj.drop_table("test_create_duplicated_table_with_replace_option" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.TABLE_NOT_EXIST

    def test_create_upper_table_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_lower_name = "test_table_my_table" + suffix.lower()
        db_obj.drop_table(table_lower_name, ConflictType.Ignore)

        table_upper_name = "TEST_TABLE_MY_TABLE" + suffix.upper()
        # create table
        tb = db_obj.create_table(
            table_upper_name, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        # get table
        table_obj = db_obj.get_table(table_lower_name)
        assert table_obj is not None
        table_obj = db_obj.get_table(table_upper_name)
        assert table_obj is not None

    def test_create_table_with_upper_column_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"C1": {"type": "int", "constraints": ["primary key"]}, "C2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        # get table
        table_obj = db_obj.get_table(table_name)

        res = table_obj.show_columns()
        print("\n")
        print(res)
        assert res["name"][0] == "c1"
        assert res["name"][1] == "c2"

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.usefixtures("skip_if_http")
    def test_create_table_with_upper_param_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"TYPE": "int", "CONSTRAINTS": ["primary key"]}, "c2": {"TYPE": "float"}},
            ConflictType.Error)
        assert tb is not None

        # get table
        table_obj = db_obj.get_table(table_name)

        res = table_obj.show_columns()
        print("\n")
        print(res)
        assert res["name"][0] == "c1"
        assert res["name"][1] == "c2"

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_table_with_upper_data_type_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "INT", "constraints": ["primary key"]}, "c2": {"type": "FLOAT"}},
            ConflictType.Error)
        assert tb is not None

        # get table
        table_obj = db_obj.get_table(table_name)

        res = table_obj.show_columns()
        print("\n")
        print(res)
        assert res["name"][0] == "c1"
        assert res["name"][1] == "c2"

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_table_with_upper_constraint_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "int", "constraints": ["PRIMARY KEY"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        # get table
        table_obj = db_obj.get_table(table_name)

        res = table_obj.show_columns()
        print("\n")
        print(res)
        assert res["name"][0] == "c1"
        assert res["name"][1] == "c2"

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def _test_table(self, suffix):
        """
        target: test table apis
        method:
        1. create tables
            - 'my_table'        √
                - c1 int primary key
                - c2 float
            - 'my_table!@#'     ❌
                - c1 int primary key
                - c2 float
            - 'my-table-dash'   ❌
                - c1 float primary key
                - c2 int
            - '123_table'       ❌
                - c1 int primary key
                - c2 float
            - 'bad_column'      ❌
                - 123 int primary key
                - c2 float
            - ''                ❌
                - c1 int primary key
                - c2 float
        2. list tables
            - 'my_table'
        3. drop tables
            - 'my_table
        4. list tables: empty
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        with pytest.raises(InfinityException) as e:
            db_obj.create_table(
                "my_table!@#", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.INVALID_IDENTIFIER_NAME

        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my-table-dash", {"c1": "float, primary key", "c2": "int"}, ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "123_table", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "bad_column", {"123": "int, primary key", "c2": "float"}, ConflictType.Error)
        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # show table
        res = db_obj.show_table(table_name)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

    def _test_show_tables(self, suffix):
        db = self.infinity_obj.get_database("default_db")

        with pl.Config(fmt_str_lengths=1000):
            res = db.show_tables()
            print(res)
            # check the polars dataframe
            assert res.columns == ["database", "table", "column_count", "block_count", "block_capacity",
                                   "segment_count", "segment_capacity", "comment"]

    def _test_create_varchar_table(self, suffix):
        """
        target: test create table with varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_varchar_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name,
                                        {"c1": {"type": "varchar", "constraints": ["primary key"]},
                                         "c2": {"type": "float"}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_embedding_table(self, suffix):
        """
        target: test create table with embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_embedding_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, {
            "c1": {"type": "vector,128,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_tensor_table(self, suffix):
        """
        target: test create table with tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_tensor_table" + suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, {
            "c1": {"type": "tensor,128,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_tensorarray_table(self, suffix):
        """
        target: test create table with tensorarray column
        method: create table with tensorarray column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_tensorarray_table" + suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_tensorarray_table" + suffix, {
            "c1": {"type": "tensorarray,128,float"}}, ConflictType.Error)
        assert table_obj is not None

        res = db_obj.drop_table("test_create_tensorarray_table" + suffix)
        assert res.error_code == ErrorCode.OK

    def _test_table_with_different_column_types(self, suffix):
        """
        target: test create/drop/show/get valid table name with different column types
        methods:
        1. create table
            - 'my_table'            √
                - c1 bool primary key
                - c2 int
                - c3 int8
                - c4 int16
                - c5 int32
                - c6 int64
                - c7 float
                - c8 float32
                - c9 double
                - c10 float64
                - c11 varchar
                - c12 integer
                - c13 int128 # invalid now
            - 'my_table'             ❌
                - c1 int!@#
             - 'my_table'            ❌
                - c1 ''
             - 'my_table'            ❌
                - c1 123int
             - 'my_table'            ❌
                - c1 int-varchar
             - 'my_table'            ❌
                - c1 ['int']
             - 'my_table'            ❌
                - c1 {"int"}
             - 'my_table'            ❌
                - c1 ("int")
        2. get table
        3. list table
        4. show table
        expect: all operations successfully

        """
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_different_column_types" + suffix, ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            "test_table_with_different_column_types" + suffix, {
                "c1": {"type": "bool", "constraints": ["primary key"]},
                "c2": {"type": "int"}, "c3": {"type": "int8"}, "c4": {"type": "int16"},
                "c5": {"type": "int32"}, "c6": {"type": "int64"}, "c7": {"type": "float"}, "c8": {"type": "float32"},
                "c9": {"type": "double"}, "c10": {"type": "float64"}, "c11": {"type": "varchar"},
                "c12": {"type": "integer"}}, ConflictType.Error)
        assert tb is not None

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        table_obj = db_obj.get_table("test_table_with_different_column_types" + suffix)
        assert table_obj is not None

        # show table
        res = db_obj.show_table("test_table_with_different_column_types" + suffix)
        assert res.error_code == ErrorCode.OK

        # drop table
        res = db_obj.drop_table("test_table_with_different_column_types" + suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def _test_create_drop_table(self, suffix):
        """
        target: create created table, drop dropped table
        methods: create table ,drop table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_drop_table" + suffix, ConflictType.Ignore)

        # create
        tb = db_obj.create_table(
            "test_create_drop_table" + suffix,
            {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        try:
            tb = db_obj.create_table("test_create_drop_table" + suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

        # drop
        db_obj.drop_table("test_create_drop_table" + suffix, ConflictType.Error)

        res = db_obj.drop_table("test_create_drop_table" + suffix, ConflictType.Error)
        res.error_code == ErrorCode.TABLE_NOT_EXIST

    def test_table(self, suffix):
        # self.test_infinity_obj._test_version()
        self._test_table(suffix)
        self._test_show_tables(suffix)
        self._test_create_varchar_table(suffix)
        self._test_create_embedding_table(suffix)
        self._test_create_tensor_table(suffix)
        self._test_create_tensorarray_table(suffix)
        self._test_create_drop_table(suffix)

    def test_after_disconnect_use_table(self, suffix):
        """
        target: after disconnection, create / drop / show / list / get table
        methods:
        1. disconnect database
        2. create / drop / show / list / get table
        expect: all operations successfully
        """
        # connect
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_after_disconnect_use_table" + suffix, ConflictType.Ignore)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # create table
        table_obj = db_obj.create_table("test_after_disconnect_use_table" + suffix, {"c1": {"type": "int"}},
                                        ConflictType.Error)
        assert table_obj is not None

        res = db_obj.drop_table("test_after_disconnect_use_table" + suffix, ConflictType.Error)
        res.error_code == ErrorCode.OK

        # show table
        with pytest.raises(InfinityException) as e:
            db_obj.show_table("test_after_disconnect_use_table" + suffix)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

        # list table
        res = db_obj.list_tables()
        res.error_code == ErrorCode.OK

        # get table
        with pytest.raises(InfinityException) as e:
            db_obj.get_table("test_after_disconnect_use_table" + suffix)
        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.TABLE_NOT_EXIST

    def test_create_1K_table(self, suffix):
        infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        tb_count = 1000

        for i in range(tb_count):
            db_obj.drop_table("test_create_1k_table" + str(i) + suffix, ConflictType.Ignore)

        for i in range(tb_count):
            table_obj = db_obj.create_table("test_create_1k_table" + str(i) + suffix, {"c1": {"type": "int"}},
                                            ConflictType.Error)
            assert table_obj is not None

        tbs = db_obj.list_tables()
        res_tbs = []
        for tb_name in tbs.table_names:
            if tb_name.startswith("test_create_1k") and tb_name.endswith(suffix):
                res_tbs.append(tb_name)
        assert len(res_tbs) == tb_count

        for i in range(tb_count):
            res = db_obj.drop_table("test_create_1k_table" + str(i) + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK

    @pytest.mark.slow
    def test_create_10k_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")

        tb_count = 10000
        for i in range(tb_count):
            db_obj.drop_table("test_create_10k_table" + str(i) + suffix, ConflictType.Ignore)

        for i in range(tb_count):
            table_obj = db_obj.create_table("test_create_10k_table" + str(i) + suffix, {"c1": {"type": "int"}},
                                            ConflictType.Error)
            assert table_obj is not None

        tbs = db_obj.list_tables()
        res_tbs = []
        for tb_name in tbs.table_names:
            if tb_name.startswith("test_create_10k") and tb_name.endswith(suffix):
                res_tbs.append(tb_name)
        assert len(res_tbs) == tb_count

        for i in range(tb_count):
            res = db_obj.drop_table("test_create_10k_table" + str(i) + suffix, ConflictType.Error)
            assert res.error_code == ErrorCode.OK
