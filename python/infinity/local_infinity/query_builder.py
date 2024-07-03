from __future__ import annotations

from abc import ABC
from typing import List, Optional, Any

import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from pyarrow import Table
from sqlglot import condition, maybe_parse

from infinity.common import VEC, SPARSE, InfinityException, DEFAULT_MATCH_VECTOR_TOPN
from infinity.embedded_infinity_ext import *
from infinity.local_infinity.types import logic_type_to_dtype
from infinity.local_infinity.utils import traverse_conditions, parse_expr
from infinity.table import ExplainType as BaseExplainType

class Query(ABC):
    def __init__(self, columns: Optional[List[WrapParsedExpr]], search: Optional[WrapSearchExpr], filter: Optional[WrapParsedExpr],
                 limit: Optional[WrapParsedExpr], offset: Optional[WrapParsedExpr]):
        self.columns = columns
        self.search = search
        self.filter = filter
        self.limit = limit
        self.offset = offset


class ExplainQuery(Query):
    def __init__(self, columns: Optional[List[WrapParsedExpr]], search: Optional[WrapSearchExpr], filter: Optional[WrapParsedExpr],
                 limit: Optional[WrapParsedExpr], offset: Optional[WrapParsedExpr], explain_type: Optional[BaseExplainType]):
        super().__init__(columns, search, filter, limit, offset)
        self.explain_type = explain_type


