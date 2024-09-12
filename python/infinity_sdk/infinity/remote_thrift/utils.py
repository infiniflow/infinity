# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import re
import functools
import inspect
import pandas as pd
import polars as pl
from sqlglot import condition
import sqlglot.expressions as exp
import numpy as np
import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.remote_thrift.types import build_result, logic_type_to_dtype
from infinity.utils import binary_exp_to_paser_exp
from infinity.common import InfinityException, SparseVector
from infinity.errors import ErrorCode


def traverse_conditions(cons, fn=None) -> ttypes.ParsedExpr:
    if isinstance(cons, exp.Binary):
        parsed_expr = ttypes.ParsedExpr()
        function_expr = ttypes.FunctionExpr()
        function_expr.function_name = binary_exp_to_paser_exp(
            cons.key)  # key is the function name cover to >, <, >=, <=, =, and, or, etc.

        arguments = []
        for value in cons.hashable_args:
            if fn:
                expr = fn(value)
            else:
                expr = traverse_conditions(value)
            arguments.append(expr)
        function_expr.arguments = arguments

        parser_expr_type = ttypes.ParsedExprType()
        parser_expr_type.function_expr = function_expr

        parsed_expr.type = parser_expr_type

        return parsed_expr
    elif isinstance(cons, exp.Not):
        parsed_expr = ttypes.ParsedExpr()
        function_expr = ttypes.FunctionExpr()
        function_expr.function_name = "not"
        arguments = []
        for value in cons.hashable_args:
            if fn:
                expr = fn(value)
            else:
                expr = traverse_conditions(value)
            arguments.append(expr)
        function_expr.arguments = arguments
        parser_expr_type = ttypes.ParsedExprType()
        parser_expr_type.function_expr = function_expr
        parsed_expr.type = parser_expr_type
        return parsed_expr
    elif isinstance(cons, exp.Column):

        match cons.alias_or_name:
            case "_row_id":
                func_expr = ttypes.FunctionExpr(function_name="row_id", arguments=[])
                expr_type = ttypes.ParsedExprType(function_expr=func_expr)
                parsed_expr = ttypes.ParsedExpr(type=expr_type)
                return parsed_expr
            case "_score":
                func_expr = ttypes.FunctionExpr(function_name="score", arguments=[])
                expr_type = ttypes.ParsedExprType(function_expr=func_expr)
                parsed_expr = ttypes.ParsedExpr(type=expr_type)
                return parsed_expr
            case "_similarity":
                func_expr = ttypes.FunctionExpr(function_name="similarity", arguments=[])
                expr_type = ttypes.ParsedExprType(function_expr=func_expr)
                parsed_expr = ttypes.ParsedExpr(type=expr_type)
                return parsed_expr
            case "_distance":
                func_expr = ttypes.FunctionExpr(function_name="distance", arguments=[])
                expr_type = ttypes.ParsedExprType(function_expr=func_expr)
                parsed_expr = ttypes.ParsedExpr(type=expr_type)
                return parsed_expr
            case _:
                parsed_expr = ttypes.ParsedExpr()
                column_expr = ttypes.ColumnExpr()
                column_name = [cons.alias_or_name]
                if cons.alias_or_name == "*":
                    column_expr.star = True
                else:
                    column_expr.star = False
                column_expr.column_name = column_name

                parser_expr_type = ttypes.ParsedExprType()
                parser_expr_type.column_expr = column_expr

                parsed_expr.type = parser_expr_type
                return parsed_expr

    elif isinstance(cons, exp.Boolean):
        parsed_expr = ttypes.ParsedExpr()
        constant_expr = ttypes.ConstantExpr()
        constant_expr.literal_type = ttypes.LiteralType.Boolean
        constant_expr.bool_value = cons.this

        parser_expr_type = ttypes.ParsedExprType()
        parser_expr_type.constant_expr = constant_expr

        parsed_expr.type = parser_expr_type
        return parsed_expr

    elif isinstance(cons, exp.Literal):
        parsed_expr = ttypes.ParsedExpr()
        constant_expr = ttypes.ConstantExpr()

        if cons.is_int:
            constant_expr.literal_type = ttypes.LiteralType.Int64
            constant_expr.i64_value = int(cons.output_name)
        elif cons.is_number:
            constant_expr.literal_type = ttypes.LiteralType.Double
            constant_expr.f64_value = float(cons.output_name)
        elif cons.is_string:
            constant_expr.literal_type = ttypes.LiteralType.String
            constant_expr.str_value = cons.output_name
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, f"Unknown literal type: {cons}")

        parser_expr_type = ttypes.ParsedExprType()
        parser_expr_type.constant_expr = constant_expr

        parsed_expr.type = parser_expr_type
        return parsed_expr

    elif isinstance(cons, exp.Paren):
        for value in cons.hashable_args:
            return traverse_conditions(value)
    elif isinstance(cons, exp.Neg):
        parsed_expr = ttypes.ParsedExpr()
        if isinstance(cons.hashable_args[0], exp.Literal):
            constant_expr = ttypes.ConstantExpr()
            if cons.hashable_args[0].is_int:
                constant_expr.literal_type = ttypes.LiteralType.Int64
                constant_expr.i64_value = -int(cons.hashable_args[0].output_name)
            elif cons.hashable_args[0].is_number:
                constant_expr.literal_type = ttypes.LiteralType.Double
                constant_expr.f64_value = -float(cons.hashable_args[0].output_name)
            else:
                raise InfinityException(ErrorCode.INVALID_EXPRESSION, f"unknown literal type: {cons}")

            parser_expr_type = ttypes.ParsedExprType()
            parser_expr_type.constant_expr = constant_expr
            parsed_expr.type = parser_expr_type

            return parsed_expr
    elif isinstance(cons, exp.Anonymous):
        arguments = []
        for arg in cons.args['expressions']:
            if arg:
                arguments.append(parse_expr(arg))
        func_expr = ttypes.FunctionExpr(
            function_name=cons.args['this'],
            arguments=arguments
        )
        expr_type = ttypes.ParsedExprType(function_expr=func_expr)
        parsed_expr = ttypes.ParsedExpr(type=expr_type)
        return parsed_expr
    elif isinstance(cons, exp.Func):
        arguments = []
        for arg in cons.args.values():
            if arg:
                arguments.append(parse_expr(arg))
        func_expr = ttypes.FunctionExpr(
            function_name=cons.key,
            arguments=arguments
        )
        expr_type = ttypes.ParsedExprType(function_expr=func_expr)
        parsed_expr = ttypes.ParsedExpr(type=expr_type)
        return parsed_expr
    else:
        raise InfinityException(ErrorCode.INVALID_EXPRESSION, f"unknown condition type: {cons}")


