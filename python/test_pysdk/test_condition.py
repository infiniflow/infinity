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
            "c3->'a' = 'x'",
            "c3->>'a' = 'x'",
        ]:
            print(cond_str)
            cond = condition(cond_str)
            print(cond)
            res = traverse_conditions(cond)
            print(res)
            assert res

    def test_condition_json_operators(self):
        # -> and ->> must map to the json_extract / json_extract_string
        # server functions with the JSON path passed as a string constant;
        # previously -> recursed until RecursionError and ->> raised
        # "unknown binary expression: jsonextractscalar".
        for cond_str, expected_func in [
            ("c3->'a' = 'x'", "json_extract"),
            ("c3->>'a' = 'x'", "json_extract_string"),
        ]:
            res = traverse_conditions(condition(cond_str))
            args = res.type.function_expr.arguments
            inner = args[0].type.function_expr
            assert inner.function_name == expected_func, f"{cond_str}: got {inner.function_name}"
            path_const = inner.arguments[1].type.constant_expr
            assert path_const.str_value == "$.a", f"{cond_str}: got path {path_const.str_value}"

    def test_json_operators_embedded(self, request):
        # embedded SDK output traversal must support the JSON operators too
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from sqlglot import parse_one
        from infinity_embedded.local_infinity.utils import parse_expr as embedded_parse_expr
        for output_str, expected_func in [
            ("c3->'a'", "json_extract"),
            ("c3->>'a'", "json_extract_string"),
            ("c3->'a'->>'b'", "json_extract_string"),
        ]:
            res = embedded_parse_expr(parse_one(output_str))
            assert res.function_expr.func_name == expected_func, f"{output_str}: got {res.function_expr.func_name}"
            assert res.function_expr.arguments[1].constant_expr.str_value.endswith("b" if output_str.endswith("b'") else "a")
