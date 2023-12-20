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
import os
from abc import ABC
from typing import Optional, Union, List, Any

from sqlglot import condition

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.common import INSERT_DATA, VEC
from infinity.index import IndexInfo
from infinity.remote_thrift.query_builder import Query, InfinityThriftQueryBuilder
from infinity.remote_thrift.types import build_result
from infinity.remote_thrift.utils import traverse_conditions
from infinity.table import Table


class RemoteTable(Table, ABC):

    def __init__(self, conn, db_name, table_name):
        self._conn = conn
        self._db_name = db_name
        self._table_name = table_name
        self.query_builder = InfinityThriftQueryBuilder(table=self)

    def create_index(self, index_name: str, index_infos: list[IndexInfo], options=None):

        index_name = index_name.strip()

        index_info_list_to_use: list[ttypes.IndexInfo] = []

        for index_info in index_infos:
            index_info_to_use = ttypes.IndexInfo(column_name=index_info.column_name.strip(),
                                                 index_type=index_info.index_type.to_ttype(),
                                                 index_param_list=[init_param.to_ttype() for init_param in
                                                                   index_info.params])

            index_info_list_to_use.append(index_info_to_use)

        return self._conn.create_index(db_name=self._db_name,
                                       table_name=self._table_name,
                                       index_name=index_name,
                                       index_info_list=index_info_list_to_use,
                                       option=options)

    def drop_index(self, index_name: str):
        return self._conn.drop_index(db_name=self._db_name, table_name=self._table_name,
                                     index_name=index_name)

    def insert(self, data: Union[INSERT_DATA, list[INSERT_DATA]]):
        # [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}]
        db_name = self._db_name
        table_name = self._table_name
        column_names: list[str] = []
        fields: list[ttypes.Field] = []

        if isinstance(data, dict):
            data = [data]

        for row in data:
            column_names = list(row.keys())
            parse_exprs = []
            for column_name, value in row.items():
                constant_expression = None
                if isinstance(value, str):
                    constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.String,
                                                              str_value=value)

                elif isinstance(value, int):
                    constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64,
                                                              i64_value=value)

                elif isinstance(value, float):
                    constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Double,
                                                              f64_value=value)
                elif isinstance(value, list):
                    if isinstance(value[0], int):
                        constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.IntegerArray,
                                                                  i64_array_value=value)
                    elif isinstance(value[0], float):
                        constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.DoubleArray,
                                                                  f64_array_value=value)
                else:
                    raise Exception("Invalid constant expression")

                expr_type = ttypes.ParsedExprType(constant_expr=constant_expression)
                paser_expr = ttypes.ParsedExpr(type=expr_type)

                parse_exprs.append(paser_expr)

            field = ttypes.Field(parse_exprs=parse_exprs)
            fields.append(field)

        return self._conn.insert(db_name=db_name, table_name=table_name, column_names=column_names,
                                 fields=fields)

    def import_data(self, file_path: str, options=None):

        options = ttypes.ImportOption()

        total_size = os.path.getsize(file_path)
        chunk_size = 1024 * 1024 * 10  # 10MB
        file_name = os.path.basename(file_path)
        if file_name.endswith('.csv'):
            options.copy_file_type = ttypes.CopyFileType.CSV
            options.delimiter = ","
        elif file_name.endswith('.json'):
            options.copy_file_type = ttypes.CopyFileType.JSON
        elif file_name.endswith('.fvecs'):
            options.copy_file_type = ttypes.CopyFileType.FVECS

        with open(file_path, 'rb') as f:
            file_data = f.read()
            num_chunks = len(file_data) // chunk_size + 1

            for index in range(num_chunks):
                start = index * chunk_size
                end = min((index + 1) * chunk_size, len(file_data))
                chunk_data = file_data[start:end]
                is_last = (index == num_chunks - 1)
                res = self._conn.upload(db_name=self._db_name,
                                        table_name=self._table_name,
                                        file_name=file_name,
                                        data=chunk_data,
                                        index=index,
                                        is_last=is_last,
                                        total_size=total_size)
                match res:
                    case None:
                        raise Exception("upload failed")
                    case _:
                        if not res.success:
                            raise Exception(f"upload failed: {res.error_msg}")
                        if res.error_msg:
                            raise Exception(f"upload failed: {res.error_msg}")
                        if res.can_skip:
                            break

        return self._conn.import_data(db_name=self._db_name,
                                      table_name=self._table_name,
                                      file_name=file_name,
                                      import_options=options)

    def delete(self, cond: Optional[str] = None):
        match cond:
            case None:
                where_expr = None
            case _:
                where_expr = traverse_conditions(condition(cond))
        return self._conn.delete(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr)

    def update(self, cond: Optional[str], data: Optional[list[dict[str, Union[str, int, float]]]]):
        # {"c1": 1, "c2": 1.1}
        match cond:
            case None:
                where_expr = None
            case _:
                where_expr = traverse_conditions(condition(cond))
        match data:
            case None:
                update_expr_array = None
            case _:
                update_expr_array: list[ttypes.UpdateExpr] = []
                for row in data:
                    parse_exprs = []
                    for column_name, value in row.items():

                        if isinstance(value, str):
                            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.String,
                                                                      str_value=value)
                        elif isinstance(value, int):
                            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64,
                                                                      i64_value=value)
                        elif isinstance(value, float):
                            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Double,
                                                                      f64_value=value)
                        else:
                            raise Exception("Invalid constant expression")

                        expr_type = ttypes.ParsedExprType(constant_expr=constant_expression)
                        paser_expr = ttypes.ParsedExpr(type=expr_type)
                        update_expr = ttypes.UpdateExpr(column_name=column_name, value=paser_expr)

                        update_expr_array.append(update_expr)

        return self._conn.update(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr,
                                 update_expr_array=update_expr_array)

    def knn(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
            topn: int):
        self.query_builder.knn(vector_column_name, embedding_data, embedding_data_type, distance_type, topn)

        return self

    def match(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, topn: int):
        self.query_builder.match(vector_column_name, embedding_data, embedding_data_type, topn)

        return self

    def output(self, columns: Optional[List[str]]):
        self.query_builder.output(columns)

        return self

    def filter(self, filter: Optional[str]):
        self.query_builder.filter(filter)

        return self

    def limit(self, limit: Optional[int]):
        self.query_builder.limit(limit)

        return self

    def offset(self, offset: Optional[int]):

        self.query_builder.offset(offset)

        return self

    def to_result(self):
        return self.query_builder.to_result()

    def to_df(self):
        return self.query_builder.to_df()

    def to_pl(self):
        return self.query_builder.to_pl()

    def to_arrow(self):
        return self.query_builder.to_arrow()


    def _execute_query(self, query: Query) -> tuple[dict[str, list[Any]], dict[str, Any]]:
        # process select_list
        select_list: List[ttypes.ParsedExpr] = []
        for column in query.columns:
            match column:
                case "*":
                    column_expr = ttypes.ColumnExpr(star=True, column_name=[])
                    expr_type = ttypes.ParsedExprType(column_expr=column_expr)
                    parsed_expr = ttypes.ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_row_id":
                    func_expr = ttypes.FunctionExpr(function_name="row_id", arguments=[])
                    expr_type = ttypes.ParsedExprType(function_expr=func_expr)
                    parsed_expr = ttypes.ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)

                case _:
                    select_list.append(traverse_conditions(condition(column)))

        # process where_expr
        match query.filter:
            case None:
                where_expr = None
            case _:
                where_expr = traverse_conditions(condition(query.filter))

        # process limit_expr
        match query.limit:
            case None:
                limit_expr = None
            case _:
                constant_exp = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64, i64_value=query.limit)
                expr_type = ttypes.ParsedExprType(constant_expr=constant_exp)
                limit_expr = ttypes.ParsedExpr(type=expr_type)

        # process offset_expr
        match query.offset:
            case None:
                offset_expr = None
            case _:
                constant_exp = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64, i64_value=query.offset)
                expr_type = ttypes.ParsedExprType(constant_expr=constant_exp)
                offset_expr = ttypes.ParsedExpr(type=expr_type)

        # execute the query
        res = self._conn.select(db_name=self._db_name,
                                table_name=self._table_name,
                                select_list=select_list,
                                search_expr=query.search,
                                where_expr=where_expr,
                                group_by_list=None,
                                limit_expr=limit_expr,
                                offset_expr=offset_expr)

        # process the results
        if res.success:
            return build_result(res)
        else:
            raise Exception(res.error_msg)
