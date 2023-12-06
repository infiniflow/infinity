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
from typing import Optional, Union, List

import pandas as pd
from sqlglot import condition

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
from infinity.common import VEC
from infinity.index import IndexInfo
from infinity.query import Query, InfinityVectorQueryBuilder
from infinity.remote_thrift.types import build_result
from infinity.remote_thrift.utils import traverse_conditions
from infinity.table import Table


class RemoteTable(Table, ABC):

    def __init__(self, conn, db_name, table_name):
        self._conn = conn
        self._db_name = db_name
        self._table_name = table_name

    def create_index(self, index_name: str, index_infos: list[IndexInfo], options=None):

        index_name = index_name.strip()

        index_info_list_to_use: list[ttypes.IndexInfo] = []

        for index_info in index_infos:
            index_info_to_use = ttypes.IndexInfo()
            index_info_to_use.column_name = index_info.column_name.strip()
            index_info_to_use.index_type = index_info.index_type.to_ttype()
            index_info_to_use.index_param_list = [init_param.to_ttype() for init_param in index_info.params]

            index_info_list_to_use.append(index_info_to_use)

        return self._conn.create_index(db_name=self._db_name,
                                       table_name=self._table_name,
                                       index_name=index_name,
                                       index_info_list=index_info_list_to_use,
                                       option=options)

    def drop_index(self, index_name: str):
        return self._conn.drop_index(db_name=self._db_name, table_name=self._table_name,
                                     index_name=index_name)

    def insert(self, data: list[dict[str, Union[str, int, float, list[Union[int, float]]]]]):
        # [{"c1": 1, "c2": 1.1}, {"c1": 2, "c2": 2.2}]
        db_name = self._db_name
        table_name = self._table_name
        column_names: list[str] = []
        fields: list[ttypes.Field] = []
        for row in data:
            column_names = list(row.keys())
            parse_exprs = []
            for column_name, value in row.items():
                constant_expression = ttypes.ConstantExpr()
                if isinstance(value, str):
                    constant_expression.literal_type = ttypes.LiteralType.String
                    constant_expression.str_value = value
                elif isinstance(value, int):
                    constant_expression.literal_type = ttypes.LiteralType.Int64
                    constant_expression.i64_value = value
                elif isinstance(value, float):
                    constant_expression.literal_type = ttypes.LiteralType.Double
                    constant_expression.f64_value = value
                elif isinstance(value, list):
                    if isinstance(value[0], int):
                        constant_expression.literal_type = ttypes.LiteralType.IntegerArray
                        constant_expression.i64_array_value = value
                    elif isinstance(value[0], float):
                        constant_expression.literal_type = ttypes.LiteralType.DoubleArray
                        constant_expression.f64_array_value = value
                else:
                    raise Exception("Invalid constant expression")
                paser_expr_type = ttypes.ParsedExprType()
                paser_expr_type.constant_expr = constant_expression

                paser_expr = ttypes.ParsedExpr()
                paser_expr.type = paser_expr_type
                parse_exprs.append(paser_expr)
            f = ttypes.Field()
            f.parse_exprs = parse_exprs
            fields.append(f)

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
                            constant_expression = ttypes.ConstantExpr()
                            constant_expression.literal_type = ttypes.LiteralType.String
                            constant_expression.str_value = value
                        elif isinstance(value, int):
                            constant_expression = ttypes.ConstantExpr()
                            constant_expression.literal_type = ttypes.LiteralType.Int64
                            constant_expression.i64_value = value
                        elif isinstance(value, float):
                            constant_expression = ttypes.ConstantExpr()
                            constant_expression.literal_type = ttypes.LiteralType.Double
                            constant_expression.f64_value = value
                        else:
                            raise Exception("Invalid constant expression")

                        paser_expr_type = ttypes.ParsedExprType()
                        paser_expr_type.constant_expr = constant_expression
                        paser_expr = ttypes.ParsedExpr()
                        paser_expr.type = paser_expr_type

                        update_expr = ttypes.UpdateExpr()
                        update_expr.column_name = column_name
                        update_expr.value = paser_expr
                        update_expr_array.append(update_expr)

        return self._conn.update(db_name=self._db_name, table_name=self._table_name, where_expr=where_expr,
                                 update_expr_array=update_expr_array)

        pass

    def search(
            self,
            embedding: Optional[Union[VEC, str]] = None,
            vector_column_name: str = "vector",
            distance: str = "L2",
            threshold: Optional[float] = None,
    ) -> InfinityVectorQueryBuilder:
        return InfinityVectorQueryBuilder.create(
            table=self,
            embedding=embedding,
            vector_column_name=vector_column_name,
            distance=distance,
            threshold=threshold
        )

    def _execute_query(self, query: Query) -> pd.DataFrame:
        # process select_list
        select_list: List[ttypes.ParsedExpr] = []
        for column in query.columns:
            match column:
                case "*":
                    column_expr = ttypes.ColumnExpr()
                    column_expr.star = True
                    paser_expr_type = ttypes.ParsedExprType()
                    paser_expr_type.column_expr = column_expr
                    parsed_expr = ttypes.ParsedExpr()
                    parsed_expr.type = paser_expr_type
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
                constant_exp = ttypes.ConstantExpr()
                constant_exp.literal_type = ttypes.LiteralType.Int64
                constant_exp.i64_value = query.limit
                paser_expr_type = ttypes.ParsedExprType()
                paser_expr_type.constant_expr = constant_exp
                limit_expr = ttypes.ParsedExpr()
                limit_expr.type = paser_expr_type

        # process offset_expr
        match query.offset:
            case None:
                offset_expr = None
            case _:
                constant_exp = ttypes.ConstantExpr()
                constant_exp.literal_type = ttypes.LiteralType.Int64
                paser_expr_type = ttypes.ParsedExprType()
                paser_expr_type.constant_expr = constant_exp
                offset_expr = ttypes.ParsedExpr()
                offset_expr.type = paser_expr_type
                offset_expr.i64_value = query.offset

        # process knn_exprs
        match query.embedding:
            case None:
                knn_exprs_list = None

            case _:
                knn_exprs_list = []
                embedding = query.embedding
                vector_column_name = query.vector_column_name
                distance = query.distance
                threshold = query.threshold

                knn_expr = ttypes.KnnExpr()

                knn_expr.column_expr = traverse_conditions(condition(vector_column_name))

                if isinstance(embedding[0], int):
                    data = ttypes.EmbeddingData()
                    data.i64_array_value = embedding
                    knn_expr.embedding_data = data
                    knn_expr.embedding_data_type = ttypes.ElementType.ElementInt64
                elif isinstance(embedding[0], float):
                    data = ttypes.EmbeddingData()
                    data.f32_array_value = embedding
                    knn_expr.embedding_data = data
                    knn_expr.embedding_data_type = ttypes.ElementType.ElementFloat32
                else:
                    raise Exception(f"Invalid embedding {embedding[0]} type")

                match query.distance:
                    case "L2":
                        knn_expr.distance = ttypes.KnnDistanceType.L2
                    case "IP":
                        knn_expr.distance = ttypes.KnnDistanceType.InnerProduct
                    case _:
                        raise Exception("Invalid distance type")

                knn_expr.dimension = len(embedding)
                knn_exprs_list.append(knn_expr)

        # execute the query
        res = self._conn.select(db_name=self._db_name,
                                table_name=self._table_name,
                                select_list=select_list,
                                where_expr=where_expr,
                                group_by_list=None,
                                limit_expr=limit_expr,
                                offset_expr=offset_expr,
                                search_expr=None,
                                knn_expr_list=knn_exprs_list)

        # process the results
        if res.success:
            return build_result(res)
        # todo: how to convert bytes to string?
