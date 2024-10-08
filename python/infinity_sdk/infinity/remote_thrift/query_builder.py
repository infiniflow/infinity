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

from infinity.common import VEC, SparseVector, InfinityException, SortType
from infinity.errors import ErrorCode
from infinity.remote_thrift.infinity_thrift_rpc.ttypes import *
from infinity.remote_thrift.types import (
    logic_type_to_dtype,
    make_match_tensor_expr,
    make_match_sparse_expr,
)
from infinity.remote_thrift.utils import traverse_conditions, parse_expr, get_search_optional_filter_from_opt_params

"""FIXME: How to disable validation of only the search field?"""


class Query(ABC):
    def __init__(
        self,
        columns: Optional[List[ParsedExpr]],
        search: Optional[SearchExpr],
        filter: Optional[ParsedExpr],
        limit: Optional[ParsedExpr],
        offset: Optional[ParsedExpr],
        sort:  Optional[List[OrderByExpr]],
    ):
        self.columns = columns
        self.search = search
        self.filter = filter
        self.limit = limit
        self.offset = offset
        self.sort = sort


class ExplainQuery(Query):
    def __init__(
        self,
        columns: Optional[List[ParsedExpr]],
        search: Optional[SearchExpr],
        filter: Optional[ParsedExpr],
        limit: Optional[ParsedExpr],
        offset: Optional[ParsedExpr],
        #sort:  Optional[List[OrderByExpr]],
        explain_type: Optional[ExplainType],
    ):
        super().__init__(columns, search, filter, limit, offset, None)
        self.explain_type = explain_type


