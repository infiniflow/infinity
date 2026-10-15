from unittest.mock import Mock

import pytest
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.remote_thrift.table import RemoteTable


@pytest.mark.parametrize("method", ["import_data", "export_data"])
@pytest.mark.parametrize("delimiter", [1, None, True, b",", [], {}])
def test_non_string_delimiter(method, delimiter):
    conn = Mock()
    table = RemoteTable(conn, "default_db", "test_table")

    with pytest.raises(InfinityException) as exc:
        getattr(table, method)("/tmp/data.csv", {"delimiter": delimiter})

    assert exc.value.error_code == ErrorCode.IMPORT_FILE_FORMAT_ERROR
    assert "delimiter" in exc.value.error_msg
    getattr(conn, method).assert_not_called()


@pytest.mark.parametrize("method", ["import_data", "export_data"])
@pytest.mark.parametrize("delimiter", ["", "||"])
def test_invalid_delimiter_length(method, delimiter):
    conn = Mock()
    table = RemoteTable(conn, "default_db", "test_table")

    with pytest.raises(InfinityException) as exc:
        getattr(table, method)("/tmp/data.csv", {"delimiter": delimiter})

    assert exc.value.error_code == ErrorCode.IMPORT_FILE_FORMAT_ERROR
    getattr(conn, method).assert_not_called()


@pytest.mark.parametrize("method", ["import_data", "export_data"])
@pytest.mark.parametrize("delimiter", [",", "\t", "|", "X"])
def test_valid_delimiter(method, delimiter):
    conn = Mock()
    response = Mock(error_code=ErrorCode.OK)
    getattr(conn, method).return_value = response
    table = RemoteTable(conn, "default_db", "test_table")

    assert getattr(table, method)("/tmp/data.csv", {"delimiter": delimiter}) is response

    call = getattr(conn, method).call_args
    assert call.kwargs[f"{method.split('_')[0]}_options"].delimiter == delimiter.lower()
