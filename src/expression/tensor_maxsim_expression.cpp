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

module tensor_maxsim_expression;
import stl;
import expression_type;
import knn_expr;
import third_party;
import statement_common;

namespace infinity {

TensorMaxSimExpression::TensorMaxSimExpression(const String &tensor_column_name,
                                               EmbeddingDataType embedding_data_type,
                                               const u32 dimension,
                                               EmbeddingT query_embedding,
                                               const String &options_text)
    : BaseExpression(ExpressionType::kTensorMaxSim, Vector<SharedPtr<BaseExpression>>()), tensor_column_name_(tensor_column_name),
      embedding_data_type_(embedding_data_type), dimension_(dimension), query_embedding_(std::move(query_embedding)), options_text_(options_text) {}

String TensorMaxSimExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    return fmt::format("TensorMaxSim({}, [{}], {})",
                       tensor_column_name_,
                       EmbeddingT::Embedding2String(query_embedding_, embedding_data_type_, dimension_),
                       options_text_);
}

} // namespace infinity
