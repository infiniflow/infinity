from __future__ import annotations

from abc import ABC
from typing import List, Optional, Any

import numpy as np
import pandas as pd
import polars as pl
import pyarrow as pa
from pyarrow import Table
from sqlglot import condition, maybe_parse

from infinity_embedded.common import VEC, SparseVector, InfinityException
from infinity_embedded.embedded_infinity_ext import *
from infinity_embedded.local_infinity.types import logic_type_to_dtype, make_match_tensor_expr
from infinity_embedded.local_infinity.utils import traverse_conditions, parse_expr
from infinity_embedded.local_infinity.utils import get_search_optional_filter_from_opt_params
from infinity_embedded.table import ExplainType as BaseExplainType
from infinity_embedded.errors import ErrorCode

class Query(ABC):
    def __init__(
        self,
        columns: Optional[List[WrapParsedExpr]],
        search: Optional[WrapSearchExpr],
        filter: Optional[WrapParsedExpr],
        limit: Optional[WrapParsedExpr],
        offset: Optional[WrapParsedExpr],
        sort: Optional[WrapOrderByExpr]
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
        columns: Optional[List[WrapParsedExpr]],
        search: Optional[WrapSearchExpr],
        filter: Optional[WrapParsedExpr],
        limit: Optional[WrapParsedExpr],
        offset: Optional[WrapParsedExpr],
        explain_type: Optional[BaseExplainType],
    ):
        super().__init__(columns, search, filter, limit, offset, None)
        self.explain_type = explain_type


