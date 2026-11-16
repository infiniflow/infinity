import infinity
import polars as pl
import pytest
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.infinity_http import infinity_http
from infinity.table import ExplainType

from common import common_values


@pytest.mark.usefixtures("http")
@pytest.mark.usefixtures("suffix")
def _plan_text(result) -> str:
    """Return the plan text of an explain result.

    The thrift paths return a DataFrame whose column is named "Physical Plan"
    on the C++ engine and "explain" on the Rust one, so read the cells by
    position. Iterating them also avoids ``str(frame)``, which elides rows
    with "…" once a plan outgrows the display budget and so hides the
    operator being asserted on. The HTTP client has already joined the plan
    into a plain string and is returned as-is.
    """
    if isinstance(result, str):
        return result
    return "\n".join(str(value) for row in result.iter_rows() for value in row)


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

    def test_explain(self, suffix):
        """
        # Analyze = 1
        # Ast = 2
        # UnOpt = 3
        # Opt = 4
        # Physical = 5
        # Pipeline = 6
        # Fragment = 7
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_default" + suffix, ConflictType.Ignore)
        table = db_obj.create_table(
            "test_explain_default" + suffix,
            {
                "c1": {"type": "varchar", "constraints": ["primary key"]},
                "c2": {"type": "float"},
            },
            ConflictType.Error,
        )
        assert table

        table.insert({"c1": "hello", "c2": 1.0})
        table.insert({"c1": "world", "c2": 2.0})
        table.insert({"c1": "hello", "c2": 3.0})

        with pl.Config(fmt_str_lengths=1000):
            res = table.output(["*"]).explain()
            print(res)

            res = table.output(["*"]).explain(ExplainType.Ast)
            print(res)

            res = table.output(["*"]).explain(ExplainType.UnOpt)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Opt)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Physical)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Pipeline)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Fragment)
            print(res)

            res = table.output(["*"]).explain(ExplainType.Analyze)
            print(res)

        db_obj.drop_table("test_explain_default" + suffix, ConflictType.Error)

    def test_explain_with_match_dense(self, suffix):
        """
        Regression test: explain() must send the search expression to the server.
        The HTTP client used to build the explain request from stale fields
        (_knn/_match/_match_tensor/_match_sparse/_fusion) that were never
        populated, so every match_*/fusion clause was silently dropped and
        the server explained a plain table scan instead of the search query.
        """
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_match_dense" + suffix, ConflictType.Ignore)
        table = db_obj.create_table(
            "test_explain_match_dense" + suffix,
            {
                "c1": {"type": "varchar", "constraints": ["primary key"]},
                "vec": {"type": "vector,4,float"},
            },
            ConflictType.Error,
        )
        assert table

        res = (
            table.output(["c1"])
            .match_dense("vec", [0.1, 0.2, 0.3, 0.4], "float", "l2", 10)
            .explain(ExplainType.Physical)
        )
        # The physical plan of a match_dense query must contain a KNN SCAN
        # operator.
        assert "KNN SCAN" in _plan_text(res)

        db_obj.drop_table("test_explain_match_dense" + suffix, ConflictType.Error)

    def test_explain_group_by_having(self, suffix):
        # explain() must carry group by / having into the explained query
        db_obj = self.infinity_obj.get_database("default_db")
        db_obj.drop_table("test_explain_group_by_having" + suffix, ConflictType.Ignore)
        table = db_obj.create_table(
            "test_explain_group_by_having" + suffix,
            {"c1": {"type": "varchar"}, "c2": {"type": "float"}},
            ConflictType.Error,
        )
        assert table

        table.insert({"c1": "hello", "c2": 1.0})
        table.insert({"c1": "world", "c2": 2.0})
        table.insert({"c1": "hello", "c2": 3.0})

        query = table.output(["c1", "sum(c2)"]).group_by(["c1"]).having("sum(c2) > 0")
        # having must reach the explained query: the thrift builder handed the
        # server a hardcoded having_expr=None, dropping the clause. The builder
        # clears itself inside explain(), so assert the forwarding before asking
        # for the plan. The HTTP client builds its request inline and owns no
        # builder.
        if suffix != "_http":
            assert query.query_builder._having is not None
            assert query.query_builder._groupby is not None

        with pl.Config(fmt_str_lengths=1000):
            res = query.explain(ExplainType.Physical)
            # Read the plan rather than str(): polars elides rows with "…"
            # when the frame is wider than its display budget, which hides
            # the operator this test is looking for.
            plan_text = _plan_text(res)
            print(plan_text)
            # A group-by query explains through an AGGREGATE operator; without
            # forwarding group by the plan is a plain projection.
            assert "AGGREGATE" in plan_text

        db_obj.drop_table("test_explain_group_by_having" + suffix, ConflictType.Error)


def test_parsed_expression_to_string_constants():
    """
    Regression test: parsed_expression_to_string() read constant values from
    expr_type (ParsedExprType) instead of expr_type.constant_expr, so every
    constant literal except Boolean crashed with AttributeError when an
    explain result with a limit/offset/filter was rendered (thrift SDK).
    Two tensor-array branches also used stale field names without the _value
    suffix.
    """
    from infinity.remote_thrift.infinity_thrift_rpc import ttypes
    from infinity.remote_thrift.utils import parsed_expression_to_string

    def mk(literal_type, **kw):
        constant_expr = ttypes.ConstantExpr(literal_type=literal_type, **kw)
        expr_type = ttypes.ParsedExprType()
        expr_type.constant_expr = constant_expr
        expr = ttypes.ParsedExpr()
        expr.type = expr_type
        return expr

    assert (
        parsed_expression_to_string(mk(ttypes.LiteralType.Int64, i64_value=10)) == "10"
    )
    assert (
        parsed_expression_to_string(mk(ttypes.LiteralType.Double, f64_value=1.5))
        == "1.5"
    )
    assert (
        parsed_expression_to_string(mk(ttypes.LiteralType.String, str_value="abc"))
        == "abc"
    )
    assert (
        parsed_expression_to_string(mk(ttypes.LiteralType.Boolean, bool_value=True))
        == "True"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.IntegerArray, i64_array_value=[1, 2])
        )
        == "[1, 2]"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.DoubleTensor, f64_tensor_value=[[1.0, 2.0]])
        )
        == "[[1.0, 2.0]]"
    )
    assert (
        parsed_expression_to_string(
            mk(
                ttypes.LiteralType.IntegerTensorArray,
                i64_tensor_array_value=[[[1, 2], [3, 4]]],
            )
        )
        == "[[[1, 2], [3, 4]]]"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.DoubleArray, f64_array_value=[1.5, 2.5])
        )
        == "[1.5, 2.5]"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.IntegerTensor, i64_tensor_value=[[1, 2]])
        )
        == "[[1, 2]]"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.DoubleTensorArray, f64_tensor_array_value=[[[1.5]]])
        )
        == "[[[1.5]]]"
    )
    assert (
        parsed_expression_to_string(
            mk(ttypes.LiteralType.SparseIntegerArray, i64_array_idx=[0, 3])
        )
        == "[0, 3]"
    )
