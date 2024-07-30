import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode
import infinity


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
    request.cls.infinity_obj = infinity.connect(uri)
    yield
    request.cls.infinity_obj.disconnect()

@pytest.mark.usefixtures("setup_class")
class TestInfinity:
    def _test_show_table(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table", ConflictType.Ignore)
        db_obj.create_table(
            "test_show_table",
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_table("test_show_table")
            print(res)

        res = db_obj.drop_table("test_show_table", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
    def _test_show_columns(self):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_columns", ConflictType.Ignore)
        db_obj.create_table(
            "test_show_columns",
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_columns("test_show_columns")
            print(res)
            # check the polars dataframe
            assert res.columns == ["column_name", "column_type", "constraint", "default"]

        res = db_obj.drop_table("test_show_columns", ConflictType.Error)
        assert res.error_code == ErrorCode.OK
    def _test_show_big_databases(self):
        for i in range(8193):
            self.infinity_obj.drop_database(f"test_show_big_databases_{i}", ConflictType.Ignore)

        for i in range(8193):
            self.infinity_obj.create_database(f"test_show_big_databases_{i}", ConflictType.Ignore)

        res = self.infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for i in range(8193):
            self.infinity_obj.drop_database(f"test_show_big_databases_{i}", ConflictType.Ignore)

    def test_show(self):
        self._test_show_table()
        self._test_show_columns()
        self._test_show_big_databases()