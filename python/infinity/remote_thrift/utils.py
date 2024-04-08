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

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.remote_thrift.types import build_result, logic_type_to_dtype
from infinity.utils import binary_exp_to_paser_exp


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

        paser_expr_type = ttypes.ParsedExprType()
        paser_expr_type.function_expr = function_expr

        parsed_expr.type = paser_expr_type

        return parsed_expr

    elif isinstance(cons, exp.Column):
        parsed_expr = ttypes.ParsedExpr()
        column_expr = ttypes.ColumnExpr()
        column_name = [cons.alias_or_name]
        if cons.alias_or_name == "*":
            column_expr.star = True
        else:
            column_expr.star = False
        column_expr.column_name = column_name

        paser_expr_type = ttypes.ParsedExprType()
        paser_expr_type.column_expr = column_expr

        parsed_expr.type = paser_expr_type
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
            raise Exception(f"unknown literal type: {cons}")

        paser_expr_type = ttypes.ParsedExprType()
        paser_expr_type.constant_expr = constant_expr

        parsed_expr.type = paser_expr_type
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
                raise Exception(f"unknown literal type: {cons}")

            paser_expr_type = ttypes.ParsedExprType()
            paser_expr_type.constant_expr = constant_expr
            parsed_expr.type = paser_expr_type

            return parsed_expr
    else:
        raise Exception(f"unknown condition type: {cons}")


def parse_expr(expr) -> ttypes.ParsedExpr:
    try:
        return traverse_conditions(expr, parse_expr)
    except:
        if isinstance(expr, exp.Func):
            arguments = []
            for arg in expr.args.values():
                if arg:
                    arguments.append(parse_expr(arg))
            func_expr = ttypes.FunctionExpr(
                function_name=expr.key,
                arguments=arguments
            )
            expr_type = ttypes.ParsedExprType(function_expr=func_expr)
            parsed_expr = ttypes.ParsedExpr(type=expr_type)
            return parsed_expr
        elif isinstance(expr, exp.Star):
            column_expr = ttypes.ColumnExpr(
                star=True,
                column_name=[]
            )
            expr_type = ttypes.ParsedExprType(column_expr=column_expr)
            parsed_expr = ttypes.ParsedExpr(type=expr_type)
            return parsed_expr
        else:
            raise Exception(f"unknown expression type: {expr}")


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
