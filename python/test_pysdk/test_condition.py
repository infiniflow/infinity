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

    def test_remote_constant_expr_mixed_numeric_lists(self):
        # Regression test (thrift SDK): constant-expression and match_sparse
        # type dispatch looked at the first element only, so a mixed
        # int/float list like [1, 2.5] became an integer array, a sparse dict
        # {1: 5, 2: 0.5} was truncated by int(v) in the SDK itself, and
        # SparseVector([1, 2], [5, 0.5]) became a sparse integer array.
        from infinity.common import SparseVector
        from infinity.remote_thrift.infinity_thrift_rpc import ttypes
        from infinity.remote_thrift.utils import get_remote_constant_expr_from_python_value

        L = ttypes.LiteralType
        res = get_remote_constant_expr_from_python_value([1, 2.5])
        assert res.literal_type == L.DoubleArray
        assert res.f64_array_value == [1.0, 2.5]

        res = get_remote_constant_expr_from_python_value([[1, 2.5]])
        assert res.literal_type == L.DoubleTensor
        assert res.f64_tensor_value == [[1.0, 2.5]]

        res = get_remote_constant_expr_from_python_value({1: 5, 2: 0.5})
        assert res.literal_type == L.SparseDoubleArray
        assert res.i64_array_idx == [1, 2]
        assert res.f64_array_value == [5.0, 0.5]

        res = get_remote_constant_expr_from_python_value(SparseVector([1, 2], [5, 0.5]))
        assert res.literal_type == L.SparseDoubleArray
        assert res.f64_array_value == [5.0, 0.5]

        # all-int inputs keep their integer types
        res = get_remote_constant_expr_from_python_value([1, 2])
        assert res.literal_type == L.IntegerArray
        assert res.i64_array_value == [1, 2]

        res = get_remote_constant_expr_from_python_value({1: 5, 2: 6})
        assert res.literal_type == L.SparseIntegerArray
        assert res.i64_array_value == [5, 6]

    def test_match_sparse_mixed_numeric_values_thrift(self):
        # Same first-element dispatch in the thrift match_sparse builder:
        # {1: 5, 2: 0.5} became a sparse-integer query vector with 0.5
        # truncated to 0.
        from infinity.common import SparseVector
        from infinity.remote_thrift.infinity_thrift_rpc import ttypes
        from infinity.remote_thrift.types import make_match_sparse_expr

        L = ttypes.LiteralType
        expr = make_match_sparse_expr("c1", {1: 5, 2: 0.5}, "ip", 3)
        assert expr.query_sparse_expr.literal_type == L.SparseDoubleArray
        assert expr.query_sparse_expr.f64_array_value == [5.0, 0.5]

        expr = make_match_sparse_expr("c1", SparseVector([1, 2], [5, 0.5]), "ip", 3)
        assert expr.query_sparse_expr.literal_type == L.SparseDoubleArray
        assert expr.query_sparse_expr.f64_array_value == [5.0, 0.5]
