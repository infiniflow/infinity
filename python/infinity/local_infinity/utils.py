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
import sqlglot.expressions as exp
import numpy as np
from infinity.errors import ErrorCode
from infinity.common import InfinityException, SparseVector
from infinity.local_infinity.types import build_result, logic_type_to_dtype
from infinity.utils import binary_exp_to_paser_exp
from infinity.embedded_infinity_ext import WrapParsedExpr, WrapFunctionExpr, WrapColumnExpr, WrapSearchExpr, WrapConstantExpr, ParsedExprType, LiteralType


def traverse_conditions(cons, fn=None):
    if isinstance(cons, exp.Binary):
        parsed_expr = WrapParsedExpr()
        function_expr = WrapFunctionExpr()
        function_expr.func_name = binary_exp_to_paser_exp(
            cons.key)  # key is the function name cover to >, <, >=, <=, =, and, or, etc.

        arguments = []
        for value in cons.hashable_args:
            if fn:
                expr = fn(value)
            else:
                expr = traverse_conditions(value)
            arguments.append(expr)
        function_expr.arguments = arguments

        parsed_expr.type = ParsedExprType.kFunction
        parsed_expr.function_expr = function_expr

        return parsed_expr

    elif isinstance(cons, exp.Column):
        parsed_expr = WrapParsedExpr()
        column_expr = WrapColumnExpr()
        column_name = [cons.alias_or_name]
        if cons.alias_or_name == "*":
            column_expr.star = True
        else:
            column_expr.star = False
        column_expr.names = column_name

        parsed_expr.type = ParsedExprType.kColumn
        parsed_expr.column_expr = column_expr

        return parsed_expr

    elif isinstance(cons, exp.Boolean):
        parsed_expr = WrapParsedExpr()
        constant_expr = WrapConstantExpr()
        constant_expr.literal_type = LiteralType.kBoolean
        constant_expr.bool_value = cons.this
        parsed_expr.type = ParsedExprType.kConstant
        parsed_expr.constant_expr = constant_expr
        return parsed_expr

    elif isinstance(cons, exp.Literal):
        parsed_expr = WrapParsedExpr()
        constant_expr = WrapConstantExpr()

        if cons.is_int:
            constant_expr.literal_type = LiteralType.kInteger
            constant_expr.i64_value = int(cons.output_name)
        elif cons.is_number:
            constant_expr.literal_type = LiteralType.kDouble
            constant_expr.f64_value = float(cons.output_name)
        elif cons.is_string:
            constant_expr.literal_type = LiteralType.kString
            constant_expr.str_value = cons.output_name
        else:
            raise Exception(f"unknown literal type: {cons}")

        parsed_expr.type = ParsedExprType.kConstant
        parsed_expr.constant_expr = constant_expr
        return parsed_expr

    elif isinstance(cons, exp.Paren):
        for value in cons.hashable_args:
            return traverse_conditions(value)

    elif isinstance(cons, exp.Neg):
        parsed_expr = WrapParsedExpr()
        if isinstance(cons.hashable_args[0], exp.Literal):
            constant_expr = WrapConstantExpr()
            if cons.hashable_args[0].is_int:
                constant_expr.literal_type = LiteralType.kInteger
                constant_expr.i64_value = -int(cons.hashable_args[0].output_name)
            elif cons.hashable_args[0].is_number:
                constant_expr.literal_type = LiteralType.kDouble
                constant_expr.f64_value = -float(cons.hashable_args[0].output_name)
            else:
                raise Exception(f"unknown literal type: {cons}")

            parsed_expr.type = ParsedExprType.kConstant
            parsed_expr.constant_expr = constant_expr

            return parsed_expr
    elif isinstance(cons, exp.Func):
        arguments = []
        for arg in cons.args.values():
            if arg:
                parsed_expr = parse_expr(arg)
                arguments.append(parsed_expr)
        func_expr = WrapFunctionExpr()
        func_expr.func_name = cons.key
        func_expr.arguments = arguments

        parsed_expr = WrapParsedExpr()
        parsed_expr.type = ParsedExprType.kFunction
        parsed_expr.function_expr = func_expr
        return parsed_expr
    else:
        raise Exception(f"unknown condition type: {cons}")


def parse_expr(expr):
    try:
        return traverse_conditions(expr, parse_expr)
    except:
        if isinstance(expr, exp.Star):
            column_expr = WrapColumnExpr()
            column_expr.star = True
            parsed_expr = WrapParsedExpr(ParsedExprType.kColumn)
            parsed_expr.column_expr = column_expr
            return parsed_expr
        else:
            raise Exception(f"unknown expression type: {expr}")


def get_local_constant_expr_from_python_value(value) -> WrapConstantExpr:
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
    constant_expression = WrapConstantExpr()
    match value:
        case str():
            constant_expression.literal_type = LiteralType.kString
            constant_expression.str_value = value
        case bool():
            constant_expression.literal_type = LiteralType.kBoolean
            constant_expression.bool_value = value
        case int():
            constant_expression.literal_type = LiteralType.kInteger
            constant_expression.i64_value = value
        case float():
            constant_expression.literal_type = LiteralType.kDouble
            constant_expression.f64_value = value
        case [int(), *_]:
            constant_expression.literal_type = LiteralType.kIntegerArray
            constant_expression.i64_array_value = value
        case [float(), *_]:
            constant_expression.literal_type = LiteralType.kDoubleArray
            constant_expression.f64_array_value = value
        case [[int(), *_], *_]:
            constant_expression.literal_type = LiteralType.kSubArrayArray
            constant_expression.i64_tensor_value = value
        case [[float(), *_], *_]:
            constant_expression.literal_type = LiteralType.kSubArrayArray
            constant_expression.f64_tensor_value = value
        case [[[int(), *_], *_], *_]:
            constant_expression.literal_type = LiteralType.kSubArrayArray
            constant_expression.i64_tensor_array_value = value
        case [[[float(), *_], *_], *_]:
            constant_expression.literal_type = LiteralType.kSubArrayArray
            constant_expression.f64_tensor_array_value = value
        case SparseVector([int(), *_] as indices, [int(), *_] as values):
            constant_expression.literal_type = LiteralType.kLongSparseArray
            constant_expression.i64_array_idx = indices
            constant_expression.i64_array_value = values
        case SparseVector([int(), *_] as indices, [float(), *_] as values):
            constant_expression.literal_type = LiteralType.kDoubleSparseArray
            constant_expression.i64_array_idx = indices
            constant_expression.f64_array_value = values
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
        raise ValueError(f"{name_type} name must be a string, got {type(name)}")
    if not re.match(r"^[a-zA-Z][a-zA-Z0-9_]*$", name):
        raise ValueError(
            f"{name_type} name '{name}' is not valid. It should start with a letter and can contain only letters, numbers and underscores")
    if len(name) > identifier_limit:
        raise ValueError(f"{name_type} name '{name}' is not of appropriate length")
    if name is None:
        raise ValueError(f"invalid name: {name}")
    if name.isspace():
        raise ValueError(f"{name_type} name cannot be composed of whitespace characters only")
    if name == '':
        raise ValueError(f"invalid name: {name}")
    if name == ' ':
        raise ValueError(f"invalid name: {name}")
    if name.isdigit():
        raise ValueError(f"invalid name: {name}")


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
