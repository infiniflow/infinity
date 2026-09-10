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

    def test_constant_expr_mixed_numeric_lists(self, request):
        # Regression test (embedded SDK): a mixed int/float list used to be
        # labeled an integer array (dispatch looked at the first element only),
        # so 2.5 in [1, 2.5] lost its fractional part. The same first-element
        # dispatch truncated sparse values: {1: 5, 2: 0.5} and
        # SparseVector([1, 2], [5, 0.5]) were stored as long-sparse with the
        # 0.5 truncated to 0 (dict inserts were truncated by int(v) in the SDK
        # itself).
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from infinity_embedded.common import SparseVector
        from infinity_embedded.embedded_infinity_ext import LiteralType
        from infinity_embedded.local_infinity.utils import get_local_constant_expr_from_python_value

        res = get_local_constant_expr_from_python_value([1, 2.5])
        assert res.literal_type == LiteralType.kDoubleArray
        assert res.f64_array_value == [1.0, 2.5]

        res = get_local_constant_expr_from_python_value([1, 2])
        assert res.literal_type == LiteralType.kIntegerArray
        assert res.i64_array_value == [1, 2]

        # tensors get the same all-elements dispatch
        res = get_local_constant_expr_from_python_value([[1, 2.5]])
        assert res.literal_type == LiteralType.kSubArrayArray
        assert res.f64_tensor_value == [[1.0, 2.5]]

        res = get_local_constant_expr_from_python_value([[[1, 2.5]]])
        assert res.literal_type == LiteralType.kSubArrayArray
        assert res.f64_tensor_array_value == [[[1.0, 2.5]]]

        res = get_local_constant_expr_from_python_value([[1, 2]])
        assert res.literal_type == LiteralType.kSubArrayArray
        assert res.i64_tensor_value == [[1, 2]]

        res = get_local_constant_expr_from_python_value({1: 5, 2: 0.5})
        assert res.literal_type == LiteralType.kDoubleSparseArray
        assert res.i64_array_idx == [1, 2]
        assert res.f64_array_value == [5.0, 0.5]

        res = get_local_constant_expr_from_python_value({1: 5, 2: 6})
        assert res.literal_type == LiteralType.kLongSparseArray
        assert res.i64_array_value == [5, 6]

        res = get_local_constant_expr_from_python_value(SparseVector([1, 2], [5, 0.5]))
        assert res.literal_type == LiteralType.kDoubleSparseArray
        assert res.f64_array_value == [5.0, 0.5]

    def test_match_sparse_mixed_numeric_values(self, request):
        # Same first-element dispatch in match_sparse: {1: 5, 2: 0.5} became a
        # long-sparse query vector with 0.5 truncated to 0, silently changing
        # the scored results.
        if not request.config.getoption("--local-infinity"):
            pytest.skip("embedded-only regression test")
        from infinity_embedded.common import SparseVector
        from infinity_embedded.embedded_infinity_ext import LiteralType
        from infinity_embedded.local_infinity.query_builder import InfinityLocalQueryBuilder

        qb = InfinityLocalQueryBuilder(table=None)
        qb.match_sparse("c1", {1: 5, 2: 0.5}, "ip", 3)
        sparse_expr = qb._search.match_exprs[0].match_sparse_expr.sparse_expr
        assert sparse_expr.literal_type == LiteralType.kDoubleSparseArray
        assert sparse_expr.f64_array_value == [5.0, 0.5]

        qb = InfinityLocalQueryBuilder(table=None)
        qb.match_sparse("c1", SparseVector([1, 2], [5, 0.5]), "ip", 3)
        sparse_expr = qb._search.match_exprs[0].match_sparse_expr.sparse_expr
        assert sparse_expr.literal_type == LiteralType.kDoubleSparseArray
        assert sparse_expr.f64_array_value == [5.0, 0.5]
