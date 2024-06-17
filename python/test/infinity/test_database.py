import pytest

from common import common_values
from infinity.common import ConflictType

from src.test_database import TestDatabase

@pytest.mark.usefixtures("local_infinity")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, local_infinity):
        if local_infinity:
            self.uri = common_values.TEST_LOCAL_PATH
        else:
            self.uri = common_values.TEST_REMOTE_HOST
        self.test_infinity_obj = TestDatabase(self.uri)

    def teardown(self):
        self.test_infinity_obj.disconnect()

    def test_database(self):
        self.test_infinity_obj._test_version()
        self.test_infinity_obj._test_database()
        self.test_infinity_obj._test_create_database_invalid_name()
        self.test_infinity_obj._test_create_drop_show_1K_databases()
        self.test_infinity_obj._test_repeatedly_create_drop_show_databases()
        self.test_infinity_obj._test_drop_database_with_invalid_name()
        self.test_infinity_obj._test_get_db()
        self.test_infinity_obj._test_drop_non_existent_db()
        self.test_infinity_obj._test_get_drop_db_with_two_threads()
        self.test_infinity_obj._test_create_same_db_in_different_threads()
        self.test_infinity_obj._test_show_database()

    @pytest.mark.parametrize("conflict_type", [ConflictType.Error,
                                               ConflictType.Ignore,
                                               ConflictType.Replace,
                                               0,
                                               1,
                                               2,
                                               ])
    def test_create_with_valid_option(self, conflict_type):
        self.test_infinity_obj._test_create_with_valid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [pytest.param(1.1),
                                               pytest.param("#@$@!%string"),
                                               pytest.param([]),
                                               pytest.param({}),
                                               pytest.param(()),
                                               ])
    def test_create_with_invalid_option(self, conflict_type):
        self.test_infinity_obj._test_create_with_invalid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        ConflictType.Error,
        ConflictType.Ignore,
        0,
        1,
    ])
    def test_drop_option_with_valid_option(self, conflict_type):
        self.test_infinity_obj._test_drop_option_with_valid_option(conflict_type)

    @pytest.mark.parametrize("conflict_type", [
        pytest.param(ConflictType.Replace),
        pytest.param(2),
        pytest.param(1.1),
        pytest.param("#@$@!%string"),
        pytest.param([]),
        pytest.param({}),
        pytest.param(()),
    ])
    def test_drop_option(self, conflict_type):
        self.test_infinity_obj._test_drop_option(conflict_type)

    @pytest.mark.parametrize("table_name", ["test_show_table"])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_show_valid_table(self, get_infinity_db, table_name):
        self.test_infinity_obj._test_show_valid_table(get_infinity_db, table_name)

    @pytest.mark.parametrize("table_name", [pytest.param("Invalid name"),
                                            pytest.param(1),
                                            pytest.param(1.1),
                                            pytest.param(True),
                                            pytest.param([]),
                                            pytest.param(()),
                                            pytest.param({}),
                                            ])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_show_invalid_table(self, get_infinity_db, table_name):
        self.test_infinity_obj._test_show_invalid_table(get_infinity_db, table_name)

    @pytest.mark.parametrize("table_name", [pytest.param("not_exist_name")])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_show_not_exist_table(self, get_infinity_db, table_name):
        self.test_infinity_obj._test_show_not_exist_table(get_infinity_db, table_name)

    @pytest.mark.parametrize("column_name", ["test_show_table_columns"])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_show_table_columns_with_valid_name(self, get_infinity_db, column_name):
        self.test_infinity_obj._test_show_table_columns_with_valid_name(get_infinity_db, column_name)

    @pytest.mark.parametrize("column_name", [pytest.param("Invalid name"),
                                             pytest.param("not_exist_name"),
                                             pytest.param(1),
                                             pytest.param(1.1),
                                             pytest.param(True),
                                             pytest.param([]),
                                             pytest.param(()),
                                             pytest.param({}),
                                             ])
    @pytest.mark.parametrize("get_infinity_db", [""], indirect=True)
    def test_show_table_columns_with_invalid_name(self, get_infinity_db, column_name):
        self.test_infinity_obj._test_show_table_columns_with_invalid_name(get_infinity_db, column_name)

    @pytest.mark.slow
    def test_create_drop_show_1M_databases(self):
        self.test_infinity_obj._test_create_drop_show_1M_databases()