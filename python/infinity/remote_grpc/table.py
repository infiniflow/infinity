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

from abc import ABC
import struct
from infinity.query import Query, InfinityVectorQueryBuilder
from infinity.table import Table
from infinity.remote_grpc.grpc_pb.infinity_grpc_pb2_grpc import *
from infinity.remote_grpc.grpc_pb.infinity_grpc_pb2 import *
from typing import Optional, Union, Dict, Any
from sqlglot import condition, expressions as exp


class RemoteTable(Table, ABC):

    def __init__(self, conn, db_name, table_name):
        self._conn = conn
        self._db_name = db_name
        self._table_name = table_name

    def create_index(self, index_name: str, column_names: list[str], method_type: str,
                     index_para_list: list[dict[str, Union[str, int, float]]],
                     options=None):

        index_name = index_name.strip()
        column_names: list[str] = [column_name.strip() for column_name in column_names]
        method_type = method_type.strip()
        index_para_list_to_use: InitParameter = []

        for index_para in index_para_list:
            for index, (key, value) in enumerate(index_para.items()):
                proto_index_para = InitParameter()
                proto_index_para.para_name = key
                proto_index_para.para_value = str(value)
                index_para_list_to_use.append(proto_index_para)

        return self._conn.client.create_index(db_name=self._db_name,
                                              table_name=self._table_name,
                                              index_name=index_name,
                                              column_names=column_names,
                                              method_type=method_type,
                                              index_para_list=index_para_list_to_use,
                                              options=None)

    def drop_index(self, index_name: str):
        return self._conn.client.drop_index(db_name=self._db_name, table_name=self._table_name,
                                            index_name=index_name)

    def insert(self, data: list[dict[str, Union[str, int, float]]]):

        db_name = self._db_name
        table_name = self._table_name
        column_names: list[str] = []
        fields: list[Field] = []
        for row in data:
            column_names = list(row.keys())

            field = Field()
            for index, (column_name, value) in enumerate(row.items()):
                constant_expression = ConstantExpr()
                if isinstance(value, str):
                    constant_expression.literal_type = ConstantExpr.LiteralType.kString
                    constant_expression.str_value = value
                elif isinstance(value, int):
                    constant_expression.literal_type = ConstantExpr.LiteralType.kInt64
                    constant_expression.i64_value = value
                elif isinstance(value, float):
                    constant_expression.literal_type = ConstantExpr.LiteralType.kDouble
                    constant_expression.f64_value = value
                else:
                    raise Exception("Invalid constant expression")
                paser_expr = ParsedExpr()
                paser_expr.constant_expr.CopyFrom(constant_expression)
                field.parse_exprs.append(paser_expr)

            fields.append(field)

        # print(db_name, table_name, column_names, fields)
        return self._conn.client.insert(db_name=db_name, table_name=table_name, column_names=column_names,
                                        fields=fields)

    def import_data(self, file_path: str, options=None):

        return self._conn.client.import_data(db_name=self._db_name, table_name=self._table_name, file_path=file_path,
                                             import_options=options)

    def delete(self, condition):
        pass

    def update(self, condition, data):
        pass

    def search(
            self,
            query: Optional[Union[str]] = None,
    ) -> InfinityVectorQueryBuilder:
        return InfinityVectorQueryBuilder.create(
            table=self,
            query=query,
            vector_column_name="",
        )

    def _execute_query(self, query: Query) -> Dict[str, Any]:
        # process select_list
        global covered_column_vector, where_expr
        select_list: list[ParsedExpr] = []
        group_by_list: list[ParsedExpr] = []


        for column in query.columns:
            column_expr = ColumnExpr()

            if column == "*":
                column_expr.star = True
            else:
                column_expr.star = False
                column_expr.column_name.append(column)

            paser_expr = ParsedExpr()
            paser_expr.column_expr.CopyFrom(column_expr)

            select_list.append(paser_expr)

        # process where_expr

        # str to ParsedExpr
        from sqlglot import condition
        if query.filter is not None:
            where_expr = traverse_conditions(condition(query.filter))

        # process limit_expr and offset_expr
        limit_expr = ParsedExpr()
        offset_expr = ParsedExpr()
        if query.limit is not None:
            limit_expr.constant_expr.literal_type = ConstantExpr.LiteralType.kInt64
            limit_expr.constant_expr.i64_value = query.limit
        if query.offset is not None:
            offset_expr.constant_expr.literal_type = ConstantExpr.LiteralType.kInt64
            offset_expr.constant_expr.i64_value = query.offset

        res = self._conn.client.select(db_name=self._db_name,
                                       table_name=self._table_name,
                                       select_list=select_list,
                                       where_expr=where_expr,
                                       group_by_list=group_by_list,
                                       limit_expr=limit_expr,
                                       offset_expr=offset_expr,
                                       search_expr=None)

        # process the results
        results = dict()
        for column_def in res.column_defs:
            column_name = column_def.name
            column_id = column_def.id
            column_field = res.column_fields[column_id]
            column_type = column_field.column_type
            column_vector = column_field.column_vector
            length = len(column_vector)
            if column_type == ColumnType.kColumnInt32:
                value_list = struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.kColumnInt64:
                value_list = struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.kColumnFloat:
                value_list = struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.kColumnDouble:
                value_list = struct.unpack('<{}d'.format(len(column_vector) // 8), column_vector)
                results[column_name] = value_list
            else:
                raise Exception(f"unknown column type: {column_type}")

        return results
        # todo: how to convert bytes to string?


def traverse_conditions(cons) -> ParsedExpr:
    if isinstance(cons, exp.Binary):
        parsed_expr = ParsedExpr()
        function_expr = FunctionExpr()
        function_expr.function_name = binary_exp_to_paser_exp(
            cons.key)  # key is the function name cover to >, <, =, and, or, etc.

        for value in cons.hashable_args:
            expr = traverse_conditions(value)
            function_expr.arguments.append(expr)

        parsed_expr.function_expr.CopyFrom(function_expr)
        return parsed_expr

    elif isinstance(cons, exp.Column):
        parsed_expr = ParsedExpr()
        column_expr = ColumnExpr()
        column_expr.column_name.append(cons.alias_or_name)

        parsed_expr.column_expr.CopyFrom(column_expr)
        return parsed_expr

    elif isinstance(cons, exp.Literal):
        parsed_expr = ParsedExpr()
        constant_expr = ConstantExpr()

        if cons.is_int:
            constant_expr.literal_type = ConstantExpr.LiteralType.kInt64
            constant_expr.i64_value = int(cons.output_name)
        elif cons.is_number:
            constant_expr.literal_type = ConstantExpr.LiteralType.kDouble
            constant_expr.f64_value = float(cons.output_name)
        else:
            raise Exception(f"unknown literal type: {cons}")

        parsed_expr.constant_expr.CopyFrom(constant_expr)
        return parsed_expr

    elif isinstance(cons, exp.Paren):
        for value in cons.hashable_args:
            traverse_conditions(value)
    else:
        raise Exception(f"unknown condition: {cons}")


def binary_exp_to_paser_exp(binary_expr_key) -> str:
    if binary_expr_key == "eq":
        return "="
    elif binary_expr_key == "gt":
        return ">"
    elif binary_expr_key == "lt":
        return "<"
    elif binary_expr_key == "gte":
        return ">="
    elif binary_expr_key == "lte":
        return "<="
    elif binary_expr_key == "neq":
        return "!="
    elif binary_expr_key == "and":
        return "and"
    elif binary_expr_key == "or":
        return "or"
    else:
        raise Exception(f"unknown binary expression: {binary_expr_key}")
