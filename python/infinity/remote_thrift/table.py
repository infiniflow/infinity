import struct
from abc import ABC
from typing import Optional, Union, List, Dict, Any

from sqlglot import condition, expressions as exp

from infinity.query import Query, InfinityVectorQueryBuilder
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from infinity.table import Table, binary_exp_to_paser_exp


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
        index_para_list_to_use: list[InitParameter] = []

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
                                              option=options)

    def drop_index(self, index_name: str):
        return self._conn.client.drop_index(db_name=self._db_name, table_name=self._table_name,
                                            index_name=index_name)

    def insert(self, data: list[dict[str, Union[str, int, float]]]):
        # [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}]
        db_name = self._db_name
        table_name = self._table_name
        column_names: list[str] = []
        fields: list[Field] = []
        for row in data:
            column_names = list(row.keys())
            parse_exprs = []
            for column_name, value in row.items():
                constant_expression = ConstantExpr()
                if isinstance(value, str):
                    constant_expression.literal_type = LiteralType.String
                    constant_expression.str_value = value
                elif isinstance(value, int):
                    constant_expression.literal_type = LiteralType.Int64
                    constant_expression.i64_value = value
                elif isinstance(value, float):
                    constant_expression.literal_type = LiteralType.Double
                    constant_expression.f64_value = value
                else:
                    raise Exception("Invalid constant expression")
                paser_expr_type = ParsedExprType()
                paser_expr_type.constant_expr = constant_expression

                paser_expr = ParsedExpr()
                paser_expr.type = paser_expr_type
                parse_exprs.append(paser_expr)
            f = Field()
            f.parse_exprs = parse_exprs
            fields.append(f)

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
        select_list: List[ParsedExpr] = []

        for column in query.columns:
            match column:
                case "*":
                    column_expr = ColumnExpr()
                    column_expr.star = True
                    paser_expr_type = ParsedExprType()
                    paser_expr_type.column_expr = column_expr
                    parsed_expr = ParsedExpr()
                    parsed_expr.type = paser_expr_type
                    select_list.append(parsed_expr)
                case _:
                    select_list.append(traverse_conditions(condition(column)))

        # process where_expr
        where_expr = ParsedExpr()
        if query.filter is not None:
            where_expr = traverse_conditions(condition(query.filter))

        # process limit_expr and offset_expr
        limit_expr = ParsedExpr()
        offset_expr = ParsedExpr()
        if query.limit is not None:
            constant_exp = ConstantExpr()
            constant_exp.literal_type = LiteralType.Int64

            paser_expr_type = ParsedExprType()
            paser_expr_type.constant_expr = constant_exp
            limit_expr.type = paser_expr_type
            limit_expr.i64_value = query.limit
        if query.offset is not None:
            constant_exp = ConstantExpr()
            constant_exp.literal_type = LiteralType.Int64
            paser_expr_type = ParsedExprType()
            paser_expr_type.constant_expr = constant_exp
            offset_expr.type = paser_expr_type
            offset_expr.i64_value = query.offset

        res = self._conn.client.select(db_name=self._db_name,
                                       table_name=self._table_name,
                                       select_list=select_list,
                                       where_expr=where_expr,
                                       group_by_list=None,
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
            # print(column_name, column_type, column_vector)

            if column_type == ColumnType.ColumnInt32:
                value_list = struct.unpack('<{}i'.format(len(column_vector) // 4), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.ColumnInt64:
                value_list = struct.unpack('<{}q'.format(len(column_vector) // 8), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.ColumnFloat32:
                value_list = struct.unpack('<{}f'.format(len(column_vector) // 4), column_vector)
                results[column_name] = value_list
            elif column_type == ColumnType.ColumnFloat64:
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

        arguments = []
        for value in cons.hashable_args:
            expr = traverse_conditions(value)
            arguments.append(expr)
        function_expr.arguments = arguments

        paser_expr_type = ParsedExprType()
        paser_expr_type.function_expr = function_expr

        parsed_expr.type = paser_expr_type

        return parsed_expr

    elif isinstance(cons, exp.Column):
        parsed_expr = ParsedExpr()
        column_expr = ColumnExpr()
        column_name = [cons.alias_or_name]
        if cons.alias_or_name == "*":
            column_expr.star = True
        else:
            column_expr.star = False
        column_expr.column_name = column_name

        paser_expr_type = ParsedExprType()
        paser_expr_type.column_expr = column_expr

        parsed_expr.type = paser_expr_type
        return parsed_expr

    elif isinstance(cons, exp.Literal):
        parsed_expr = ParsedExpr()
        constant_expr = ConstantExpr()

        if cons.is_int:
            constant_expr.literal_type = LiteralType.Int64
            constant_expr.i64_value = int(cons.output_name)
        elif cons.is_number:
            constant_expr.literal_type = LiteralType.Double
            constant_expr.f64_value = float(cons.output_name)
        else:
            raise Exception(f"unknown literal type: {cons}")

        paser_expr_type = ParsedExprType()
        paser_expr_type.constant_expr = constant_expr

        parsed_expr.type = paser_expr_type
        return parsed_expr

    elif isinstance(cons, exp.Paren):
        for value in cons.hashable_args:
            traverse_conditions(value)
    else:
        raise Exception(f"unknown condition: {cons}")
