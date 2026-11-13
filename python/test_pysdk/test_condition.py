import infinity
import pytest
from common import common_values
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.remote_thrift.table import traverse_conditions
from sqlglot import condition


@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
class TestInfinity:
    @pytest.fixture(autouse=True)
    def setup(self, http):
        if http:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity_http()
        else:
            self.uri = common_values.TEST_LOCAL_HOST
            self.infinity_obj = infinity.connect(self.uri)
        assert self.infinity_obj

    def teardown(self):
        res = self.infinity_obj.disconnect()
        assert res.error_code == ErrorCode.OK

    def test_condition(self):
        for cond_str in [
            "c1 = 1",
            "c1 != 1",
            "c1 > 1 and c2 < 2 or c3 = 3.3",
            "-8 < c1 and c1 <= -7",
            "(-7 < c1 or 9 <= c1) and (c1 = 3)",
            "!(9 <= c1)",
            "c1 IN (1,2,3)",
            "c1 NOT IN (1,2,3)",
            "c1 LIKE '%test%'",
            "c1 NOT LIKE '%test%'",
            "c1 LIKE '%test%' ESCAPE '!'",
            "c1 NOT LIKE '%test%' ESCAPE '!'",
        ]:
            print(cond_str)
            cond = condition(cond_str)
            print(cond)
            res = traverse_conditions(cond)
            print(res)
            assert res


@pytest.mark.parametrize("output_str,expected_func", [
    ("trim(name)", "trim"),
    ("trim(both from name)", "trim"),
    ("trim(leading from name)", "ltrim"),
    ("trim(trailing from name)", "rtrim"),
    ("ltrim(name)", "ltrim"),
    ("rtrim(name)", "rtrim"),
])
def test_output_trim_embedded(request, output_str, expected_func):
    """Whitespace trim variants preserve the function and column argument."""
    if not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    from infinity_embedded.local_infinity.utils import parse_expr as embedded_parse_expr
    from sqlglot import parse_one

    res = embedded_parse_expr(parse_one(output_str))
    assert res.function_expr.func_name == expected_func
    assert len(res.function_expr.arguments) == 1
    assert res.function_expr.arguments[0].column_expr.names == ["name"]


@pytest.mark.parametrize("output_str", [
    "trim(leading 'x' from name)",
    "trim(trailing 'xy' from name)",
    "trim(both ' ' from name)",
    "trim('' from name)",
])
def test_output_trim_embedded_rejects_characters(request, output_str):
    """Explicit trim characters fail instead of silently changing semantics."""
    if not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    from infinity_embedded.common import InfinityException
    from infinity_embedded.errors import ErrorCode
    from infinity_embedded.local_infinity.utils import parse_expr as embedded_parse_expr
    from sqlglot import parse_one

    with pytest.raises(InfinityException, match="explicit characters") as exc_info:
        embedded_parse_expr(parse_one(output_str))
    assert exc_info.value.error_code == ErrorCode.INVALID_EXPRESSION
