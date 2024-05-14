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

namespace infinity {

export class TensorMaxSimExpression : public BaseExpression {
public:
    TensorMaxSimExpression(const String &tensor_column_name,
                           EmbeddingDataType embedding_data_type,
                           u32 dimension,
                           EmbeddingT query_embedding,
                           const String &options_text);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

    String tensor_column_name_;
    const EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    const u32 dimension_{0};
    const EmbeddingT query_embedding_;
    String options_text_;
};

} // namespace infinity