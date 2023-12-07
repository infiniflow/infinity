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

import stl;
import base_expression;
import parser;

export module knn_expression;

namespace infinity {

export class KnnExpression : public BaseExpression {
public:
    static String KnnDistanceType2Str(KnnDistanceType type);

public:
    KnnExpression(EmbeddingDataType embedding_data_type,
                  i64 dimension,
                  KnnDistanceType knn_distance_type,
                  EmbeddingT query_embedding,
                  Vector<SharedPtr<BaseExpression>> arguments,
                  i64 topn);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

public:
    i64 dimension_{0};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type_{KnnDistanceType::kInvalid};
    EmbeddingT query_embedding_;
    i64 topn_;
};

} // namespace infinity