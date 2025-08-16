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

export module infinity_core:match_tensor_expression;

import :base_expression;
import :column_expression;

import match_tensor_expr;
import data_type;
import logical_type;
import knn_expr;
import internal_types;

namespace infinity {

export class MatchTensorExpression final : public BaseExpression {
public:
    MatchTensorExpression(std::vector<std::shared_ptr<BaseExpression>> search_column,
                          MatchTensorSearchMethod search_method,
                          EmbeddingDataType embedding_data_type,
                          u32 dimension,
                          EmbeddingT query_embedding,
                          u32 tensor_basic_embedding_dimension,
                          const std::string &options_text,
                          std::shared_ptr<BaseExpression> optional_filterm,
                          std::string index_name,
                          bool ignore_index);

    DataType Type() const override;

    std::string ToString() const override;

    static std::string MethodToString(MatchTensorSearchMethod method);

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

    const MatchTensorSearchMethod search_method_;
    const ColumnExpression *column_expr_ = nullptr;
    const EmbeddingDataType embedding_data_type_;
    const u32 dimension_;                        // num of total elements in the tensor (num of embedding * dimension of single embedding)
    EmbeddingT query_embedding_;                 // treat the query tensor as an embedding here
    const u32 tensor_basic_embedding_dimension_; // dimension of single embedding in the tensor column
    const u32 num_of_embedding_in_query_tensor_ = dimension_ / tensor_basic_embedding_dimension_;
    const std::string options_text_;
    std::shared_ptr<BaseExpression> optional_filter_;
    const std::string index_name_;
    bool ignore_index_;
};

} // namespace infinity
