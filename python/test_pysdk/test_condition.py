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

    def test_match_dense_knn_params_non_string_thrift(self):
        # Regression test (thrift SDK): match_dense crashed with
        # AttributeError: 'int' object has no attribute 'lower' when a
        # knn_params value was not a string (e.g. {"ef": 200}).
        from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder

        qb = InfinityThriftQueryBuilder(table=None)
        qb.match_dense("v", [1.0, 2.0], "float", "l2", 5, {"ef": 200})
        params = qb._search.match_exprs[0].match_vector_expr.opt_params
        assert [(p.param_name, p.param_value) for p in params] == [("ef", "200")]

    def test_match_dense_knn_params_non_string_embedded(self, request):
        # Regression test (embedded SDK): same AttributeError on non-string
        # knn_params values.
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from infinity_embedded.local_infinity.query_builder import InfinityLocalQueryBuilder

        qb = InfinityLocalQueryBuilder(table=None)
        qb.match_dense("v", [1.0, 2.0], "float", "l2", 5, {"ef": 200})
        params = qb._search.match_exprs[0].knn_expr.opt_params
        assert [(p.param_name, p.param_value) for p in params] == [("ef", "200")]
