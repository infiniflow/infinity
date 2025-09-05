import importlib
import sys
import os
import pytest
import polars as pl
import infinity.index as index
from common import common_values
from infinity.common import InfinityException
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
    def test_set_config(self, suffix):
        # log_level
        res = self.infinity_obj.show_config("log_level")
        assert res.error_code == ErrorCode.OK
        old_value = res.config_value

        res = self.infinity_obj.set_config("log_level", "debug")
        assert res.error_code == ErrorCode.OK

        res = self.infinity_obj.show_config("log_level")
        assert res.error_code == ErrorCode.OK
        assert res.config_value.lower() == "debug"

        res = self.infinity_obj.set_config("log_level", old_value)
        assert res.error_code == ErrorCode.OK

        res = self.infinity_obj.show_config("log_level")
        assert res.error_code == ErrorCode.OK
        assert res.config_value == old_value

        with pytest.raises(InfinityException) as e:
            self.infinity_obj.set_config("log_level", 1)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.DATA_TYPE_MISMATCH

        # cleanup_interval
        res = self.infinity_obj.show_config("cleanup_interval")
        assert res.error_code == ErrorCode.OK
        old_value = res.config_value

        res = self.infinity_obj.set_config("cleanup_interval", old_value + 10)
        assert res.error_code == ErrorCode.OK

        res = self.infinity_obj.show_config("cleanup_interval")
        assert res.error_code == ErrorCode.OK
        assert res.config_value == old_value + 10

        res = self.infinity_obj.set_config("cleanup_interval", old_value)
        assert res.error_code == ErrorCode.OK

        res = self.infinity_obj.show_config("cleanup_interval")
        assert res.error_code == ErrorCode.OK
        assert res.config_value == old_value

        # read-only config
        with pytest.raises(InfinityException) as e:
            self.infinity_obj.set_config("mem_index_capacity", old_value + 1000)
            assert e.type == InfinityException
            assert e.value.args[0] == ErrorCode.INVALID_COMMAND
            assert e.value.args[1] == "Invalid command: Config mem_index_capacity is read-only"