class InfinityLocalQueryBuilder(ABC):
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
            topn: int = DEFAULT_MATCH_VECTOR_TOPN, knn_params: {} = None) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
        if self._search.knn_exprs is None or len(self._search.knn_exprs) == 0:
            self._search.knn_exprs = []

        column_expr = WrapColumnExpr()
        column_expr.names = [vector_column_name]
        column_expr.star = False

        if not isinstance(topn, int):
            raise InfinityException(3073, f"Invalid topn, type should be embedded, but get {type(topn)}")

        # type casting
        if isinstance(embedding_data, list):
            embedding_data = embedding_data
        elif isinstance(embedding_data, tuple):
            embedding_data = embedding_data
        elif isinstance(embedding_data, np.ndarray):
            embedding_data = embedding_data.tolist()
        else:
            raise InfinityException(3051, f"Invalid embedding data, type should be embedded, but get {type(embedding_data)}")

        if (embedding_data_type == 'tinyint' or
                embedding_data_type == 'smallint' or
                embedding_data_type == 'int' or
                embedding_data_type == 'bigint'):
            embedding_data = [int(x) for x in embedding_data]

        data = EmbeddingData()
        elem_type = EmbeddingDataType.kElemFloat
        if embedding_data_type == 'bit':
            elem_type = EmbeddingDataType.kElemBit
            raise Exception(f"Invalid embedding {embedding_data[0]} type")
        elif embedding_data_type == 'tinyint':
            elem_type = EmbeddingDataType.kElemInt8
            data.i8_array_value = embedding_data
        elif embedding_data_type == 'smallint':
            elem_type = EmbeddingDataType.kElemInt16
            data.i16_array_value = embedding_data
        elif embedding_data_type == 'int':
            elem_type = EmbeddingDataType.kElemInt32
            data.i32_array_value = embedding_data
        elif embedding_data_type == 'bigint':
            elem_type = EmbeddingDataType.kElemInt64
            data.i64_array_value = embedding_data
        elif embedding_data_type == 'float':
            elem_type = EmbeddingDataType.kElemFloat
            data.f32_array_value = embedding_data
        elif embedding_data_type == 'double':
            elem_type = EmbeddingDataType.kElemDouble
            data.f64_array_value = embedding_data
        else:
            raise InfinityException(3057, f"Invalid embedding {embedding_data[0]} type")

        dist_type = KnnDistanceType.kInvalid
        if distance_type == 'l2':
            dist_type = KnnDistanceType.kL2
        elif distance_type == 'cosine':
            dist_type = KnnDistanceType.kCosine
        elif distance_type == 'ip':
            dist_type = KnnDistanceType.kInnerProduct
        elif distance_type == 'hamming':
            dist_type = KnnDistanceType.kHamming
        else:
            raise InfinityException(3056, f"Invalid distance type {distance_type}")

        knn_opt_params = []
        if knn_params != None:
            for k, v in knn_params.items():
                knn_opt_params.append(InitParameter(k, v))

        knn_expr = WrapKnnExpr()
        knn_expr.column_expr = column_expr
        knn_expr.embedding_data = data
        knn_expr.embedding_data_type = elem_type
        knn_expr.distance_type = dist_type
        knn_expr.topn = topn
        knn_expr.opt_params = knn_opt_params

        # not work
        # self._search.knn_exprs.append(knn_expr)

        knn_exprs = [knn_expr]
        self._search.knn_exprs = self._search.knn_exprs + knn_exprs

        assert(len(self._search.knn_exprs) > 0)
        return self

    def match_sparse(self, vector_column_name: str, sparse_data: SPARSE, metric_type: str, topn: int, opt_params: {} = None) \
        -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
        if self._search.match_sparse_exprs is None:
            self._search.match_sparse_exprs = list()

        column_expr = WrapColumnExpr()
        column_expr.names = [vector_column_name]
        column_expr.star = False

        if not isinstance(topn, int):
            raise InfinityException(3073, f"Invalid topn, type should be embedded, but get {type(topn)}")

        sparse_opt_params = []
        if opt_params != None:
            for k, v in opt_params.items():
                params = InitParameter()
                params.param_name = k
                params.param_value = v
                sparse_opt_params.append(params)
        match_sparse_expr = WrapMatchSparseExpr()
        match_sparse_expr.column_expr = column_expr

        sparse_expr = WrapConstantExpr()
        if isinstance(sparse_data["values"][0], int):
            sparse_expr.literal_type = LiteralType.kLongSparseArray
            sparse_expr.i64_array_idx = sparse_data["indices"]
            sparse_expr.i64_array_value = sparse_data["values"]
        elif isinstance(sparse_data["values"][0], float):
            sparse_expr.literal_type = LiteralType.kDoubleSparseArray
            sparse_expr.i64_array_idx = sparse_data["indices"]
            sparse_expr.f64_array_value = sparse_data["values"]
        else:
            raise InfinityException(3058, f"Invalid sparse data {sparse_data['values'][0]} type")
        match_sparse_expr.sparse_expr = sparse_expr

        match_sparse_expr.metric_type = metric_type
        match_sparse_expr.topn = topn
        match_sparse_expr.opt_params = sparse_opt_params
        
        match_sparse_exprs = [match_sparse_expr]
        self._search.match_sparse_exprs = self._search.match_sparse_exprs + match_sparse_exprs
        assert(len(self._search.match_sparse_exprs) > 0)
        return self

    def match(self, fields: str, matching_text: str, options_text: str = '') -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
        if self._search.match_exprs is None:
            self._search.match_exprs = list()
        match_expr = WrapMatchExpr()
        match_expr.fields = fields
        match_expr.matching_text = matching_text
        match_expr.options_text = options_text

        match_exprs = [match_expr]
        self._search.match_exprs = self._search.match_exprs + match_exprs
        assert(len(self._search.match_exprs) > 0)

        # self._search.match_exprs.append(match_expr)
        return self

    def fusion(self, method: str, options_text: str = '') -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
        fusion_expr = WrapFusionExpr()
        fusion_expr.method = method
        fusion_expr.options_text = options_text
        # self._search.fusion_exprs.append(fusion_expr)
        fusion_exprs = [fusion_expr]
        self._search.fusion_exprs = self._search.fusion_exprs + fusion_exprs
        assert(len(self._search.fusion_exprs) > 0)
        return self

    def filter(self, where: Optional[str]) -> InfinityLocalQueryBuilder:
        where_expr = traverse_conditions(condition(where))
        self._filter = where_expr
        return self

    def limit(self, limit: Optional[int]) -> InfinityLocalQueryBuilder:
        constant_exp = WrapConstantExpr(literal_type=LiteralType.kInteger, i64_value=limit)
        limit_expr = WrapParsedExpr(ParsedExprType.kConstant)
        limit_expr.constant_expr = constant_exp
        self._limit = limit_expr
        return self

    def offset(self, offset: Optional[int]) -> InfinityLocalQueryBuilder:
        constant_exp = WrapConstantExpr(literal_type=LiteralType.kInteger, i64_value=offset)
        offset_expr = WrapParsedExpr(ParsedExprType.kConstant)
        offset_expr.constant_expr = constant_exp
        self._offset = offset_expr
        return self

    def output(self, columns: Optional[list]) -> InfinityLocalQueryBuilder:
        self._columns = columns
        select_list: List[WrapParsedExpr] = []
        for column in columns:
            if isinstance(column, str):
                column = column.lower()

            match column:
                case "*":
                    column_expr = WrapColumnExpr()
                    column_expr.star = True

                    parsed_expr = WrapParsedExpr(ParsedExprType.kColumn)
                    parsed_expr.column_expr = column_expr

                    select_list.append(parsed_expr)
                case "_row_id":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "row_id"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    select_list.append(parsed_expr)
                case "_score":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "score"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    select_list.append(parsed_expr)

                case "_similarity":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "similarity"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    select_list.append(parsed_expr)
                case "_distance":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "distance"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    select_list.append(parsed_expr)

                case _:
                    parsed_expr = parse_expr(maybe_parse(column))
                    select_list.append(parsed_expr)

        self._columns = select_list
        return self

    def to_result(self):
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
            data_series = pd.Series(v, dtype=logic_type_to_dtype(data_type_dict[k]))
            df_dict[k] = data_series
        return pd.DataFrame(df_dict)

    def to_pl(self) -> pl.DataFrame:
        return pl.from_pandas(self.to_df())

    def to_arrow(self) -> Table:
        return pa.Table.from_pandas(self.to_df())

    def explain(self, explain_type=ExplainType.kPhysical) -> Any:
        query = ExplainQuery(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset,
            explain_type=explain_type
        )
        return self._table._explain_query(query)
