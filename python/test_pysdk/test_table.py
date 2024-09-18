import importlib
import sys
import os
import concurrent.futures
import os
import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType, InfinityException
import infinity
import infinity_embedded
from infinity.errors import ErrorCode
from common.utils import trace_expected_exceptions
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http

@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def http(request):
    return request.config.getoption("--http")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity, http):
    if local_infinity:
        module = importlib.import_module("infinity_embedded.common")
        func = getattr(module, 'ConflictType')
        globals()['ConflictType'] = func
        func = getattr(module, 'InfinityException')
        globals()['InfinityException'] = func
        uri = common_values.TEST_LOCAL_PATH
        request.cls.infinity_obj = infinity_embedded.connect(uri)
    elif http:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity_http()
    else:
        uri = common_values.TEST_LOCAL_HOST
        request.cls.infinity_obj = infinity.connect(uri)
    request.cls.uri = uri
    yield
    request.cls.infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("local_infinity")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.mark.parametrize("column_name", common_values.invalid_name_array)
    def test_create_table_with_invalid_column_name_python(self, column_name, suffix):
        """
        target: create with invalid column name
        methods: create table with invalid column name
        expect: all operations throw exception on python side
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_table_with_invalid_column_name_python"+suffix, ConflictType.Ignore)

        try:
            tb = db_obj.create_table("test_create_table_with_invalid_column_name_python"+suffix, {column_name: {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

    # create/drop table with different invalid options
    @pytest.mark.parametrize("invalid_option_array", [
        pytest.param([]),
        pytest.param(()),
        pytest.param({}),
        pytest.param(1.1),
        pytest.param(''),
        pytest.param(' '),
        pytest.param('12'),
        pytest.param('name-12'),
        pytest.param('12name'),
        pytest.param('数据库名'),
        pytest.param(''.join('x' for i in range(65536 + 1))),
        None,
    ])
    def test_table_with_different_invalid_options(self, invalid_option_array, suffix):
        """
        target: create/drop table with different invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_different_invalid_options"+suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_table_with_different_invalid_options"+suffix, {"c1": {"type": "int"}},
                                invalid_option_array)

        assert e.type == InfinityException
        #assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

    @trace_expected_exceptions
    def test_create_or_drop_same_table_in_different_thread(self, suffix):
        """
        target: create/drop same table in different thread to test conflict
        methods: create table at same time for 16 times
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_or_drop_same_table_in_different_thread"+suffix
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

        db_obj.drop_table(table_name, ConflictType.Ignore)

    def test_create_empty_column_table(self, suffix):
        """
        target: create empty column table
        methods: create empty column table
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_empty_column_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            db_obj.create_table(table_name, {}, ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.NO_COLUMN_DEFINED


    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    def test_create_valid_option(self, types, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_valid_option"+suffix, ConflictType.Ignore)

        db_obj.create_table("test_valid_option"+suffix, {"c1": {"type": types}}, ConflictType.Error)

        res = db_obj.drop_table("test_valid_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    #     # disconnect
    #     res = self.infinity_obj.disconnect()
    #     assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    @pytest.mark.parametrize("boolean", [True, False])
    def test_drop_option(self, types, boolean, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_option"+suffix, ConflictType.Ignore)

        db_obj.create_table("test_drop_option"+suffix, {"c1": {"type": types}}, ConflictType.Error)
        res = db_obj.drop_table("test_drop_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_same_name_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_same_name"+suffix, ConflictType.Ignore)

        # create
        db_obj.create_table("test_create_same_name"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_create_same_name"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_same_name"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    # todo fix: why return TABLE_NOT_EXIST error
    @pytest.mark.usefixtures("skip_if_http")
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_drop_same_name_table(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_drop_same_name"+suffix, ConflictType.Ignore)
        # drop
        db_obj.drop_table("test_drop_same_name"+suffix)

    def test_same_column_name(self, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_same_column_name"+suffix, ConflictType.Ignore)

        db_obj.create_table("test_same_column_name"+suffix, {"c1": {"type": "int"},
                                                      "c1": {"type": "int"}}, ConflictType.Error)

        res = db_obj.drop_table("test_same_column_name"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"])
    @pytest.mark.parametrize("column_number", [[
        0, 1, pow(2, 63) - 1
    ]])
    def test_column_numbers(self, types, column_number, suffix):
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_column_numbers"+suffix, ConflictType.Ignore)

        values = {"c" + str(i): {"type": types} for i in column_number}
        db_obj.create_table("test_column_numbers"+suffix, values, ConflictType.Error)

        res = db_obj.drop_table("test_column_numbers"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        ConflictType.Replace,
        0,
        1,
        2,
    ])
    def test_create_valid_option(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_create_valid_option"+suffix, ConflictType.Ignore)
        db_obj.create_table("test_table_create_valid_option"+suffix, {"c1": {"type": "int"}}, conflict_type)
        res = db_obj.drop_table("test_table_create_valid_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_create_invalid_option(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_various_table_create_option"+suffix, ConflictType.Ignore)
        with pytest.raises(InfinityException) as e:
            db_obj.create_table("test_various_table_create_option"+suffix, {"c1": {"type": "int"}}, conflict_type)

        assert e.type == InfinityException
        #assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def test_drop_valid_option(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.create_table("test_table_drop_valid_option"+suffix, {"c1": {"type": "int"}}, ConflictType.Ignore)
        db_obj.drop_table("test_table_drop_valid_option"+suffix, conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_drop_invalid_option(self, conflict_type, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.create_table("test_various_table_drop_option"+suffix, {"c1": {"type": "int"}}, ConflictType.Ignore)
        with pytest.raises(InfinityException) as e:
            db_obj.drop_table("test_various_table_drop_option"+suffix, conflict_type)

        assert e.type == InfinityException
        #assert e.value.args[0] == ErrorCode.INVALID_CONFLICT_TYPE

        res = db_obj.drop_table("test_various_table_drop_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_ignore_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_ignore_option"+suffix, ConflictType.Ignore)

        for i in range(100):
            db_obj.create_table("test_create_duplicated_table_with_ignore_option"+suffix, {"c1": {"type": "int"}},
                                ConflictType.Ignore)

        res = db_obj.drop_table("test_create_duplicated_table_with_ignore_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_error_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_error_option"+suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_error_option"+suffix, {"c1": {"type": "int"}},
                                    ConflictType.Error)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_duplicated_table_with_error_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_duplicated_table_with_replace_option(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_duplicated_table_with_replace_option"+suffix, ConflictType.Ignore)

        with pytest.raises(InfinityException) as e:
            for i in range(100):
                db_obj.create_table("test_create_duplicated_table_with_replace_option"+suffix, {"c" + str(i): {"type": "int"}},
                                    ConflictType.Replace)

        assert e.type == InfinityException
        assert e.value.args[0] == ErrorCode.DUPLICATE_TABLE_NAME

        res = db_obj.drop_table("test_create_duplicated_table_with_replace_option"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

    def test_create_upper_table_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_lower_name = "test_table_my_table"+suffix.lower()
        db_obj.drop_table(table_lower_name, ConflictType.Ignore)

        table_upper_name = "TEST_TABLE_MY_TABLE"+suffix.upper()
        # create table
        tb = db_obj.create_table(
            table_upper_name, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb

        # get table
        res = db_obj.get_table(table_lower_name)
        res = db_obj.get_table(table_upper_name)

    def test_create_table_with_upper_column_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"C1": {"type": "int", "constraints": ["primary key"]}, "C2": {"type": "float"}},
            ConflictType.Error)
        assert tb

        # get table
        res = db_obj.get_table(table_name)

        res = db_obj.show_columns(table_name)
        print("\n")
        print(res)
        assert res["column_name"][0] == "c1"
        assert res["column_name"][1] == "c2"
        db_obj.drop_table(table_name, ConflictType.Ignore)

    @pytest.mark.usefixtures("skip_if_http")
    def test_create_table_with_upper_param_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"TYPE": "int", "CONSTRAINTS": ["primary key"]}, "c2": {"TYPE": "float"}},
            ConflictType.Error)
        assert tb

        # get table
        res = db_obj.get_table(table_name)

        res = db_obj.show_columns(table_name)
        print("\n")
        print(res)
        db_obj.drop_table(table_name, ConflictType.Ignore)

    def test_create_table_with_upper_data_type_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "INT", "constraints": ["primary key"]}, "c2": {"type": "FLOAT"}},
            ConflictType.Error)
        assert tb

        # get table
        res = db_obj.get_table(table_name)

        res = db_obj.show_columns(table_name)
        print("\n")
        print(res)
        db_obj.drop_table(table_name, ConflictType.Ignore)

    def test_create_table_with_upper_constraint_name(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        # create table
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "int", "constraints": ["PRIMARY KEY"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb

        # get table
        res = db_obj.get_table(table_name)

        res = db_obj.show_columns(table_name)
        print("\n")
        print(res)
        db_obj.drop_table(table_name, ConflictType.Ignore)

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
            - 'my_table'
        4. list tables: empty
        expect: all operations successfully
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_table_my_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            table_name, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        with pytest.raises(Exception):
            tb = db_obj.create_table(
                "my_table!@#", {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
                ConflictType.Error)
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

        # FIXME: res = db_obj.show_table("my_table")

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        res = db_obj.drop_table(table_name, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

    def _test_show_tables(self, suffix):
        db = self.infinity_obj.get_database("default_db")

        with pl.Config(fmt_str_lengths=1000):
            res = db.show_tables()
            print(res)
            # check the polars dataframe
            assert res.columns == ["database", "table", "type", "column_count", "block_count", "block_capacity",
                                   "segment_count", "segment_capacity"]

    def _test_create_varchar_table(self, suffix):
        """
        target: test create table with varchar column
        method: create table with varchar column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_varchar_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name,
                                        {"c1": {"type": "varchar", "constraints": ["primary key"]},
                                         "c2": {"type": "float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_embedding_table(self, suffix):
        """
        target: test create table with embedding column
        method: create table with embedding column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_embedding_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, {
            "c1": {"type": "vector,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_tensor_table(self, suffix):
        """
        target: test create table with tensor column
        method: create table with tensor column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_tensor_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)
        table_obj = db_obj.create_table(table_name, {
            "c1": {"type": "tensor,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table(table_name)
        assert res.error_code == ErrorCode.OK

    def _test_create_tensorarray_table(self, suffix):
        """
        target: test create table with tensorarray column
        method: create table with tensorarray column
        expected: ok
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_tensorarray_table"+suffix, ConflictType.Ignore)
        table_obj = db_obj.create_table("test_create_tensorarray_table"+suffix, {
            "c1": {"type": "tensorarray,128,float"}}, ConflictType.Error)
        assert table_obj

        res = db_obj.drop_table("test_create_tensorarray_table"+suffix)
        assert res.error_code == ErrorCode.OK

    def _test_create_table_with_invalid_column_name(self, suffix):
        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                " ": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "12": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "[]": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "()": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "{}": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "1": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

        with pytest.raises(Exception):
            db_obj = self.infinity_obj.get_database("default_db")
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Ignore)
            table_obj = db_obj.create_table("test_create_invalid_column_name"+suffix, {
                "1.1": {"type": "vector,128,float"}}, ConflictType.Error)
            assert table_obj
            db_obj.drop_table("test_create_invalid_column_name"+suffix, ConflictType.Error)

    def _test_table_with_different_column_name(self, suffix, local_infinity):
        """
        target: test create/drop/show/get valid table name with different column names
        methods:
        1. create table
        2. get table
        3. list table
        4. show table
        expect: all operations successfully

        """
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        table_name = "test_table_with_different_column_name"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        for column_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table(table_name, {column_name: {"type": "int"}}, ConflictType.Error)
                raise Exception(f"Can create column_name: {column_name}")
            except Exception as e:
                print(e)

            # list table
            try:
                res = db_obj.list_tables()
            except Exception as e:
                print(e)

            # get table
            try:
                res = db_obj.get_table(table_name)
            except Exception as e:
                print(e)

            # drop table
            try:
                res = db_obj.drop_table(table_name)
            except Exception as e:
                print(e)
        # FIXME: res = db_obj.show_table("my_table")

        # disconnect
        # res = infinity_obj.disconnect()
        # assert res

        try:
            res = infinity_obj.disconnect()
        except Exception as e:
            print(e)

    def _test_table_with_different_column_types(self, suffix, local_infinity):
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
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_different_column_types"+suffix, ConflictType.Ignore)

        # infinity
        tb = db_obj.create_table(
            "test_table_with_different_column_types"+suffix, {
                "c1": {"type": "bool", "constraints": ["primary key"]},
                "c2": {"type": "int"}, "c3": {"type": "int8"}, "c4": {"type": "int16"},
                "c5": {"type": "int32"}, "c6": {"type": "int64"}, "c7": {"type": "float"}, "c8": {"type": "float32"},
                "c9": {"type": "double"}, "c10": {"type": "float64"}, "c11": {"type": "varchar"},
                "c12": {"type": "integer"}}, ConflictType.Error)
        assert tb is not None

        for tb_type in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("test_table_with_different_column_types"+suffix, {"c1": {"type": tb_type}},
                                         ConflictType.Error)
                raise Exception(f"Can create tb: {tb_type}")
            except Exception as e:
                print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("test_table_with_different_column_types"+suffix)
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("test_table_with_different_column_types"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def _test_table_with_various_column_types(self, suffix, local_infinity):
        """
        target: create/drop/show/get table with 10000 columns with various column types.
        methods: create table with various column types
        expect: all operations successfully
        """
        # connect
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_various_column_types"+suffix, ConflictType.Ignore)
        c_count = 10000

        types = [
            "int", "int8", "int16", "int32", "int64", "float",
            "float32", "double", "float64", "varchar", "integer", "bool",
        ]
        # make params
        params = {}
        for i in range(c_count - 12):
            params.update({
                "c" + str(i): {"type": types[i % 12]}
            })

        # create tb with 10000 columns with various column types
        try:
            tb = db_obj.create_table('test_table_with_various_column_types'+suffix, params, ConflictType.Error)
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        res = db_obj.list_tables()
        assert res.error_code == ErrorCode.OK

        # get table
        try:
            res = db_obj.get_table("test_table_with_various_column_types"+suffix)
            print(res.output(["c2"]))
        except Exception as e:
            print(e)

        # drop table
        res = db_obj.drop_table("test_table_with_various_column_types"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def _test_table_with_invalid_options(self, suffix, local_infinity):
        """
        target: create/drop table with invalid options.
        methods: create table with various options
        expect: all operations successfully
        """
        # connect
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_table_with_invalid_options"+suffix, ConflictType.Ignore)

        for option_name in common_values.invalid_name_array:
            try:
                tb = db_obj.create_table("test_table_with_invalid_options"+suffix, {"c1": {"type": "int"}}, option_name)
                # raise Exception(f"Can create option_name: {option_name}")
            except Exception as e:
                print(e)

        db_obj.drop_table("test_table_with_invalid_options"+suffix, ConflictType.Error)
        # try:
        #     res = self.infinity_obj.disconnect()
        # except Exception as e:
        #     print(e)

    def _test_create_drop_table(self, suffix, local_infinity):
        """
        target: create created table, drop dropped table
        methods: create table ,drop table
        expect: all operations successfully
        """
        # connect
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_create_drop_table"+suffix, ConflictType.Ignore)

        # create
        tb = db_obj.create_table(
            "test_create_drop_table"+suffix, {"c1": {"type": "int", "constraints": ["primary key"]}, "c2": {"type": "float"}},
            ConflictType.Error)
        assert tb is not None

        try:
            tb = db_obj.create_table("test_create_drop_table"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

        # drop
        db_obj.drop_table("test_create_drop_table"+suffix, ConflictType.Error)

        try:
            tb = db_obj.drop_table("test_create_drop_table"+suffix, ConflictType.Error)
        except Exception as e:
            print(e)

    def test_table(self, suffix, local_infinity):
        # self.test_infinity_obj._test_version()
        self._test_table(suffix)
        self._test_show_tables(suffix)
        self._test_create_varchar_table(suffix)
        self._test_create_embedding_table(suffix)
        self._test_create_tensor_table(suffix)
        self._test_create_tensorarray_table(suffix)
        self._test_create_table_with_invalid_column_name(suffix)
        self._test_table_with_different_column_name(suffix, local_infinity)
        # create/drop/show/get valid table name with different column types
        self._test_table_with_different_column_types(suffix, local_infinity)
        # create/drop/show/get table with 10000 columns with various column types.
        self._test_table_with_various_column_types(suffix, local_infinity)
        self._test_table_with_invalid_options(suffix, local_infinity)
        self._test_create_drop_table(suffix, local_infinity)

    # todo fix
    # local infinity disconnect = uninit, db obj cannot know weather disconnected
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_after_disconnect_use_table(self, suffix, local_infinity):
        """
        target: after disconnection, create / drop / show / list / get table
        methods:
        1. disconnect database
        2. create / drop / show / list / get table
        expect: all operations successfully
        """
        # connect
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        db_obj.drop_table("test_after_disconnect_use_table"+suffix, ConflictType.Ignore)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

        # create table
        try:
            res = db_obj.create_table("test_after_disconnect_use_table"+suffix, {"c1": {"type": "int"}}, ConflictType.Error)
        except Exception as e:
            print(e)

        # drop table
        try:
            res = db_obj.drop_table("test_after_disconnect_use_table"+suffix, ConflictType.Error)
        except Exception as e:
            print(e)

        # FIXME: res = db_obj.show_table("my_table")

        # list table
        try:
            res = db_obj.list_tables()
        except Exception as e:
            print(e)

        # get table
        try:
            res = db_obj.get_table("my_table"+suffix)
        except Exception as e:
            print(e)
    # create/drop/show/get 1000 tables with 10000 columns with various column types.
    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST") != "1", reason="Cost too much time")
    def test_create_10k_table(self, suffix):
        """
        target: create/drop/list/get 1K table
        methods: show table
        expect: all operations successfully
        """
        # connect
        db_obj = self.infinity_obj.get_database("default_db")
        table_name = "test_create_10K_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        tb_count = 10000
        for i in range(tb_count):
            res = db_obj.create_table(table_name + str(i), {"c1": {"type": "int"}}, ConflictType.Error)
            assert res

        for i in range(tb_count):
            res = db_obj.drop_table(table_name + str(i), ConflictType.Error)
            assert res.error_code == ErrorCode.OK
    def test_create_1K_table(self, suffix, local_infinity):
        """
        target: create/drop/list/get 1K table
        methods: show table
        expect: all operations successfully
        """
        # connect
        if local_infinity:
            infinity_obj = infinity_embedded.connect(self.uri)
        else:
            infinity_obj = infinity.connect(self.uri)
        db_obj = infinity_obj.get_database("default_db")
        table_name = "test_create_1K_table"+suffix
        db_obj.drop_table(table_name, ConflictType.Ignore)

        tb_count = 1000
        for i in range(tb_count):
            try:
                tb = db_obj.create_table(table_name + str(i), {"c1": {"type": "int"}}, ConflictType.Error)
            except Exception as e:
                print(e)

        # list table
        try:
            res = db_obj.list_tables()
            print(res)
        except Exception as e:
            print(e)

        # get table
        for i in range(tb_count):
            try:
                res = db_obj.get_table(table_name + str(i))
            except Exception as e:
                print(e)

        for i in range(tb_count):
            try:
                tb = db_obj.drop_table(table_name + str(i), ConflictType.Error)
            except Exception as e:
                print(e)

        # disconnect
        res = infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK
