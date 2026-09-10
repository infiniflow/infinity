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

    def test_search_opt_params_not_mutated_thrift(self):
        # Regression test (thrift SDK): match_* used to pop "filter" out of
        # the caller's options dict, silently changing data owned by the
        # caller. The filter must still land on the built search expression.
        from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder

        qb = InfinityThriftQueryBuilder(table=None)
        opts = {"filter": "c2 > 1", "operator": "and"}
        qb.match_text("c1", "hello", 3, opts)
        assert opts == {"filter": "c2 > 1", "operator": "and"}
        match_expr = qb._search.match_exprs[0].match_text_expr
        assert match_expr.filter_expr is not None
        assert match_expr.options_text == "topn=3;operator=and"

        qb = InfinityThriftQueryBuilder(table=None)
        opts = {"filter": "c2 > 1", "ef": "200"}
        qb.match_dense("v", [1.0, 2.0], "float", "l2", 5, opts)
        assert opts == {"filter": "c2 > 1", "ef": "200"}
        knn_expr = qb._search.match_exprs[0].match_vector_expr
        assert knn_expr.filter_expr is not None

    def test_search_opt_params_not_mutated_embedded(self, request):
        # Regression test (embedded SDK): same caller-dict mutation as the
        # thrift SDK - "filter" was popped out of the options dict.
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from infinity_embedded.local_infinity.query_builder import InfinityLocalQueryBuilder

        qb = InfinityLocalQueryBuilder(table=None)
        opts = {"filter": "c2 > 1", "operator": "and"}
        qb.match_text("c1", "hello", 3, opts)
        assert opts == {"filter": "c2 > 1", "operator": "and"}
        match_expr = qb._search.match_exprs[0].match_expr
        assert match_expr.filter_expr is not None
        assert match_expr.options_text == "topn=3;operator=and"
