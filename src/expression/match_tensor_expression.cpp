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
#include<vector>

module match_tensor_expression;

import stl;
import expression_type;
import internal_types;
import third_party;
import base_expression;
import column_expression;
import embedding_info;
import infinity_exception;
import status;
import logger;
import logical_type;

namespace infinity {

MatchTensorExpression::MatchTensorExpression(Vector<SharedPtr<BaseExpression>> search_column,
                                             const MatchTensorSearchMethod search_method,
                                             const EmbeddingDataType embedding_data_type,
                                             const u32 dimension,
                                             EmbeddingT query_embedding,
                                             const u32 tensor_basic_embedding_dimension,
                                             const String &options_text,
                                             SharedPtr<BaseExpression> optional_filter)
    : BaseExpression(ExpressionType::kMatchTensor, std::move(search_column)), search_method_(search_method),
      embedding_data_type_(embedding_data_type), dimension_(dimension), query_embedding_(std::move(query_embedding)),
      tensor_basic_embedding_dimension_(tensor_basic_embedding_dimension), options_text_(options_text), optional_filter_(std::move(optional_filter)) {
    column_expr_ = static_cast<ColumnExpression *>(arguments_[0].get());
}

DataType MatchTensorExpression::Type() const { return DataType(LogicalType::kFloat); }

String MatchTensorExpression::MethodToString(const MatchTensorSearchMethod method) { return MatchTensorExpr::SearchMethodToString(method); }

String MatchTensorExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    auto tensor_str =
        TensorT::Tensor2String(query_embedding_.ptr, embedding_data_type_, tensor_basic_embedding_dimension_, num_of_embedding_in_query_tensor_);
    const auto options_str = options_text_.empty() ? "" : fmt::format(", '{}'", options_text_);
    const auto filter_str = optional_filter_ ? fmt::format(", WHERE {}", optional_filter_->ToString()) : "";
    return fmt::format("MATCH TENSOR ({}, {}, {}{}{})", column_expr_->Name(), tensor_str, MethodToString(search_method_), options_str, filter_str);
}

u64 MatchTensorExpression::Hash() const {
    u64 h = 0;
    for (const auto &arg : arguments_) {
        h ^= arg->Hash();
    }
    h ^= std::hash<MatchTensorSearchMethod>()(search_method_);
    h ^= column_expr_->Hash();
    h ^= std::hash<EmbeddingDataType>()(embedding_data_type_);
    h ^= std::hash<u32>()(dimension_);
    h ^= std::hash<u32>()(tensor_basic_embedding_dimension_);
    h ^= std::hash<String>()(options_text_);
    return h;
}

bool MatchTensorExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kMatchTensor) {
        return false;
    }
    const auto &other = static_cast<const MatchTensorExpression &>(other_base);
    if (arguments_.size() != other.arguments_.size()) {
        return false;
    }
    for (SizeT i = 0; i < arguments_.size(); ++i) {
        if (!arguments_[i]->Eq(*other.arguments_[i])) {
            return false;
        }
    }
    bool eq = search_method_ == other.search_method_ && column_expr_->Eq(*other.column_expr_) && embedding_data_type_ == other.embedding_data_type_ &&
              dimension_ == other.dimension_ && query_embedding_.Eq(other.query_embedding_, embedding_data_type_, dimension_) &&
              tensor_basic_embedding_dimension_ == other.tensor_basic_embedding_dimension_ && options_text_ == other.options_text_;
    return eq;
}

} // namespace infinity
