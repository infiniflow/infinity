import infinity
import pytest
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.remote_thrift.table import traverse_conditions
from sqlglot import condition

from common import common_values


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


@pytest.mark.parametrize("cond_str,expected_func", [
    ("c1 LIKE '%test%'", "like"),
    ("c1 NOT LIKE '%test%'", "not_like"),
    ("c1 LIKE '%test!%' ESCAPE '!'", "like"),
    ("c1 NOT LIKE '%test!%' ESCAPE '!'", "not_like"),
])
def test_condition_embedded_like(request, cond_str, expected_func):
    """LIKE variants produce a column, pattern and escape argument."""
    if not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    from infinity_embedded.local_infinity.utils import (
        traverse_conditions as embedded_traverse,
    )

    res = embedded_traverse(condition(cond_str))
    assert res.function_expr.func_name == expected_func
    assert len(res.function_expr.arguments) == 3
    assert res.function_expr.arguments[0].column_expr.names == ["c1"]
    assert res.function_expr.arguments[1].constant_expr.str_value == ("%test!%" if "ESCAPE" in cond_str else "%test%")
    assert res.function_expr.arguments[2].constant_expr.str_value == ("!" if "ESCAPE" in cond_str else "\\")


@pytest.mark.parametrize("with_escape", [False, True])
def test_condition_embedded_wrapped_not_like(request, with_escape):
    """Older SQLGlot Not wrappers reach the dedicated NOT LIKE handler."""
    if not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    from infinity_embedded.local_infinity.utils import (
        traverse_conditions as embedded_traverse,
    )
    from sqlglot import expressions as exp

    node = exp.Like(this=exp.column("c1"), expression=exp.Literal.string("%test!%"))
    if with_escape:
        node = exp.Escape(this=node, expression=exp.Literal.string("!"))
    res = embedded_traverse(exp.Not(this=node))
    assert res.function_expr.func_name == "not_like"
    assert len(res.function_expr.arguments) == 3
    assert res.function_expr.arguments[0].column_expr.names == ["c1"]
    assert res.function_expr.arguments[1].constant_expr.str_value == "%test!%"
    assert res.function_expr.arguments[2].constant_expr.str_value == ("!" if with_escape else "\\")