class InfinityThriftQueryBuilder(ABC):
    def __init__(self, table):
        self._table = table
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None
        self._sort = None

    def reset(self):
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None
        self._sort = None

    def match_dense(
        self,
        vector_column_name: str,
        embedding_data: VEC,
        embedding_data_type: str,
        distance_type: str,
        topn: int,
        knn_params: {} = None,
    ) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
            self._search.match_exprs = list()

        column_expr = ColumnExpr(column_name=[vector_column_name], star=False)

        if not isinstance(topn, int):
            raise InfinityException(
                ErrorCode.INVALID_TOPK_TYPE, f"Invalid topn, type should be embedded, but get {type(topn)}"
            )

        # type casting
        if isinstance(embedding_data, list):
            embedding_data = embedding_data
        elif isinstance(embedding_data, tuple):
            embedding_data = embedding_data
        elif isinstance(embedding_data, np.ndarray):
            embedding_data = embedding_data.tolist()
        else:
            raise InfinityException(
                ErrorCode.INVALID_DATA_TYPE,
                f"Invalid embedding data, type should be embedded, but get {type(embedding_data)}",
            )

        if embedding_data_type in ["uint8", "int8", "int16", "int32", "int", "int64"]:
            embedding_data = [int(x) for x in embedding_data]

        if embedding_data_type in ["float", "float32", "double", "float64", "float16", "bfloat16"]:
            embedding_data = [float(x) for x in embedding_data]

        data = EmbeddingData()
        elem_type = ElementType.ElementFloat32
        if embedding_data_type == "bit":
            elem_type = ElementType.ElementBit
            raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")
        elif embedding_data_type == "uint8":
            elem_type = ElementType.ElementUInt8
            data.u8_array_value = embedding_data
        elif embedding_data_type == "int8":
            elem_type = ElementType.ElementInt8
            data.i8_array_value = embedding_data
        elif embedding_data_type == "int16":
            elem_type = ElementType.ElementInt16
            data.i16_array_value = embedding_data
        elif embedding_data_type in ["int", "int32"]:
            elem_type = ElementType.ElementInt32
            data.i32_array_value = embedding_data
        elif embedding_data_type == "int64":
            elem_type = ElementType.ElementInt64
            data.i64_array_value = embedding_data
        elif embedding_data_type in ["float", "float32"]:
            elem_type = ElementType.ElementFloat32
            data.f32_array_value = embedding_data
        elif embedding_data_type in ["double", "float64"]:
            elem_type = ElementType.ElementFloat64
            data.f64_array_value = embedding_data
        elif embedding_data_type == "float16":
            elem_type = ElementType.ElementFloat16
            data.f16_array_value = embedding_data
        elif embedding_data_type == "bfloat16":
            elem_type = ElementType.ElementBFloat16
            data.bf16_array_value = embedding_data
        else:
            raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")

        dist_type = KnnDistanceType.L2
        if distance_type == "l2":
            dist_type = KnnDistanceType.L2
        elif distance_type == "cosine" or distance_type == "cos":
            dist_type = KnnDistanceType.Cosine
        elif distance_type == "ip":
            dist_type = KnnDistanceType.InnerProduct
        elif distance_type == "hamming":
            dist_type = KnnDistanceType.Hamming
        else:
            raise InfinityException(ErrorCode.INVALID_KNN_DISTANCE_TYPE, f"Invalid distance type {distance_type}")

        knn_opt_params = []
        optional_filter = None
        if knn_params is not None:
            optional_filter = get_search_optional_filter_from_opt_params(knn_params)
            for k, v in knn_params.items():
                key = k.lower()
                value = v.lower()
                knn_opt_params.append(InitParameter(key, value))

        knn_expr = KnnExpr(
            column_expr=column_expr,
            embedding_data=data,
            embedding_data_type=elem_type,
            distance_type=dist_type,
            topn=topn,
            opt_params=knn_opt_params,
            filter_expr=optional_filter,
        )
        generic_match_expr = GenericMatchExpr(match_vector_expr=knn_expr)
        self._search.match_exprs.append(generic_match_expr)
        return self

    def match_sparse(
        self,
        vector_column_name: str,
        sparse_data: SparseVector | dict,
        metric_type: str,
        topn: int,
        opt_params: Optional[dict] = None,
    ) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
            self._search.match_exprs = list()

        optional_filter = None if opt_params is None else get_search_optional_filter_from_opt_params(opt_params)
        match_sparse_expr = make_match_sparse_expr(
            vector_column_name, sparse_data, metric_type, topn, opt_params, optional_filter
        )
        generic_match_expr = GenericMatchExpr(match_sparse_expr=match_sparse_expr)
        self._search.match_exprs.append(generic_match_expr)
        return self

    def match_text(
        self, fields: str, matching_text: str, topn: int, extra_options: Optional[dict]
    ) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
            self._search.match_exprs = list()
        match_expr = MatchExpr()
        match_expr.fields = fields
        match_expr.matching_text = matching_text
        options_text = f"topn={topn}"
        if extra_options is not None:
            match_expr.filter_expr = get_search_optional_filter_from_opt_params(extra_options)
            for k, v in extra_options.items():
                options_text += f";{k}={v}"
        match_expr.options_text = options_text
        generic_match_expr = GenericMatchExpr(match_text_expr=match_expr)
        self._search.match_exprs.append(generic_match_expr)
        return self

    def match_tensor(
        self,
        column_name: str,
        query_data: VEC,
        query_data_type: str,
        topn: int,
        extra_option: Optional[dict] = None,
    ) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
            self._search.match_exprs = list()
        option_str = f"topn={topn}"
        optional_filter = None
        if extra_option is not None:
            optional_filter = get_search_optional_filter_from_opt_params(extra_option)
            for k, v in extra_option.items():
                option_str += f";{k}={v}"
        match_tensor_expr = make_match_tensor_expr(
            vector_column_name=column_name,
            embedding_data=query_data,
            embedding_data_type=query_data_type,
            method_type="maxsim",
            extra_option=option_str,
            filter_expr=optional_filter,
        )
        generic_match_expr = GenericMatchExpr(match_tensor_expr=match_tensor_expr)
        self._search.match_exprs.append(generic_match_expr)
        return self

    def fusion(self, method: str, topn: int, fusion_params: Optional[dict]) -> InfinityThriftQueryBuilder:
        if self._search is None:
            self._search = SearchExpr()
        if self._search.fusion_exprs is None:
            self._search.fusion_exprs = list()
        fusion_expr = FusionExpr()
        fusion_expr.method = method
        final_option_text = f"topn={topn}"
        if method in ["rrf", "weighted_sum"]:
            if isinstance(fusion_params, dict):
                for k, v in fusion_params.items():
                    if k == "topn":
                        raise InfinityException(ErrorCode.INVALID_EXPRESSION, "topn is not allowed in fusion params")
                    final_option_text += f";{k}={v}"
        elif method in ["match_tensor"]:
            fusion_expr.optional_match_tensor_expr = make_match_tensor_expr(
                vector_column_name=fusion_params["field"], embedding_data=fusion_params["query_tensor"],
                embedding_data_type=fusion_params["element_type"], method_type="maxsim", extra_option=None)
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Invalid fusion method")
        fusion_expr.options_text = final_option_text
        self._search.fusion_exprs.append(fusion_expr)
        return self

    def filter(self, where: Optional[str]) -> InfinityThriftQueryBuilder:
        where_expr = traverse_conditions(condition(where))
        self._filter = where_expr
        return self

    def limit(self, limit: Optional[int]) -> InfinityThriftQueryBuilder:
        constant_exp = ConstantExpr(literal_type=LiteralType.Int64, i64_value=limit)
        expr_type = ParsedExprType(constant_expr=constant_exp)
        limit_expr = ParsedExpr(type=expr_type)
        self._limit = limit_expr
        return self

    def offset(self, offset: Optional[int]) -> InfinityThriftQueryBuilder:
        constant_exp = ConstantExpr(literal_type=LiteralType.Int64, i64_value=offset)
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
                    func_expr = FunctionExpr(function_name="row_id", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_score":
                    func_expr = FunctionExpr(function_name="score", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_similarity":
                    func_expr = FunctionExpr(function_name="similarity", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case "_distance":
                    func_expr = FunctionExpr(function_name="distance", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    select_list.append(parsed_expr)
                case _:
                    select_list.append(parse_expr(maybe_parse(column)))

        self._columns = select_list
        return self
    
    def sort(self, order_by_expr_list: Optional[List[list[str, bool]]]) -> InfinityThriftQueryBuilder:
        sort_list: List[OrderByExpr] = []
        for order_by_expr in order_by_expr_list:
            if isinstance(order_by_expr[0], str):
                order_by_expr[0] = order_by_expr[0].lower()

            match order_by_expr[0]:
                case "*":
                    column_expr = ColumnExpr(star=True, column_name=[])
                    expr_type = ParsedExprType(column_expr=column_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    order_by_expr = OrderByExpr(expr = parsed_expr, asc = order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_row_id":
                    func_expr = FunctionExpr(function_name="row_id", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    order_by_expr = OrderByExpr(expr = parsed_expr, asc = order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_score":
                    func_expr = FunctionExpr(function_name="score", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    order_by_expr = OrderByExpr(expr = parsed_expr, asc = order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_similarity":
                    func_expr = FunctionExpr(function_name="similarity", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    order_by_expr = OrderByExpr(expr = parsed_expr, asc = order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_distance":
                    func_expr = FunctionExpr(function_name="distance", arguments=[])
                    expr_type = ParsedExprType(function_expr=func_expr)
                    parsed_expr = ParsedExpr(type=expr_type)
                    order_by_expr = OrderByExpr(expr = parsed_expr, asc = order_by_expr[1])
                    sort_list.append(order_by_expr)
                case _:
                    parsed_expr = parse_expr(maybe_parse(order_by_expr[0]))
                    sort_list.append(OrderByExpr(expr = parsed_expr, asc = order_by_expr[1]))

        self._sort = sort_list
        return self

    def to_result(self) -> tuple[dict[str, list[Any]], dict[str, Any]]:
        query = Query(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset,
            sort=self._sort,
        )
        self.reset()
        return self._table._execute_query(query)

    def to_df(self) -> pd.DataFrame:
        df_dict = {}
        data_dict, data_type_dict = self.to_result()
        for k, v in data_dict.items():
            data_series = pd.Series(v, dtype=logic_type_to_dtype(data_type_dict[k]))
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
            explain_type=explain_type,
        )
        return self._table._explain_query(query)
