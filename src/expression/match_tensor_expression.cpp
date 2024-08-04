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
                                             const String &options_text)
    : BaseExpression(ExpressionType::kMatchTensor, std::move(search_column)), search_method_(search_method),
      embedding_data_type_(embedding_data_type), dimension_(dimension), query_embedding_(std::move(query_embedding)),
      tensor_basic_embedding_dimension_(tensor_basic_embedding_dimension), options_text_(options_text) {
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
    if (options_text_.empty()) {
        return fmt::format("MATCH TENSOR ({}, {}, {})", column_expr_->Name(), tensor_str, MethodToString(search_method_));
    }
    return fmt::format("MATCH TENSOR ({}, {}, {}, '{}')", column_expr_->Name(), tensor_str, MethodToString(search_method_), options_text_);
}

} // namespace infinity