class InfinityLocalQueryBuilder(ABC):
    def __init__(self, table):
        self._table = table
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None
        self._sort = []

    def reset(self):
        self._columns = None
        self._search = None
        self._filter = None
        self._limit = None
        self._offset = None
        self._sort = []

    def match_dense(
        self,
        vector_column_name: str,
        embedding_data: VEC,
        embedding_data_type: str,
        distance_type: str,
        topn: int,
        knn_params: {} = None,
    ) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
            self._search.match_exprs = []

        column_expr = WrapColumnExpr()
        column_expr.names = [vector_column_name]
        column_expr.star = False

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

        if embedding_data_type in ["int", "uint8", "int8", "int16", "int32", "int64", "tinyint", "unsigned tinyint",
                                   "smallint", "bigint"]:
            embedding_data = [int(x) for x in embedding_data]

        if embedding_data_type in ["float", "float32", "double", "float64", "float16", "bfloat16"]:
            embedding_data = [float(x) for x in embedding_data]

        data = EmbeddingData()
        elem_type = EmbeddingDataType.kElemFloat
        if embedding_data_type == "bit":
            elem_type = EmbeddingDataType.kElemBit
            raise Exception(f"Invalid embedding {embedding_data[0]} type")
        elif embedding_data_type in ["unsigned tinyint", "uint8"]:
            elem_type = EmbeddingDataType.kElemUInt8
            data.u8_array_value = embedding_data
        elif embedding_data_type in ["tinyint", "int8"]:
            elem_type = EmbeddingDataType.kElemInt8
            data.i8_array_value = embedding_data
        elif embedding_data_type in ["smallint", "int16"]:
            elem_type = EmbeddingDataType.kElemInt16
            data.i16_array_value = embedding_data
        elif embedding_data_type in ["int", "int32"]:
            elem_type = EmbeddingDataType.kElemInt32
            data.i32_array_value = embedding_data
        elif embedding_data_type in ["bigint", "int64"]:
            elem_type = EmbeddingDataType.kElemInt64
            data.i64_array_value = embedding_data
        elif embedding_data_type in ["float", "float32"]:
            elem_type = EmbeddingDataType.kElemFloat
            data.f32_array_value = embedding_data
        elif embedding_data_type in ["double", "float64"]:
            elem_type = EmbeddingDataType.kElemDouble
            data.f64_array_value = embedding_data
        elif embedding_data_type in ["float16"]:
            elem_type = EmbeddingDataType.kElemFloat16
            data.f16_array_value = embedding_data
        elif embedding_data_type in ["bfloat16"]:
            elem_type = EmbeddingDataType.kElemBFloat16
            data.bf16_array_value = embedding_data
        else:
            raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Invalid embedding {embedding_data[0]} type")

        dist_type = KnnDistanceType.kInvalid
        if distance_type == "l2":
            dist_type = KnnDistanceType.kL2
        elif distance_type == "cosine" or distance_type == "cos":
            dist_type = KnnDistanceType.kCosine
        elif distance_type == "ip":
            dist_type = KnnDistanceType.kInnerProduct
        elif distance_type == "hamming":
            dist_type = KnnDistanceType.kHamming
        else:
            raise InfinityException(ErrorCode.INVALID_KNN_DISTANCE_TYPE, f"Invalid distance type {distance_type}")

        knn_opt_params = []
        optional_filter = None
        if knn_params is not None:
            # check if there is a filter
            optional_filter = get_search_optional_filter_from_opt_params(knn_params)
            for k, v in knn_params.items():
                key = k.lower()
                value = v.lower()
                tmp_param = InitParameter()
                tmp_param.param_name = key
                tmp_param.param_value = value
                knn_opt_params.append(tmp_param)

        knn_expr = WrapKnnExpr()
        knn_expr.column_expr = column_expr
        knn_expr.embedding_data = data
        knn_expr.embedding_data_type = elem_type
        knn_expr.distance_type = dist_type
        knn_expr.topn = topn
        knn_expr.opt_params = knn_opt_params
        if optional_filter is not None:
            knn_expr.filter_expr = optional_filter

        generic_match_expr = WrapParsedExpr()
        generic_match_expr.type = ParsedExprType.kKnn
        generic_match_expr.knn_expr = knn_expr
        self._search.match_exprs += [generic_match_expr]
        return self

    def match_sparse(
        self,
        vector_column_name: str,
        sparse_data: SparseVector | dict,
        metric_type: str,
        topn: int,
        opt_params: {} = None,
    ) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
            self._search.match_exprs = list()

        column_expr = WrapColumnExpr()
        column_expr.names = [vector_column_name]
        column_expr.star = False

        if not isinstance(topn, int):
            raise InfinityException(
                ErrorCode.INVALID_TOPK_TYPE, f"Invalid topn, type should be embedded, but get {type(topn)}"
            )

        sparse_opt_params = []
        optional_filter = None
        if opt_params is not None:
            # check if there is a filter
            optional_filter = get_search_optional_filter_from_opt_params(opt_params)
            for k, v in opt_params.items():
                params = InitParameter()
                params.param_name = k
                params.param_value = v
                sparse_opt_params.append(params)
        match_sparse_expr = WrapMatchSparseExpr()
        match_sparse_expr.column_expr = column_expr
        if optional_filter is not None:
            match_sparse_expr.filter_expr = optional_filter

        sparse_expr = WrapConstantExpr()
        match sparse_data:
            case SparseVector([int(), *_] as indices, [int(), *_] as values):
                sparse_expr.literal_type = LiteralType.kLongSparseArray
                sparse_expr.i64_array_idx = indices
                sparse_expr.i64_array_value = values
            case SparseVector([int(), *_] as indices, [float(), *_] as values):
                sparse_expr.literal_type = LiteralType.kDoubleSparseArray
                sparse_expr.i64_array_idx = indices
                sparse_expr.f64_array_value = values
            case SparseVector([int(), *_], None):
                raise InfinityException(ErrorCode.INVALID_CONSTANT_TYPE,
                                        f"No values! Sparse data does not support bool value type now")
            case dict():
                if len(sparse_data) == 0:
                    raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Empty sparse vector")
                match next(iter(sparse_data.values())):
                    case int():
                        sparse_expr.literal_type = LiteralType.kLongSparseArray
                        sparse_expr.i64_array_idx = [int(kk) for kk in sparse_data.keys()]
                        sparse_expr.i64_array_value = [int(vv) for vv in sparse_data.values()]
                    case float():
                        sparse_expr.literal_type = LiteralType.kDoubleSparseArray
                        sparse_expr.i64_array_idx = [int(kk) for kk in sparse_data.keys()]
                        sparse_expr.f64_array_value = [float(vv) for vv in sparse_data.values()]
                    case _:
                        raise InfinityException(ErrorCode.INVALID_EXPRESSION,
                                                f"Invalid sparse vector value type: {type(next(iter(sparse_data.values())))}")
            case _:
                raise InfinityException(ErrorCode.INVALID_CONSTANT_TYPE,
                                        f"Invalid sparse data type {type(sparse_data)}")

        match_sparse_expr.sparse_expr = sparse_expr

        match_sparse_expr.metric_type = metric_type
        match_sparse_expr.topn = topn
        match_sparse_expr.opt_params = sparse_opt_params

        generic_match_expr = WrapParsedExpr()
        generic_match_expr.type = ParsedExprType.kMatchSparse
        generic_match_expr.match_sparse_expr = match_sparse_expr
        self._search.match_exprs += [generic_match_expr]
        return self

    def match_text(
        self, fields: str, matching_text: str, topn: int, extra_options: Optional[dict]
    ) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
            self._search.match_exprs = list()
        match_expr = WrapMatchExpr()
        match_expr.fields = fields
        match_expr.matching_text = matching_text
        options_text = f"topn={topn}"
        optional_filter = None
        if extra_options is not None:
            # check if there is a filter
            optional_filter = get_search_optional_filter_from_opt_params(extra_options)
            for k, v in extra_options.items():
                options_text += f";{k}={v}"
        match_expr.options_text = options_text
        if optional_filter is not None:
            match_expr.filter_expr = optional_filter

        generic_match_expr = WrapParsedExpr()
        generic_match_expr.type = ParsedExprType.kMatch
        generic_match_expr.match_expr = match_expr
        self._search.match_exprs += [generic_match_expr]
        return self

    def match_tensor(
        self,
        column_name: str,
        query_data: VEC,
        query_data_type: str,
        topn: int,
        extra_option: Optional[dict] = None,
    ) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
            self._search.match_exprs = list()
        option_str = f"topn={topn}"
        optional_filter = None
        if extra_option is not None:
            # check if there is a filter
            optional_filter = get_search_optional_filter_from_opt_params(extra_option)
            for k, v in extra_option.items():
                option_str += f";{k}={v}"
        match_tensor_expr = WrapParsedExpr(ParsedExprType.kMatchTensor)
        if optional_filter is not None:
            match_tensor_expr.filter_expr = optional_filter
        match_tensor_expr.match_tensor_expr = make_match_tensor_expr(
            vector_column_name=column_name,
            embedding_data=query_data,
            embedding_data_type=query_data_type,
            method_type="maxsim",
            extra_option=option_str,
        )
        self._search.match_exprs += [match_tensor_expr]
        return self

    def fusion(self, method: str, topn: int, fusion_params: Optional[dict]) -> InfinityLocalQueryBuilder:
        if self._search is None:
            self._search = WrapSearchExpr()
        fusion_expr = WrapFusionExpr()
        fusion_expr.method = method
        final_option_text = f"topn={topn}"
        if method in ["rrf", "weighted_sum"]:
            if isinstance(fusion_params, dict):
                for k, v in fusion_params.items():
                    if k == "topn":
                        raise InfinityException(ErrorCode.INVALID_EXPRESSION, "topn is not allowed in fusion params")
                    final_option_text += f";{k}={v}"
        elif method in ["match_tensor"]:
            fusion_expr.has_match_tensor_expr = True
            fusion_expr.match_tensor_expr = make_match_tensor_expr(
                vector_column_name=fusion_params["field"], embedding_data=fusion_params["query_tensor"],
                embedding_data_type=fusion_params["element_type"], method_type="maxsim", extra_option=None)
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Invalid fusion method")
        fusion_expr.options_text = final_option_text
        self._search.fusion_exprs += [fusion_expr]
        # *WARN*: A list in nanobind wrapped object is odd that append() does nothing!
        # However `list add list` still works.
        # self._search.fusion_exprs.append(fusion_expr)
        assert len(self._search.fusion_exprs) > 0
        return self

    def filter(self, where: Optional[str]) -> InfinityLocalQueryBuilder:
        where_expr = traverse_conditions(condition(where))
        self._filter = where_expr
        return self

    def limit(self, limit: Optional[int]) -> InfinityLocalQueryBuilder:
        constant_exp = WrapConstantExpr()
        constant_exp.literal_type = LiteralType.kInteger
        constant_exp.i64_value = limit
        limit_expr = WrapParsedExpr(ParsedExprType.kConstant)
        limit_expr.constant_expr = constant_exp
        self._limit = limit_expr
        return self

    def offset(self, offset: Optional[int]) -> InfinityLocalQueryBuilder:
        constant_exp = WrapConstantExpr()
        constant_exp.literal_type = LiteralType.kInteger
        constant_exp.i64_value = offset
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

    def sort(self, order_by_expr_list: Optional[List[list[str, bool]]]) -> InfinityLocalQueryBuilder:
        sort_list: List[WrapOrderByExpr] = []
        for order_by_expr in order_by_expr_list:
            if isinstance(order_by_expr[0], str):
                order_by_expr[0] = order_by_expr[0].lower()

            match order_by_expr[0]:
                case "*":
                    column_expr = WrapColumnExpr()
                    column_expr.star = True

                    parsed_expr = WrapParsedExpr(ParsedExprType.kColumn)
                    parsed_expr.column_expr = column_expr

                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_row_id":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "row_id"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_score":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "score"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_similarity":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "similarity"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)
                case "_distance":
                    func_expr = WrapFunctionExpr()
                    func_expr.func_name = "distance"
                    func_expr.arguments = []

                    expr_type = ParsedExprType(ParsedExprType.kFunction)
                    parsed_expr = WrapParsedExpr(expr_type)
                    parsed_expr.function_expr = func_expr

                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)
                case _:
                    parsed_expr = parse_expr(maybe_parse(order_by_expr[0]))
                    order_by_expr = WrapOrderByExpr(parsed_expr, order_by_expr[1])
                    sort_list.append(order_by_expr)

        self._sort = sort_list
        return self

    def to_result(self):
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

    def explain(self, explain_type=ExplainType.kPhysical) -> Any:
        query = ExplainQuery(
            columns=self._columns,
            search=self._search,
            filter=self._filter,
            limit=self._limit,
            offset=self._offset,
            explain_type=explain_type,
        )
        return self._table._explain_query(query)
