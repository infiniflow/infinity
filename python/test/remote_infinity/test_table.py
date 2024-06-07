import pytest
import os

from common import common_values
from infinity.common import ConflictType

from src.test_table import TestTable
from src.utils import trace_expected_exceptions

class TestRemoteInfinity():
    def test_table(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_version()
        test_table_obj._test_table()
        test_table_obj._test_show_tables()
        test_table_obj._test_create_varchar_table()
        test_table_obj._test_create_embedding_table()
        test_table_obj._test_create_tensor_table()
        test_table_obj._test_create_tensorarray_table()
        test_table_obj._test_create_table_with_invalid_column_name()
        test_table_obj._test_table_with_different_column_name()
        # create/drop/show/get valid table name with different column types
        test_table_obj._test_table_with_different_column_types()
        # create/drop/show/get table with 10000 columns with various column types.
        test_table_obj._test_table_with_various_column_types()
        test_table_obj._test_after_disconnect_use_table()
        test_table_obj._test_table_with_invalid_options()
        test_table_obj._test_create_drop_table()
        test_table_obj._test_create_1K_table()

    @pytest.mark.parametrize("column_name", common_values.invalid_name_array)
    def test_table_test_create_table_with_invalid_column_name_python(self, column_name):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_table_with_invalid_column_name_python(column_name)

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
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_test_table_with_different_invalid_options(self, get_infinity_db, invalid_option_array):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_table_with_different_invalid_options(get_infinity_db, invalid_option_array)

    # create/drop/show/get 1000 tables with 10000 columns with various column types.
    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST") != "1", reason="Cost too much time")
    def test_table_create_10k_table(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_10K_table()

    @trace_expected_exceptions
    def test_table_create_or_drop_same_table_in_different_thread(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_or_drop_same_table_in_different_thread()

    def test_table_create_empty_column_table(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_empty_column_table()

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    def test_table_create_valid_option(self, types):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_valid_option(types)

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    @pytest.mark.parametrize("boolean", [True, False])
    def test_table_drop_option(self, types, boolean):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_drop_option(types, boolean)

    def test_table_create_same_name_table(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_same_name_table()

    def test_table_drop_same_name_table(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_drop_same_name_table()

    def test_table_same_column_name(self):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_same_column_name()

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"])
    @pytest.mark.parametrize("column_number", [[
        0, 1, pow(2, 63) - 1
    ]])
    def test_table_column_numbers(self, types, column_number):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_column_numbers(types, column_number)

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        ConflictType.Replace,
        0,
        1,
        2,
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_create_valid_option(self, get_infinity_db, conflict_type):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_table_create_valid_option(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_create_invalid_option(self, get_infinity_db, conflict_type):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_table_create_invalid_option(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_drop_valid_option(self, get_infinity_db, conflict_type):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_table_drop_valid_option(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_drop_invalid_option(self, get_infinity_db, conflict_type):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_table_drop_invalid_option(get_infinity_db, conflict_type)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_create_duplicated_table_with_ignore_option(self, get_infinity_db):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_duplicated_table_with_ignore_option(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_create_duplicated_table_with_error_option(self, get_infinity_db):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_duplicated_table_with_error_option(get_infinity_db)

    @pytest.mark.parametrize("get_infinity_db", [common_values.TEST_REMOTE_HOST], indirect=True)
    def test_table_create_duplicated_table_with_replace_option(self, get_infinity_db):
        test_table_obj = TestTable(common_values.TEST_REMOTE_HOST)
        test_table_obj._test_create_duplicated_table_with_replace_option(get_infinity_db)
