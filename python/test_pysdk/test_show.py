import importlib
import sys
import os
import pytest
import polars as pl
from common import common_values
from infinity.common import ConflictType
from infinity.errors import ErrorCode
import infinity
import infinity_embedded
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
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    def _test_show_table(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_table"+suffix, ConflictType.Ignore)
        db_obj.create_table(
            "test_show_table"+suffix,
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_table("test_show_table"+suffix)
            print(res)

        res = db_obj.drop_table("test_show_table"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
    def _test_show_columns(self, suffix):
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_show_columns"+suffix, ConflictType.Ignore)
        db_obj.create_table(
            "test_show_columns"+suffix,
            {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector,5,float"}},
            ConflictType.Error)
        with pl.Config(fmt_str_lengths=1000):
            res = db_obj.show_columns("test_show_columns"+suffix)
            print(res)
            # check the polars dataframe
            assert res.columns == ["column_name", "column_type", "constraint", "default"]

        res = db_obj.drop_table("test_show_columns"+suffix, ConflictType.Error)
        assert res.error_code == ErrorCode.OK
    def _test_show_big_databases(self, suffix):
        for i in range(8193):
            self.infinity_obj.drop_database(f"test_show_big_databases_{i}"+suffix, ConflictType.Ignore)

        for i in range(8193):
            self.infinity_obj.create_database(f"test_show_big_databases_{i}"+suffix, ConflictType.Ignore)

        res = self.infinity_obj.list_databases()
        assert res.error_code == ErrorCode.OK

        for i in range(8193):
            self.infinity_obj.drop_database(f"test_show_big_databases_{i}"+suffix, ConflictType.Ignore)

    def test_show(self, suffix):
        self._test_show_table(suffix)
        self._test_show_columns(suffix)
        self._test_show_big_databases(suffix)