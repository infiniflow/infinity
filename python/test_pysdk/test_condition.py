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
            "c1 BETWEEN 1 AND 3",
            "c1 NOT BETWEEN 1 AND 3",
            "c1 BETWEEN 1 AND 3 AND c2 < 2",
            "c1 BETWEEN SYMMETRIC 3 AND 1",
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


def test_condition_like_function_names():
    # sqlglot >= 30.x parses NOT LIKE as a Like node with negate=True
    # rather than a Not(Like) wrapper; the traversal must honor the flag.
    cases = {
        "c1 LIKE '%test%'": "like",
        "c1 NOT LIKE '%test%'": "not_like",
        "c1 LIKE '%test%' ESCAPE '!'": "like",
        "c1 NOT LIKE '%test%' ESCAPE '!'": "not_like",
    }
    for cond_str, expected_func in cases.items():
        from infinity.remote_thrift.utils import traverse_conditions as remote_traverse

        res = remote_traverse(condition(cond_str))
        function = res.type.function_expr
        assert function.arguments[0].type.column_expr.column_name == ["c1"]
        assert function.arguments[1].type.constant_expr.str_value == "%test%"
        assert len(function.arguments) == 3
        assert function.arguments[2].type.constant_expr.str_value == (
            "!" if "ESCAPE" in cond_str else "\\"
        )
        func_name = function.function_name
        assert func_name == expected_func, (
            f"{cond_str}: got {func_name}, want {expected_func}"
        )


def test_condition_json_operators():
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
        assert inner.function_name == expected_func, (
            f"{cond_str}: got {inner.function_name}"
        )
        path_const = inner.arguments[1].type.constant_expr
        assert path_const.str_value == "$.a", (
            f"{cond_str}: got path {path_const.str_value}"
        )


def test_json_operators_embedded(request):
    # embedded SDK output traversal must support the JSON operators too
    if not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    from infinity_embedded.local_infinity.utils import parse_expr as embedded_parse_expr
    from sqlglot import parse_one

    for output_str, expected_func in [
        ("c3->'a'", "json_extract"),
        ("c3->>'a'", "json_extract_string"),
        ("c3->'a'->>'b'", "json_extract_string"),
    ]:
        res = embedded_parse_expr(parse_one(output_str))
        assert res.function_expr.func_name == expected_func, (
            f"{output_str}: got {res.function_expr.func_name}"
        )
        assert res.function_expr.arguments[1].constant_expr.str_value.endswith(
            "b" if output_str.endswith("b'") else "a"
        )


@pytest.mark.parametrize("sdk", ["embedded", "thrift"])
@pytest.mark.parametrize("operator", ["->", "->>"])
@pytest.mark.parametrize(
    "key, expected",
    [
        ("a", "$.a"),
        ("O'Brien", "$.O'Brien"),
        ("a b", "$.a b"),
        ("$.a[0]", "$.a[0]"),
    ],
)
def test_json_operator_paths(sdk, operator, key, expected, request):
    """Both SDKs pass decoded, server-compatible paths to JSON functions.

    The right-hand side is built from the expression API instead of parsing
    ``c3->'key'``. sqlglot 28.1.0 renders a key holding an apostrophe as
    ``'O''Brien'`` but cannot tokenize that form back, so building the test
    through SQL text makes it fail on the tokenizer before it reaches the
    path conversion this test is about. A key that is already a JSON path
    gets a ``JSONPath`` node, as the parser would produce for it; every other
    key is a plain ``JSONPathKey``.
    """
    import sqlglot.expressions as exp

    path_expressions = [exp.JSONPathRoot()]
    if "[" in key:
        name, _, index = key.partition("[")
        if name not in ("", "$"):
            path_expressions.append(exp.JSONPathKey(this=name.removeprefix("$.")))
        path_expressions.append(exp.JSONPathSubscript(this=int(index.rstrip("]"))))
    else:
        path_expressions.append(exp.JSONPathKey(this=key))

    node = (exp.JSONExtractScalar if operator == "->>" else exp.JSONExtract)(
        this=exp.column("c3"),
        expression=exp.JSONPath(expressions=path_expressions),
    )

    if sdk == "embedded" and not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    if sdk == "embedded":
        from infinity_embedded.local_infinity.utils import parse_expr

        function = parse_expr(node).function_expr
        path_value = function.arguments[1].constant_expr.str_value
        assert function.arguments[0].column_expr.names == ["c3"]
        name = function.func_name
    else:
        from infinity.remote_thrift.utils import parse_expr

        function = parse_expr(node).type.function_expr
        path_value = function.arguments[1].type.constant_expr.str_value
        assert function.arguments[0].type.column_expr.column_name == ["c3"]
        name = function.function_name
    assert path_value == expected
    assert name == ("json_extract_string" if operator == "->>" else "json_extract")


@pytest.mark.parametrize("sdk", ["embedded", "thrift"])
def test_json_operator_rejects_unrepresentable_key(sdk, request):
    """A literal dotted key must not silently become two server path keys."""
    import sqlglot.expressions as exp

    if sdk == "embedded" and not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    if sdk == "embedded":
        from infinity_embedded.common import InfinityException
        from infinity_embedded.local_infinity.utils import _json_operator_path
    else:
        from infinity.common import InfinityException
        from infinity.remote_thrift.utils import _json_operator_path
    path = exp.JSONPath(expressions=[exp.JSONPathRoot(), exp.JSONPathKey(this="a.b")])
    with pytest.raises(InfinityException):
        _json_operator_path(path)


@pytest.mark.parametrize("sdk", ["embedded", "thrift"])
@pytest.mark.parametrize("operator", ["->", "->>"])
def test_json_operator_chain_uses_one_extraction(sdk, operator, request):
    """A chain must not pass VARCHAR from json_extract to another extractor."""
    from sqlglot import parse_one

    if sdk == "embedded" and not request.config.getoption("--local-infinity"):
        pytest.skip("embedded-only regression test")
    node = parse_one(f"c3->'a'{operator}'b[0]'")
    if sdk == "embedded":
        from infinity_embedded.local_infinity.utils import parse_expr

        function = parse_expr(node).function_expr
        assert function.arguments[0].column_expr.names == ["c3"]
        assert function.arguments[1].constant_expr.str_value == "$.a.b[0]"
    else:
        from infinity.remote_thrift.utils import parse_expr

        function = parse_expr(node).type.function_expr
        assert function.arguments[0].type.column_expr.column_name == ["c3"]
        assert function.arguments[1].type.constant_expr.str_value == "$.a.b[0]"

    def test_remote_constant_expr_mixed_numeric_lists(self):
        # Regression test (thrift SDK): constant-expression and match_sparse
        # type dispatch looked at the first element only, so a mixed
        # int/float list like [1, 2.5] became an integer array, a sparse dict
        # {1: 5, 2: 0.5} was truncated by int(v) in the SDK itself, and
        # SparseVector([1, 2], [5, 0.5]) became a sparse integer array.
        from infinity.common import SparseVector
        from infinity.remote_thrift.infinity_thrift_rpc import ttypes
        from infinity.remote_thrift.utils import (
            get_remote_constant_expr_from_python_value,
        )

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
