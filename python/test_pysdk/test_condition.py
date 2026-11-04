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

    def test_output_trim_embedded(self, request):
        # embedded SDK output traversal crashed on TRIM variants carrying a
        # position/character ("unknown expression type: TRIM(name, ' ')");
        # sqlglot also folds LTRIM/RTRIM into Trim with a position flag.
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from sqlglot import parse_one
        from infinity_embedded.local_infinity.utils import parse_expr as embedded_parse_expr
        for output_str, expected_func in [
            ("trim(name)", "trim"),
            ("trim(both ' ' from name)", "trim"),
            ("trim(leading 'x' from name)", "ltrim"),
            ("trim(trailing from name)", "rtrim"),
            ("ltrim(name)", "ltrim"),
            ("rtrim(name)", "rtrim"),
        ]:
            res = embedded_parse_expr(parse_one(output_str))
            assert res.function_expr.func_name == expected_func, f"{output_str}: got {res.function_expr.func_name}"
