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
from infinity.common import INSERT_DATA, VEC, InfinityException, SparseVector
from infinity.errors import ErrorCode
from infinity.index import IndexInfo
from infinity.remote_thrift.query_builder import Query, InfinityThriftQueryBuilder, ExplainQuery
from infinity.remote_thrift.types import build_result
from infinity.remote_thrift.utils import (
    traverse_conditions,
    name_validity_check,
    select_res_to_polars,
    check_valid_name,
    get_remote_constant_expr_from_python_value,
    get_ordinary_info,
)
from infinity.table import ExplainType
from infinity.common import ConflictType, DEFAULT_MATCH_VECTOR_TOPN, SortType
from infinity.utils import deprecated_api


class RemoteTable():

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
    def create_index(self, index_name: str, index_info: IndexInfo,
                     conflict_type: ConflictType = ConflictType.Error):
        index_name = index_name.strip()

        index_info_to_use = index_info.to_ttype()

        create_index_conflict: ttypes.CreateConflict
        if conflict_type == ConflictType.Error:
            create_index_conflict = ttypes.CreateConflict.Error
        elif conflict_type == ConflictType.Ignore:
            create_index_conflict = ttypes.CreateConflict.Ignore
        elif conflict_type == ConflictType.Replace:
            create_index_conflict = ttypes.CreateConflict.Replace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, f"Invalid conflict type")

        res = self._conn.create_index(db_name=self._db_name,
                                      table_name=self._table_name,
                                      index_name=index_name,
                                      index_info=index_info_to_use,
                                      conflict_type=create_index_conflict)

        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("index_name", "Index")
    def drop_index(self, index_name: str, conflict_type: ConflictType = ConflictType.Error):
        drop_index_conflict: ttypes.DropConflict
        if conflict_type == ConflictType.Error:
            drop_index_conflict = ttypes.DropConflict.Error
        elif conflict_type == ConflictType.Ignore:
            drop_index_conflict = ttypes.DropConflict.Ignore
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, f"Invalid conflict type")

        res = self._conn.drop_index(db_name=self._db_name, table_name=self._table_name,
                                    index_name=index_name, conflict_type=drop_index_conflict)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("index_name", "Index")
    def show_index(self, index_name: str):
        res = self._conn.show_index(db_name=self._db_name, table_name=self._table_name, index_name=index_name)

        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def list_indexes(self):
        res = self._conn.list_indexes(db_name=self._db_name, table_name=self._table_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_segments(self):
        res = self._conn.show_segments(db_name=self._db_name, table_name=self._table_name)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_segment(self, segment_id: int):
        res = self._conn.show_segment(db_name=self._db_name, table_name=self._table_name, segment_id=segment_id)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_blocks(self, segment_id: int):
        res = self._conn.show_blocks(db_name=self._db_name, table_name=self._table_name, segment_id=segment_id)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_block(self, segment_id: int, block_id: int):
        res = self._conn.show_block(db_name=self._db_name, table_name=self._table_name, segment_id=segment_id,
                                    block_id=block_id)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_block_column(self, segment_id: int, block_id: int, column_id: int):
        res = self._conn.show_block_column(db_name=self._db_name, table_name=self._table_name, segment_id=segment_id,
                                           block_id=block_id, column_id=column_id)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

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
                constant_expression = get_remote_constant_expr_from_python_value(value)
                expr_type = ttypes.ParsedExprType(constant_expr=constant_expression)
                paser_expr = ttypes.ParsedExpr(type=expr_type)
                parse_exprs.append(paser_expr)

            field = ttypes.Field(parse_exprs=parse_exprs)
            fields.append(field)

        res = self._conn.insert(db_name=db_name, table_name=table_name, column_names=column_names,
                                fields=fields)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

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
                    elif file_type == 'csr':
                        options.copy_file_type = ttypes.CopyFileType.CSR
                    elif file_type == 'bvecs':
                        options.copy_file_type = ttypes.CopyFileType.BVECS
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file type: {file_type}")
                elif key == 'delimiter':
                    delimiter = v.lower()
                    if len(delimiter) != 1:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file delimiter: {delimiter}")
                    options.delimiter = delimiter[0]
                elif key == 'header':
                    if isinstance(v, bool):
                        options.has_header = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Boolean value is expected in header field")
                else:
                    raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unknown export parameter: {k}")

        res = self._conn.import_data(db_name=self._db_name,
                                     table_name=self._table_name,
                                     file_name=file_path,
                                     import_options=options)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def export_data(self, file_path: str, export_options: {} = None, columns: [str] = None):
        options = ttypes.ExportOption()
        options.has_header = False
        options.delimiter = ','
        options.copy_file_type = ttypes.CopyFileType.CSV
        options.offset = 0
        options.limit = 0
        options.row_limit = 0

        if export_options != None:
            for k, v in export_options.items():
                key = k.lower()
                if key == 'file_type':
                    file_type = v.lower()
                    if file_type == 'csv':
                        options.copy_file_type = ttypes.CopyFileType.CSV
                    elif file_type == 'jsonl':
                        options.copy_file_type = ttypes.CopyFileType.JSONL
                    elif file_type == 'fvecs':
                        options.copy_file_type = ttypes.CopyFileType.FVECS
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file type: {file_type}")
                elif key == 'delimiter':
                    delimiter = v.lower()
                    if len(delimiter) != 1:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file delimiter: {delimiter}")
                    options.delimiter = delimiter[0]
                elif key == 'header':
                    if isinstance(v, bool):
                        options.has_header = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Boolean value is expected in header field")
                elif key == 'offset':
                    if isinstance(v, int):
                        options.offset = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Integer value is expected in 'offset' field")
                elif key == 'limit':
                    if isinstance(v, int):
                        options.limit = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Integer value is expected in 'limit' field")
                elif key == 'row_limit':
                    if isinstance(v, int):
                        options.row_limit = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Integer value is expected in 'row_limit' field")
                else:
                    raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unknown export parameter: {k}")

        res = self._conn.export_data(db_name=self._db_name,
                                     table_name=self._table_name,
                                     file_name=file_path,
                                     export_options=options,
                                     columns=columns)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

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
            raise InfinityException(res.error_code, res.error_msg)

    def update(self, cond: str, data: dict[str, Any]):
        # {"c1": 1, "c2": 1.1}
        where_expr = traverse_conditions(condition(cond))
        update_expr_array: list[ttypes.UpdateExpr] = []
        for column_name, value in data.items():
            constant_expression = get_remote_constant_expr_from_python_value(value)
            expr_type = ttypes.ParsedExprType(constant_expr=constant_expression)
            paser_expr = ttypes.ParsedExpr(type=expr_type)
            update_expr = ttypes.UpdateExpr(column_name=column_name, value=paser_expr)
            update_expr_array.append(update_expr)
        res = self._conn.update(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr,
                                update_expr_array=update_expr_array)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def match_dense(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
                    topn: int = DEFAULT_MATCH_VECTOR_TOPN, knn_params: {} = None):
        self.query_builder.match_dense(
            vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params)
        return self

    def knn(self, *args, **kwargs):
        deprecated_api("knn is deprecated, please use match_dense instead")
        return self.match_dense(*args, **kwargs)

    @params_type_check
    def match_text(self, fields: str, matching_text: str, topn: int, extra_options: Optional[dict] = None):
        self.query_builder.match_text(fields, matching_text, topn, extra_options)
        return self

    def match(self, *args, **kwargs):
        deprecated_api("match is deprecated, please use match_text instead")
        return self.match_text(*args, **kwargs)

    @params_type_check
    def match_tensor(self, column_name: str, query_data: VEC, query_data_type: str, topn: int,
                     extra_option: Optional[dict] = None):
        self.query_builder.match_tensor(column_name, query_data, query_data_type, topn, extra_option)
        return self

    def match_sparse(self, vector_column_name: str, sparse_data: SparseVector | dict, distance_type: str, topn: int,
                     opt_params: Optional[dict] = None):
        self.query_builder.match_sparse(vector_column_name, sparse_data, distance_type, topn, opt_params)
        return self

    @params_type_check
    def fusion(self, method: str, topn: int, fusion_params: Optional[dict] = None):
        self.query_builder.fusion(method, topn, fusion_params)
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
    
    def sort(self, order_by_expr_list: Optional[List[list[str, SortType]]]):
        for order_by_expr in order_by_expr_list:
            if len(order_by_expr) != 2:
                raise InfinityException(ErrorCode.INVALID_PARAMETER, f"order_by_expr_list must be a list of [column_name, sort_type]")
            if order_by_expr[1] not in [SortType.Asc, SortType.Desc]:
                raise InfinityException(ErrorCode.INVALID_PARAMETER, f"sort_type must be SortType.Asc or SortType.Desc")
            if order_by_expr[1] == SortType.Asc:
                order_by_expr[1] = True
            else :
                order_by_expr[1] = False
        self.query_builder.sort(order_by_expr_list)
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

    def optimize(self, index_name: str, opt_params: dict[str, str]):
        opt_options = ttypes.OptimizeOptions()
        opt_options.index_name = index_name
        opt_options.opt_params = [ttypes.InitParameter(k, v) for k, v in opt_params.items()]
        return self._conn.optimize(db_name=self._db_name, table_name=self._table_name, optimize_opt=opt_options)
    
    def add_columns(self, column_defs: dict):
        column_defs_list = []
        for index, (column_name, column_info) in enumerate(column_defs.items()):
            check_valid_name(column_name, "Column")
            get_ordinary_info(column_info, column_defs_list, column_name, index)
        return self._conn.add_columns(db_name=self._db_name, table_name=self._table_name, column_defs=column_defs_list)

    def drop_columns(self, column_names: list[str] | str):
        if isinstance(column_names, str):
            column_names = [column_names]

        return self._conn.drop_columns(db_name=self._db_name, table_name=self._table_name, column_names=column_names)

    def _execute_query(self, query: Query) -> tuple[dict[str, list[Any]], dict[str, Any]]:

        # execute the query
        res = self._conn.select(db_name=self._db_name,
                                table_name=self._table_name,
                                select_list=query.columns,
                                search_expr=query.search,
                                where_expr=query.filter,
                                group_by_list=None,
                                limit_expr=query.limit,
                                offset_expr=query.offset,
                                order_by_list=query.sort)

        # process the results
        if res.error_code == ErrorCode.OK:
            return build_result(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)

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
            raise InfinityException(res.error_code, res.error_msg)
