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

import copy
from abc import ABC
from typing import List, Optional

import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from pyarrow import Table

from infinity.common import VEC
from infinity.remote_thrift.infinity_thrift_rpc import *
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *

'''FIXME: How to disable validation of only the search field?'''


class Query(ABC):
    def __init__(self, columns: Optional[List[str]], search: Optional[SearchExpr], filter: Optional[str],
                 limit: Optional[int], offset: Optional[int]):
        self.columns = columns
        self.search = search
        self.filter = filter
        self.limit = limit
        self.offset = offset


class InfinityThriftQueryBuilder(ABC):
    def __init__(self, table):
        self._table = table
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None

    def knn(self, vector_column_name: str, embedding_data: VEC, embedding_data_type: str, distance_type: str,
            topn: int) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        if self._search.knn_exprs is None:
            self._search.knn_exprs = list()

        column_expr = ttypes.ColumnExpr()
        column_expr.column_name.append(vector_column_name)
        column_expr.star = False

        if isinstance(embedding_data, list):
            embedding_data = embedding_data
        if isinstance(embedding_data, np.ndarray):
            embedding_data = embedding_data.tolist()
        data = ttypes.EmbeddingData()
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
        knn_expr = KnnExpr(column_expr=copy.deepcopy(column_expr), embedding_data=data, embedding_data_type=elem_type,
                           distance_type=dist_type, topn=topn)
        # print(knn_expr)
        self._search.knn_exprs.append(knn_expr)
        return self

    def match(self, fields: str, matching_text: str, options_text: str = '') -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        if self._search.match_exprs is None:
            self._search.match_exprs = list()
        match_expr = ttypes.MatchExpr()
        match_expr.fields = fields
        match_expr.matching_text = matching_text
        match_expr.options_text = options_text
        self._search.match_exprs.append(match_expr)
        return self

    def fusion(self, method: str, options_text: str = '') -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        fusion_expr = ttypes.FusionExpr()
        fusion_expr.method = method
        fusion_expr.options_text = options_text
        self._search.fusion_expr = fusion_expr
        return self

    def filter(self, where: Optional[str]) -> InfinityThriftQueryBuilder:
        self._filter = where
        return self

    def limit(self, limit: Optional[int]) -> InfinityThriftQueryBuilder:
        self._limit = limit
        return self

    def offset(self, offset: Optional[int]) -> InfinityThriftQueryBuilder:
        self._offset = offset
        return self

    def output(self, columns: Optional[list]) -> InfinityThriftQueryBuilder:
        self._columns = columns
        return self

    def to_df(self) -> pd.DataFrame:
        query = Query(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset
        )
        return self._table._execute_query(query)

    def to_pl(self) -> pl.DataFrame:
        query = Query(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset
        )
        return pl.from_pandas(self._table._execute_query(query))

    def to_arrow(self) -> Table:
        query = Query(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset
        )

        return pa.Table.from_pandas(self._table._execute_query(query))