def parse_expr(expr) -> ttypes.ParsedExpr:
    try:
        return traverse_conditions(expr, parse_expr)
    except:
        if isinstance(expr, exp.Star):
            column_expr = ttypes.ColumnExpr(
                star=True,
                column_name=[]
            )
            expr_type = ttypes.ParsedExprType(column_expr=column_expr)
            parsed_expr = ttypes.ParsedExpr(type=expr_type)
            return parsed_expr
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, f"unknown expression type: {expr}")


def get_search_optional_filter_from_opt_params(opt_params: dict):
    optional_filter = None
    k_to_pop = []
    for k, v in opt_params.items():
        if k.lower() == "filter":
            if optional_filter is not None:
                raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                        "Only one filter is allowed in search optional parameters")
            if not isinstance(v, str):
                raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                        f"Invalid filter expression '{v}', type should be string, but get {type(v)}")
            optional_filter = traverse_conditions(condition(v))
            k_to_pop.append(k)
    for k in k_to_pop:
        opt_params.pop(k)
    return optional_filter


def get_remote_constant_expr_from_python_value(value) -> ttypes.ConstantExpr:
    # convert numpy types
    if isinstance(value, np.integer):
        value = int(value)
    elif isinstance(value, np.floating):
        value = float(value)
    elif isinstance(value, list) and isinstance(value[0], np.ndarray):
        if value[0].ndim <= 2:
            value = [x.tolist() for x in value]
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                    f"Invalid list member type: {type(value[0])}, ndarray dimension > 2")
    elif isinstance(value, np.ndarray):
        if value.ndim <= 2:
            value = value.tolist()
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                    f"Invalid list type: {type(value)}, ndarray dimension > 2")
    # match ConstantExpr
    match value:
        case str():
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.String, str_value=value)
        case bool():
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Boolean, bool_value=value)
        case int():
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64, i64_value=value)
        case float():
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Double, f64_value=value)
        case [int(), *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.IntegerArray,
                                                      i64_array_value=value)
        case [float(), *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.DoubleArray,
                                                      f64_array_value=value)
        case [[int(), *_], *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.IntegerTensor,
                                                      i64_tensor_value=value)
        case [[float(), *_], *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.DoubleTensor,
                                                      f64_tensor_value=value)
        case [[[int(), *_], *_], *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.IntegerTensorArray,
                                                      i64_tensor_array_value=value)
        case [[[float(), *_], *_], *_]:
            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.DoubleTensorArray,
                                                      f64_tensor_array_value=value)
        case SparseVector([int(), *_] as indices, [int(), *_] as values):
            constant_expression = ttypes.ConstantExpr(
                literal_type=ttypes.LiteralType.SparseIntegerArray,
                i64_array_idx=indices,
                i64_array_value=values)
        case SparseVector([int(), *_] as indices, [float(), *_] as values):
            constant_expression = ttypes.ConstantExpr(
                literal_type=ttypes.LiteralType.SparseDoubleArray,
                i64_array_idx=indices,
                f64_array_value=values)
        case dict():
            if len(value) == 0:
                raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Empty sparse vector")
            match next(iter(value.values())):
                case int():
                    constant_expression = ttypes.ConstantExpr(
                        literal_type=ttypes.LiteralType.SparseIntegerArray,
                        i64_array_idx=[int(k) for k in value.keys()],
                        i64_array_value=[int(v) for v in value.values()])
                case float():
                    constant_expression = ttypes.ConstantExpr(
                        literal_type=ttypes.LiteralType.SparseDoubleArray,
                        i64_array_idx=[int(k) for k in value.keys()],
                        f64_array_value=[float(v) for v in value.values()])
                case _:
                    raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                            f"Invalid sparse vector value type: {type(next(iter(value.values())))}")
        case _:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, f"Invalid constant type: {type(value)}")
    return constant_expression


