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
from abc import ABC
from typing import Optional, Union, List, Any

import numpy as np
from infinity.embedded_infinity_ext import ConflictType as LocalConflictType
from infinity.embedded_infinity_ext import WrapIndexInfo, WrapConstantExpr, LiteralType, ImportOptions, CopyFileType, WrapParsedExpr, \
    ParsedExprType, WrapUpdateExpr, ExportOptions, OptimizeOptions
from infinity.common import ConflictType, DEFAULT_MATCH_VECTOR_TOPN
from infinity.common import INSERT_DATA, VEC, SPARSE, InfinityException
from infinity.errors import ErrorCode
from infinity.index import IndexInfo
from infinity.local_infinity.query_builder import Query, InfinityLocalQueryBuilder, ExplainQuery
from infinity.local_infinity.types import build_result
from infinity.local_infinity.utils import traverse_conditions, select_res_to_polars
from infinity.remote_thrift.utils import name_validity_check
from infinity.table import Table, ExplainType
import infinity.index as index
from infinity.index import InitParameter
from sqlglot import condition


class LocalTable(Table, ABC):

    def __init__(self, conn, db_name, table_name):
        self._conn = conn
        self._db_name = db_name
        self._table_name = table_name
        self.query_builder = InfinityLocalQueryBuilder(table=self)

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

        create_index_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            create_index_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            create_index_conflict = LocalConflictType.kIgnore
        elif conflict_type == ConflictType.Replace:
            create_index_conflict = LocalConflictType.kReplace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, f"Invalid conflict type")

        index_info_list_to_use: list[WrapIndexInfo] = []

        for index_info in index_infos:
            index_info_to_use = WrapIndexInfo()
            index_info_to_use.index_type = index_info.index_type.to_local_type()
            index_info_to_use.column_name = index_info.column_name.strip()
            index_info_to_use.index_param_list = [init_param.to_local_type() for init_param in index_info.params]

            index_info_list_to_use.append(index_info_to_use)

        res = self._conn.create_index(db_name=self._db_name,
                                      table_name=self._table_name,
                                      index_name=index_name,
                                      index_info_list=index_info_list_to_use,
                                      conflict_type=create_index_conflict)

        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("index_name", "Index")
    def drop_index(self, index_name: str, conflict_type: ConflictType = ConflictType.Error):
        drop_index_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            drop_index_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            drop_index_conflict = LocalConflictType.kIgnore
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

    def show_segment(self, segment_id: int):
        res = self._conn.show_segment(db_name=self._db_name, table_name=self._table_name, segment_id=segment_id)
        if res.error_code == ErrorCode.OK:
            return res
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
        fields = []

        if isinstance(data, dict):
            data = [data]

        for row in data:
            column_names = list(row.keys())
            parse_exprs = []
            for column_name, value in row.items():
                constant_expression = WrapConstantExpr()
                if isinstance(value, str):
                    constant_expression.literal_type = LiteralType.kString
                    constant_expression.str_value = value
                elif isinstance(value, int):
                    constant_expression.literal_type = LiteralType.kInteger
                    constant_expression.i64_value = value
                elif isinstance(value, float) or isinstance(value, np.float32):
                    constant_expression.literal_type = LiteralType.kDouble
                    constant_expression.f64_value = value
                elif isinstance(value, list):
                    if isinstance(value[0], int):
                        constant_expression.literal_type = LiteralType.kIntegerArray
                        constant_expression.i64_array_value = value
                    elif isinstance(value[0], float):
                        constant_expression.literal_type = LiteralType.kDoubleArray
                        constant_expression.f64_array_value = value
                elif isinstance(value, dict):
                    if isinstance(value["values"][0], int):
                        constant_expression.literal_type = LiteralType.kLongSparseArray
                        if isinstance(value["indices"][0], int):
                            constant_expression.i64_array_idx = value["indices"]
                            constant_expression.i64_array_value = value["values"]
                        else:
                            raise InfinityException(3069, "Invalid constant expression")
                    elif isinstance(value["values"][0], float):
                        constant_expression.literal_type = LiteralType.kDoubleSparseArray
                        if isinstance(value["indices"][0], int):
                            constant_expression.i64_array_idx = value["indices"]
                            constant_expression.f64_array_value = value["values"]
                        else:
                            raise InfinityException(3069, "Invalid constant expression")
                    else:
                        raise InfinityException(3069, "Invalid constant expression")
                    
                else:
                    raise InfinityException(3069, "Invalid constant expression")
                parse_exprs.append(constant_expression)

            fields.append(parse_exprs)

        res = self._conn.insert(db_name=db_name, table_name=table_name, column_names=column_names,
                                fields=fields)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def import_data(self, file_path: str, import_options: {} = None):
        options = ImportOptions()
        options.header = False
        options.delimiter = ','
        options.copy_file_type = CopyFileType.kCSV
        if import_options != None:
            for k, v in import_options.items():
                key = k.lower()
                if key == 'file_type':
                    file_type = v.lower()
                    if file_type == 'csv':
                        options.copy_file_type = CopyFileType.kCSV
                    elif file_type == 'json':
                        options.copy_file_type = CopyFileType.kJSON
                    elif file_type == 'jsonl':
                        options.copy_file_type = CopyFileType.kJSONL
                    elif file_type == 'fvecs':
                        options.copy_file_type = CopyFileType.kFVECS
                    else:
                        raise InfinityException(3037, f"Unrecognized export file type: {file_type}")
                elif key == 'delimiter':
                    delimiter = v.lower()
                    if len(delimiter) != 1:
                        raise InfinityException(3037, f"Unrecognized export file delimiter: {delimiter}")
                    options.delimiter = delimiter[0]
                elif key == 'header':
                    if isinstance(v, bool):
                        options.header = v
                    else:
                        raise InfinityException(3037, "Boolean value is expected in header field")
                else:
                    raise InfinityException(3037, f"Unknown export parameter: {k}")

        res = self._conn.import_data(db_name=self._db_name,
                                     table_name=self._table_name,
                                     file_name=file_path,
                                     import_options=options)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def export_data(self, file_path: str, export_options: {} = None, columns: [str] = None):
        options = ExportOptions()
        options.header = False
        options.delimiter = ','
        options.copy_file_type = CopyFileType.kCSV
        if export_options != None:
            for k, v in export_options.items():
                key = k.lower()
                if key == 'file_type':
                    file_type = v.lower()
                    if file_type == 'csv':
                        options.copy_file_type = CopyFileType.kCSV
                    elif file_type == 'jsonl':
                        options.copy_file_type = CopyFileType.kJSONL
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file type: {file_type}")
                elif key == 'delimiter':
                    delimiter = v.lower()
                    if len(delimiter) != 1:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unrecognized export file delimiter: {delimiter}")
                    options.delimiter = delimiter[0]
                elif key == 'header':
                    if isinstance(v, bool):
                        options.header = v
                    else:
                        raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, "Boolean value is expected in header field")
                else:
                    raise InfinityException(ErrorCode.IMPORT_FILE_FORMAT_ERROR, f"Unknown export parameter: {k}")
        if not columns:
            columns = []
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
                update_expr_array: list[WrapParsedExpr] = []
                for row in data:
                    for column_name, value in row.items():
                        constant_expression = WrapConstantExpr()
                        if isinstance(value, str):
                            constant_expression.literal_type = LiteralType.kString
                            constant_expression.str_value = value
                        elif isinstance(value, int):
                            constant_expression.literal_type = LiteralType.kInteger
                            constant_expression.i64_value = value
                        elif isinstance(value, float) or isinstance(value, np.float32):
                            constant_expression.literal_type = LiteralType.kDouble
                            constant_expression.f64_value = value
                        elif isinstance(value, list):
                            if isinstance(value[0], int):
                                constant_expression.literal_type = LiteralType.kIntegerArray
                                constant_expression.i64_array_value = value
                            elif isinstance(value[0], float):
                                constant_expression.literal_type = LiteralType.kDoubleArray
                                constant_expression.f64_array_value = value
                        else:
                            raise InfinityException(3069, "Invalid constant expression")

                        parsed_expr = WrapParsedExpr(ParsedExprType.kConstant)
                        parsed_expr.constant_expr = constant_expression

                        update_expr = WrapUpdateExpr()
                        update_expr.column_name = column_name
                        update_expr.value = parsed_expr

                        update_expr_array.append(update_expr)

        res = self._conn.update(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr,
                                update_expr_array=update_expr_array)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def knn(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
            topn: int = DEFAULT_MATCH_VECTOR_TOPN, knn_params: {} = None):
        self.query_builder.knn(
            vector_column_name, embedding_data, embedding_data_type, distance_type, topn, knn_params)
        return self

    def match_sparse(self, vector_column_name: str, sparse_data: SPARSE, distance_type: str, topn: int, opt_params: {} = None):
        self.query_builder.match_sparse(vector_column_name, sparse_data, distance_type, topn, opt_params)
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

    def to_result(self):
        return self.query_builder.to_result()

    def filter(self, filter: Optional[str]):
        self.query_builder.filter(filter)
        return self

    def limit(self, limit: Optional[int]):
        self.query_builder.limit(limit)
        return self

    def offset(self, offset: Optional[int]):
        self.query_builder.offset(offset)
        return self

    def to_df(self):
        return self.query_builder.to_df()

    def to_pl(self):
        return self.query_builder.to_pl()

    def to_arrow(self):
        return self.query_builder.to_arrow()

    def explain(self, explain_type: ExplainType = ExplainType.Physical):
        return self.query_builder.explain(explain_type)
    
    def optimize(self, index_name: str, opt_params: dict[str, str]):
        opt_options = OptimizeOptions()
        opt_options.index_name = index_name
        opt_options.opt_params = [InitParameter(k, v).to_local_type() for k, v in opt_params.items()]
        return self._conn.optimize(db_name=self._db_name, table_name=self._table_name, optimize_opt=opt_options)

    def _execute_query(self, query: Query):
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
            raise InfinityException(res.error_code, res.error_msg)

    def _explain_query(self, query: ExplainQuery) -> Any:
        res = self._conn.explain(db_name=self._db_name,
                                 table_name=self._table_name,
                                 explain_type=query.explain_type.to_local_ttype(),
                                 select_list=query.columns,
                                 search_expr=query.search,
                                 where_expr=query.filter,
                                 group_by_list=None,
                                 limit_expr=query.limit,
                                 offset_expr=query.offset)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)
