import pytest
import os

from common import common_values
from infinity.common import ConflictType

from internal.test_table import TestTable
from internal.utils import trace_expected_exceptions
@pytest.fixture(scope="class")
def local_infinity(request):
    return request.config.getoption("--local-infinity")

@pytest.fixture(scope="class")
def setup_class(request, local_infinity):
    if local_infinity:
        uri = common_values.TEST_LOCAL_PATH
    else:
        uri = common_values.TEST_LOCAL_HOST
    request.cls.uri = uri
    request.cls.test_infinity_obj = TestTable(uri)
    yield
    request.cls.test_infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.mark.parametrize("column_name", common_values.invalid_name_array)
    def test_create_table_with_invalid_column_name_python(self, column_name):
        print("test test_create_table_with_invalid_column_name_python begin")
        self.test_infinity_obj._test_create_table_with_invalid_column_name_python(column_name)

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
    def test_table_with_different_invalid_options(self, invalid_option_array):
        self.test_infinity_obj._test_table_with_different_invalid_options(invalid_option_array)

    @trace_expected_exceptions
    def test_create_or_drop_same_table_in_different_thread(self):
        self.test_infinity_obj._test_create_or_drop_same_table_in_different_thread()

    def test_create_empty_column_table(self):
        self.test_infinity_obj._test_create_empty_column_table()


    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    def test_create_valid_option(self, types):
        self.test_infinity_obj._test_create_valid_option(types)

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"
    ])
    @pytest.mark.parametrize("boolean", [True, False])
    def test_drop_option(self, types, boolean):
        self.test_infinity_obj._test_drop_option(types, boolean)

    def test_create_same_name_table(self):
        self.test_infinity_obj._test_create_same_name_table()

    # todo fix: why return TABLE_NOT_EXIST error
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_drop_same_name_table(self):
        self.test_infinity_obj._test_drop_same_name_table()

    def test_same_column_name(self):
        self.test_infinity_obj._test_same_column_name()

    @pytest.mark.parametrize("types", [
        "int", "int8", "int16", "int32", "int64", "integer",
        "float", "float32", "double", "float64",
        "varchar",
        "bool",
        "vector, 3, float"])
    @pytest.mark.parametrize("column_number", [[
        0, 1, pow(2, 63) - 1
    ]])
    def test_column_numbers(self, types, column_number):
        self.test_infinity_obj._test_column_numbers(types, column_number)

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        ConflictType.Replace,
        0,
        1,
        2,
    ])
    def test_create_valid_option(self, conflict_type):
        self.test_infinity_obj._test_table_create_valid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_create_invalid_option(self, conflict_type):
        self.test_infinity_obj._test_table_create_invalid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def test_drop_valid_option(self, conflict_type):
        self.test_infinity_obj._test_table_drop_valid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_drop_invalid_option(self, conflict_type):
        self.test_infinity_obj._test_table_drop_invalid_option(conflict_type)

    def test_create_duplicated_table_with_ignore_option(self):
        self.test_infinity_obj._test_create_duplicated_table_with_ignore_option()

    def test_create_duplicated_table_with_error_option(self):
        self.test_infinity_obj._test_create_duplicated_table_with_error_option()

    def test_create_duplicated_table_with_replace_option(self):
        self.test_infinity_obj._test_create_duplicated_table_with_replace_option()

    def test_table(self):
        # self.test_infinity_obj._test_version()
        self.test_infinity_obj._test_table()
        self.test_infinity_obj._test_show_tables()
        self.test_infinity_obj._test_create_varchar_table()
        self.test_infinity_obj._test_create_embedding_table()
        self.test_infinity_obj._test_create_tensor_table()
        self.test_infinity_obj._test_create_tensorarray_table()
        self.test_infinity_obj._test_create_table_with_invalid_column_name()
        self.test_infinity_obj._test_table_with_different_column_name()
        # create/drop/show/get valid table name with different column types
        self.test_infinity_obj._test_table_with_different_column_types()
        # create/drop/show/get table with 10000 columns with various column types.
        self.test_infinity_obj._test_table_with_various_column_types()
        self.test_infinity_obj._test_table_with_invalid_options()
        self.test_infinity_obj._test_create_drop_table()

    # todo fix
    # local infinity disconnect = uninit, db obj cannot know weather disconnected
    @pytest.mark.usefixtures("skip_if_local_infinity")
    def test_after_disconnect_use_table(self):
        self.test_infinity_obj._test_after_disconnect_use_table()

    # create/drop/show/get 1000 tables with 10000 columns with various column types.
    @pytest.mark.slow
    @pytest.mark.skipif(condition=os.getenv("RUNSLOWTEST") != "1", reason="Cost too much time")
    def test_create_10k_table(self):
        self.test_infinity_obj._test_create_10K_table()

    def test_create_1K_table(self):
        self.test_infinity_obj._test_create_1K_table()



