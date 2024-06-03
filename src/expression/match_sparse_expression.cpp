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
                                             const ConstantExpr *query_sparse_expr,
                                             SparseMetricType metric_type,
                                             SizeT topn,
                                             const Vector<UniquePtr<InitParameter>> &opt_params)
    : BaseExpression(ExpressionType::kMatchSparse, std::move(search_column)), metric_type_(metric_type), topn_(topn), opt_params_(opt_params) {
    column_expr_ = static_cast<const ColumnExpression *>(arguments_[0].get());
    this->MakeQuery(query_sparse_expr);
}

DataType MatchSparseExpression::Type() const { return DataType(LogicalType::kFloat); }

void MatchSparseExpression::MakeQuery(const ConstantExpr *query_sparse_expr) {
    SharedPtr<ValueExpression> value_expr = nullptr;
    {
        Value query_sparse(DataType{LogicalType::kInvalid});
        switch (query_sparse_expr->literal_type_) {
            case LiteralType::kLongSparseArray: {
                query_sparse = Value::MakeSparse(query_sparse_expr->long_sparse_array_);
                break;
            }
            case LiteralType::kDoubleSparseArray: {
                query_sparse = Value::MakeSparse(query_sparse_expr->double_sparse_array_);
                break;
            }
            default: {
                Status status = Status::NotSupport(fmt::format("Unsupported query sparse data type: {}", query_sparse_expr->ToString()));
                LOG_ERROR(status.message());
                RecoverableError(std::move(status));
            }
        }
        value_expr = MakeShared<ValueExpression>(std::move(query_sparse));
    }

    const auto &column_type = column_expr_->Type();
    BoundCastFunc cast = CastFunction::GetBoundFunc(value_expr->GetValue().type(), column_type);
    query_sparse_expr_ = MakeShared<CastExpression>(cast, value_expr, column_type);
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

    return fmt::format("MATCH SPARSE ({}, [{}], {}, {}) WITH ({})",
                       column_expr_->Name(),
                       sparse_str,
                       MatchSparseExpr::MetricTypeToString(metric_type_),
                       topn_,
                       opt_str);
}

} // namespace infinity
