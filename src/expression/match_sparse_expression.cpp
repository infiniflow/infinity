// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <sstream>
#include <vector>

module match_sparse_expression;

import stl;
import logical_type;
import internal_types;
import third_party;
import column_expression;
import status;
import logger;
import infinity_exception;
import cast_function;
import cast_expression;
import bound_cast_func;
import value;
import constant_expr;
import sparse_info;
import value_expression;

namespace infinity {

MatchSparseExpression::MatchSparseExpression(Vector<SharedPtr<BaseExpression>> search_column,
                                             SharedPtr<BaseExpression> query_sparse_expr,
                                             SparseMetricType metric_type,
                                             SizeT query_n,
                                             SizeT topn,
                                             const Vector<UniquePtr<InitParameter>> &opt_params,
                                             SharedPtr<BaseExpression> optional_filter)
    : BaseExpression(ExpressionType::kMatchSparse, std::move(search_column)), metric_type_(metric_type), query_n_(query_n), topn_(topn),
      opt_params_(opt_params), optional_filter_(std::move(optional_filter)) {
    column_expr_ = static_cast<const ColumnExpression *>(arguments_[0].get());
    this->MakeQuery(query_sparse_expr);
}

DataType MatchSparseExpression::Type() const {
    const DataType &column_type = column_expr_->Type();
    const auto *sparse_info = static_cast<const SparseInfo *>(column_type.type_info().get());
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemBit: {
            switch (sparse_info->IndexType()) {
                case EmbeddingDataType::kElemInt8: {
                    return DataType(LogicalType::kTinyInt);
                }
                case EmbeddingDataType::kElemInt16: {
                    return DataType(LogicalType::kSmallInt);
                }
                case EmbeddingDataType::kElemInt32: {
                    return DataType(LogicalType::kInteger);
                }
                case EmbeddingDataType::kElemInt64: {
                    return DataType(LogicalType::kBigInt);
                }
                default: {
                    UnrecoverableError("Invalid index type.");
                }
            }
        }
        case EmbeddingDataType::kElemFloat: {
            return DataType(LogicalType::kFloat);
        }
        case EmbeddingDataType::kElemDouble: {
            return DataType(LogicalType::kDouble);
        }
        case EmbeddingDataType::kElemUInt8:
        case EmbeddingDataType::kElemInt8:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64: {
            return DataType(LogicalType::kFloat);
        }
        default: {
            UnrecoverableError("Unimpelmented.");
        }
    }
    return DataType(LogicalType::kInvalid);
}

void MatchSparseExpression::MakeQuery(SharedPtr<BaseExpression> query_sparse_expr) {
    const auto &column_type = column_expr_->Type();
    BoundCastFunc cast = CastFunction::GetBoundFunc(query_sparse_expr->Type(), column_type);
    query_sparse_expr_ = MakeShared<CastExpression>(cast, query_sparse_expr, column_type);
}

String MatchSparseExpression::ToString() const {
    String sparse_str = query_sparse_expr_->ToString();

    std::stringstream ss;
    size_t opt_num = opt_params_.size();
    for (size_t i = 0; i < opt_num; ++i) {
        ss << opt_params_[i]->ToString();
        if (i < opt_num - 1) {
            ss << ", ";
        }
    }
    String opt_str = ss.str();

    return fmt::format("MATCH SPARSE ({}, [{}], {}, {}{}) WITH ({})",
                       column_expr_->Name(),
                       sparse_str,
                       MatchSparseExpr::MetricTypeToString(metric_type_),
                       topn_,
                       optional_filter_ ? fmt::format(", WHERE {}", optional_filter_->ToString()) : "",
                       opt_str);
}

} // namespace infinity
