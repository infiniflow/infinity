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

    def test_output_preserves_string_literal_case(self, request):
        # output() used to lowercase the whole column expression before parsing
        # it, silently corrupting string literals: output(["'ACTIVE'"]) selected
        # 'active', and json_extract(data, '$.UserName') would query key
        # '$.username'. Only the special output tokens (*, _row_id, _score, ...)
        # are meant to be matched case-insensitively.
        from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
        qb = InfinityThriftQueryBuilder(None)
        qb.output(["'ACTIVE'"])
        assert qb._columns[0].type.constant_expr.str_value == "ACTIVE"
        qb = InfinityThriftQueryBuilder(None)
        qb.output(["_SCORE"])
        assert qb._columns[0].type.function_expr.function_name == "score"

        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded part needs the embedded engine")
        from infinity_embedded.local_infinity.query_builder import InfinityLocalQueryBuilder
        qb = InfinityLocalQueryBuilder(None)
        qb.output(["'ACTIVE'"])
        assert qb._columns[0].constant_expr.str_value == "ACTIVE"
