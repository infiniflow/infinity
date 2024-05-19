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

from __future__ import annotations

from abc import ABC
from typing import List, Optional, Any

import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from pyarrow import Table
from sqlglot import condition, maybe_parse

from infinity.common import VEC
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from infinity.remote_thrift.types import logic_type_to_dtype
from infinity.remote_thrift.utils import traverse_conditions, parse_expr

'''FIXME: How to disable validation of only the search field?'''


class Query(ABC):
    def __init__(self, columns: Optional[List[ParsedExpr]], search: Optional[SearchExpr], filter: Optional[ParsedExpr],
                 limit: Optional[ParsedExpr], offset: Optional[ParsedExpr]):
        self.columns = columns
        self.search = search
        self.filter = filter
        self.limit = limit
        self.offset = offset


class ExplainQuery(Query):
    def __init__(self, columns: Optional[List[ParsedExpr]], search: Optional[SearchExpr], filter: Optional[ParsedExpr],
                 limit: Optional[ParsedExpr], offset: Optional[ParsedExpr], explain_type: Optional[ExplainType]):
        super().__init__(columns, search, filter, limit, offset)
        self.explain_type = explain_type


class InfinityThriftQueryBuilder(ABC):
    def __init__(self, table):
        self._table = table
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None

    def reset(self):
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None

    def knn(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
            topn: int, knn_params: {} = None) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        if self._search.knn_exprs is None:
            self._search.knn_exprs = list()

        column_expr = ColumnExpr(column_name=[vector_column_name], star=False)

        if not isinstance(topn, int):
            raise Exception(f"Invalid topn, type should be embedded, but get {type(topn)}")

        # type casting
        if isinstance(embedding_data, list):
            embedding_data = embedding_data
        elif isinstance(embedding_data, tuple):
            embedding_data = embedding_data
        elif isinstance(embedding_data, np.ndarray):
            embedding_data = embedding_data.tolist()
        else:
            raise Exception(f"Invalid embedding data, type should be embedded, but get {type(embedding_data)}")

        if (embedding_data_type == 'tinyint' or
            embedding_data_type == 'smallint' or
            embedding_data_type == 'int' or
            embedding_data_type == 'bigint'):
            embedding_data = [int(x) for x in embedding_data]

        data = EmbeddingData()
        elem_type = ElementType.ElementFloat32
        if embedding_data_type == 'bit':
            elem_type = ElementType.ElementBit
            raise Exception(f"Invalid embedding {embedding_data[0]} type")
        elif embedding_data_type == 'tinyint':
            elem_type = ElementType.ElementInt8
            data.i8_array_value = embedding_data
        elif embedding_data_type == 'smallint':
            elem_type = ElementType.ElementInt16
            data.i16_array_value = embedding_data
        elif embedding_data_type == 'int':
            elem_type = ElementType.ElementInt32
            data.i32_array_value = embedding_data
        elif embedding_data_type == 'bigint':
            elem_type = ElementType.ElementInt64
            data.i64_array_value = embedding_data
        elif embedding_data_type == 'float':
            elem_type = ElementType.ElementFloat32
            data.f32_array_value = embedding_data
        elif embedding_data_type == 'double':
            elem_type = ElementType.ElementFloat64
            data.f64_array_value = embedding_data
        else:
            raise Exception(f"Invalid embedding {embedding_data[0]} type")

        dist_type = KnnDistanceType.L2
        if distance_type == 'l2':
            dist_type = KnnDistanceType.L2
        elif distance_type == 'cosine':
            dist_type = KnnDistanceType.Cosine
        elif distance_type == 'ip':
            dist_type = KnnDistanceType.InnerProduct
        elif distance_type == 'hamming':
            dist_type = KnnDistanceType.Hamming
        else:
            raise Exception(f"Invalid distance type {distance_type}")

        knn_opt_params = []
        if knn_params != None:
            for k, v in knn_params.items():
                knn_opt_params.append(InitParameter(k, v))

        knn_expr = KnnExpr(column_expr=column_expr, embedding_data=data, embedding_data_type=elem_type,
                           distance_type=dist_type, topn=topn, opt_params=knn_opt_params)
        # print(knn_expr)
        self._search.knn_exprs.append(knn_expr)
        return self

    def match(self, fields: str, matching_text: str, options_text: str = '') -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        if self._search.match_exprs is None:
            self._search.match_exprs = list()
        match_expr = MatchExpr()
        match_expr.fields = fields
        match_expr.matching_text = matching_text
        match_expr.options_text = options_text
        self._search.match_exprs.append(match_expr)
        return self

    def fusion(self, method: str, options_text: str = '') -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        fusion_expr = FusionExpr()
        fusion_expr.method = method
        fusion_expr.options_text = options_text
        self._search.fusion_expr = fusion_expr
        return self

    def filter(self, where: Optional[str]) -> InfinityThriftQueryBuilder:
        where_expr = traverse_conditions(condition(where))
        self._filter = where_expr
        return self

    def limit(self, limit: Optional[int]) -> InfinityThriftQueryBuilder:
        constant_exp = ConstantExpr(
            literal_type=LiteralType.Int64, i64_value=limit)
        expr_type = ParsedExprType(constant_expr=constant_exp)
        limit_expr = ParsedExpr(type=expr_type)
        self._limit = limit_expr
        return self

    def offset(self, offset: Optional[int]) -> InfinityThriftQueryBuilder:
        constant_exp = ConstantExpr(
            literal_type=LiteralType.Int64, i64_value=offset)
        expr_type = ParsedExprType(constant_expr=constant_exp)
        offset_expr = ParsedExpr(type=expr_type)
        self._offset = offset_expr
        return self

    def output(self, columns: Optional[list]) -> InfinityThriftQueryBuilder:
        self._columns = columns
        select_list: List[ParsedExpr] = []
        for column in columns:
            if isinstance(column, str):
                column = column.lower()

            match column:
                case "*":
                    column_expr = ColumnExpr(star=True, column_name=[])
                    expr_type = ParsedExprType(column_expr=column_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_row_id":
                    func_expr = FunctionExpr(
                        function_name="row_id", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_score":
                    func_expr = FunctionExpr(
                        function_name="score", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case _:
                    select_list.append(parse_expr(maybe_parse(column)))

        self._columns = select_list
        return self

    def to_result(self) -> tuple[dict[str, list[Any]], dict[str, Any]]:
        query = Query(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset
        )
        self.reset()
        return self._table._execute_query(query)

    def to_df(self) -> pd.DataFrame:
        df_dict = {}
        data_dict, data_type_dict = self.to_result()
        for k, v in data_dict.items():
            data_series = pd.Series(
                v, dtype=logic_type_to_dtype(data_type_dict[k]))
            df_dict[k] = data_series
        return pd.DataFrame(df_dict)

    def to_pl(self) -> pl.DataFrame:
        return pl.from_pandas(self.to_df())

    def to_arrow(self) -> Table:
        return pa.Table.from_pandas(self.to_df())

    def explain(self, explain_type=ExplainType.Physical) -> Any:
        query = ExplainQuery(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset,
            explain_type=explain_type
        )
        return self._table._explain_query(query)