# invalid_name_array = [
#     [],
#     (),
#     {},
#     1,
#     1.1,
#     '',
#     ' ',
#     '12',
#     'name-12',
#     '12name',
#     '数据库名',
#     ''.join('x' for i in range(identifier_limit + 1)),
#     None,
# ]
identifier_limit = 65536


def check_valid_name(name, name_type: str = "Table"):
    if not isinstance(name, str):
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name must be a string, got {type(name)}")
    if not re.match(r"^[a-zA-Z][a-zA-Z0-9_]*$", name):
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name '{name}' is not valid. It should start with a letter and can contain only letters, numbers and underscores")
    if len(name) > identifier_limit:
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME,
                                f"{name_type} name '{name}' is not of appropriate length")
    if name is None:
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name.isspace():
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"{name_type} name cannot be composed of whitespace characters only")
    if name == '':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name == ' ':
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")
    if name.isdigit():
        raise InfinityException(ErrorCode.INVALID_IDENTIFIER_NAME, f"invalid name: {name}")


def name_validity_check(arg_name: str, name_type: str = "Table"):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            if arg_name in kwargs:
                name = kwargs[arg_name]
            else:
                arg_names = list(inspect.signature(func).parameters.keys())
                name = args[arg_names.index(arg_name)]

            try:
                check_valid_name(name, name_type)
                return func(*args, **kwargs)
            except ValueError as e:
                raise

        return wrapper

    return decorator


def select_res_to_polars(res) -> pl.DataFrame:
    df_dict = {}
    data_dict, data_type_dict = build_result(res)
    for k, v in data_dict.items():
        data_series = pd.Series(v, dtype=logic_type_to_dtype(data_type_dict[k]))
        df_dict[k] = data_series

    return pl.from_pandas(pd.DataFrame(df_dict))
