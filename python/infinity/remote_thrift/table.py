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
import functools
import inspect
import os
import numpy as np
from abc import ABC
from typing import Optional, Union, List, Any

from sqlglot import condition

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.common import INSERT_DATA, VEC
from infinity.errors import ErrorCode
from infinity.index import IndexInfo
from infinity.remote_thrift.query_builder import Query, InfinityThriftQueryBuilder, ExplainQuery
from infinity.remote_thrift.types import build_result
from infinity.remote_thrift.utils import traverse_conditions, name_validity_check, select_res_to_polars
from infinity.table import Table, ExplainType
from infinity.common import ConflictType


class RemoteTable(Table, ABC):

    def __init__(self, conn, db_name, table_name):
        self._conn = conn
        self._db_name = db_name
        self._table_name = table_name
        self.query_builder = InfinityThriftQueryBuilder(table=self)

    def params_type_check(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            sig = inspect.signature(func)
            params = sig.parameters
            for arg, param in zip(args, params.values()):
                if param.annotation is not param.empty and not isinstance(arg, param.annotation):
                    raise TypeError(f"TypeError: Argument {param.name} must be {param.annotation}")
            for kwarg, value in kwargs.items():
                if params[kwarg].annotation is not params[kwarg].empty and not isinstance(value,
                                                                                          params[kwarg].annotation):
                    raise TypeError(f"TypeError: Argument {kwarg} must be {params[kwarg].annotation}")
            return func(*args, **kwargs)
        return wrapper

    @name_validity_check("index_name", "Index")
    def create_index(self, index_name: str, index_infos: list[IndexInfo],
                     conflict_type: ConflictType = ConflictType.Error):
        index_name = index_name.strip()

        index_info_list_to_use: list[ttypes.IndexInfo] = []

        for index_info in index_infos:
            index_info_to_use = ttypes.IndexInfo(column_name=index_info.column_name.strip(),
                                                 index_type=index_info.index_type.to_ttype(),
                                                 index_param_list=[init_param.to_ttype() for init_param in
                                                                   index_info.params])

            index_info_list_to_use.append(index_info_to_use)

        create_index_conflict: ttypes.CreateConflict
        if conflict_type == ConflictType.Error:
            create_index_conflict = ttypes.CreateConflict.Error
        elif conflict_type == ConflictType.Ignore:
            create_index_conflict = ttypes.CreateConflict.Ignore
        elif conflict_type == ConflictType.Replace:
            create_index_conflict = ttypes.CreateConflict.Replace
        else:
            raise Exception(f"ERROR:3066, Invalid conflict type")

        res = self._conn.create_index(db_name=self._db_name,
                                      table_name=self._table_name,
                                      index_name=index_name,
                                      index_info_list=index_info_list_to_use,
                                      conflict_type=create_index_conflict)

        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @name_validity_check("index_name", "Index")
    def drop_index(self, index_name: str, conflict_type: ConflictType = ConflictType.Error):
        drop_index_conflict: ttypes.DropConflict
        if conflict_type == ConflictType.Error:
            drop_index_conflict = ttypes.DropConflict.Error
        elif conflict_type == ConflictType.Ignore:
            drop_index_conflict = ttypes.DropConflict.Ignore
        else:
            raise Exception(f"ERROR:3066, invalid conflict type")

        res = self._conn.drop_index(db_name=self._db_name, table_name=self._table_name,
                                    index_name=index_name, conflict_type=drop_index_conflict)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    @name_validity_check("index_name", "Index")
    def show_index(self, index_name: str):
        res = self._conn.show_index(db_name=self._db_name, table_name=self._table_name, index_name=index_name)

        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def list_indexes(self):
        res = self._conn.list_indexes(db_name=self._db_name, table_name=self._table_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

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

                elif isinstance(value, float) or isinstance(value, np.float32):
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

                expr_type = ttypes.ParsedExprType(
                    constant_expr=constant_expression)
                paser_expr = ttypes.ParsedExpr(type=expr_type)

                parse_exprs.append(paser_expr)

            field = ttypes.Field(parse_exprs=parse_exprs)
            fields.append(field)

        res = self._conn.insert(db_name=db_name, table_name=table_name, column_names=column_names,
                                fields=fields)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def import_data(self, file_path: str, import_options: {} = None):
        options = ttypes.ImportOption()
        options.has_header = False
        options.delimiter = ','
        options.copy_file_type = ttypes.CopyFileType.CSV
        if import_options != None:
            for k, v in import_options.items():
                key = k.lower()
                if key == 'file_type':
                    file_type = v.lower()
                    if file_type == 'csv':
                        options.copy_file_type = ttypes.CopyFileType.CSV
                    elif file_type == 'json':
                        options.copy_file_type = ttypes.CopyFileType.JSON
                    elif file_type == 'jsonl':
                        options.copy_file_type = ttypes.CopyFileType.JSONL
                    elif file_type == 'fvecs':
                        options.copy_file_type = ttypes.CopyFileType.FVECS
                    else:
                        raise Exception("Unrecognized import file type")
                elif key == 'delimiter':
                    delimiter = v.lower()
                    if len(delimiter) != 1:
                        raise Exception("Unrecognized import file delimiter")
                    options.delimiter = delimiter[0]
                elif key == 'header':
                    if isinstance(v, bool):
                        options.has_header = v
                    else:
                        raise Exception("Boolean value is expected in header field")
                else:
                    raise Exception("Unknown import parameter")

        total_size = os.path.getsize(file_path)
        chunk_size = 1024 * 1024 * 10  # 10MB
        file_name = os.path.basename(file_path)

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
                        if res.error_code != ErrorCode.OK:
                            raise Exception(f"ERROR:{res.error_code} upload failed: {res.error_msg}")
                        if res.error_msg:
                            raise Exception(f"upload failed: {res.error_msg}")
                        if res.can_skip:
                            break

        res = self._conn.import_data(db_name=self._db_name,
                                     table_name=self._table_name,
                                     file_name=file_name,
                                     import_options=options)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def delete(self, cond: Optional[str] = None):
        match cond:
            case None:
                where_expr = None
            case _:
                where_expr = traverse_conditions(condition(cond))
        res = self._conn.delete(
            db_name=self._db_name, table_name=self._table_name, where_expr=where_expr)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def update(self, cond: Optional[str],
               data: Optional[list[dict[str, Union[str, int, float, list[Union[int, float]]]]]]):
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
                    for column_name, value in row.items():

                        if isinstance(value, str):
                            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.String,
                                                                      str_value=value)
                        elif isinstance(value, int):
                            constant_expression = ttypes.ConstantExpr(literal_type=ttypes.LiteralType.Int64,
                                                                      i64_value=value)
                        elif isinstance(value, float) or isinstance(value, np.float32):
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

                        expr_type = ttypes.ParsedExprType(
                            constant_expr=constant_expression)
                        paser_expr = ttypes.ParsedExpr(type=expr_type)
                        update_expr = ttypes.UpdateExpr(
                            column_name=column_name, value=paser_expr)

                        update_expr_array.append(update_expr)

        res = self._conn.update(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr,
                                update_expr_array=update_expr_array)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def knn(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
            topn: int, knn_params: {} = None):
        self.query_builder.knn(
            vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params)
        return self

    @params_type_check
    def match(self, fields: str, matching_text: str, options_text: str = ''):
        self.query_builder.match(fields, matching_text, options_text)
        return self

    @params_type_check
    def fusion(self, method: str, options_text: str = ''):
        self.query_builder.fusion(method, options_text)
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

    def explain(self, explain_type: ExplainType = ExplainType.Physical):
        return self.query_builder.explain(explain_type)

    def _execute_query(self, query: Query) -> tuple[dict[str, list[Any]], dict[str, Any]]:

        # execute the query
        res = self._conn.select(db_name=self._db_name,
                                table_name=self._table_name,
                                select_list=query.columns,
                                search_expr=query.search,
                                where_expr=query.filter,
                                group_by_list=None,
                                limit_expr=query.limit,
                                offset_expr=query.offset)

        # process the results
        if res.error_code == ErrorCode.OK:
            return build_result(res)
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")

    def _explain_query(self, query: ExplainQuery) -> Any:
        res = self._conn.explain(db_name=self._db_name,
                                 table_name=self._table_name,
                                 select_list=query.columns,
                                 search_expr=query.search,
                                 where_expr=query.filter,
                                 group_by_list=None,
                                 limit_expr=query.limit,
                                 offset_expr=query.offset,
                                 explain_type=query.explain_type.to_ttype())
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise Exception(f"ERROR:{res.error_code}, {res.error_msg}")
