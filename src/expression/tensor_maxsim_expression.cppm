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

export module tensor_maxsim_expression;

import stl;
import data_type;
import logical_type;
import knn_expr;
import internal_types;
import base_expression;
import column_expression;

namespace infinity {

export class TensorMaxSimExpression final : public BaseExpression {
public:
    TensorMaxSimExpression(Vector<SharedPtr<BaseExpression>> search_column,
                           EmbeddingDataType embedding_data_type,
                           u32 dimension,
                           EmbeddingT query_embedding,
                           u32 tensor_basic_embedding_dimension,
                           const String &options_text);

    DataType Type() const override;

    String ToString() const override;

    const ColumnExpression *column_expr_ = nullptr;
    const EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    const u32 dimension_{0};                     // num of total elements in the tensor (num of embedding * dimension of single embedding)
    const EmbeddingT query_embedding_;           // treat the query tensor as an embedding here
    const u32 tensor_basic_embedding_dimension_; // dimension of single embedding in the tensor column
    const u32 num_of_embedding_in_query_tensor_ = dimension_ / tensor_basic_embedding_dimension_;
    String options_text_;
};

} // namespace infinity