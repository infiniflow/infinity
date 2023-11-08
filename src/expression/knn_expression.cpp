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
import stl;
import expression_type;
import parser;
import scalar_function;

import infinity_exception;
import third_party;

module knn_expression;

namespace infinity {

String KnnExpression::KnnDistanceType2Str(KnnDistanceType type) {
    switch (type) {
        case KnnDistanceType::kInvalid: {
            Error<PlannerException>("Invalid KNN distance type");
        }
        case KnnDistanceType::kL2: {
            return "L2";
        }
        case KnnDistanceType::kCosine: {
            return "Cosine";
        }
        case KnnDistanceType::kInnerProduct: {
            return "InnerProduct";
        }
        case KnnDistanceType::kHamming: {
            return "Hamming";
        }
    }
}

KnnExpression::KnnExpression(EmbeddingDataType embedding_data_type,
                             i64 dimension,
                             KnnDistanceType knn_distance_type,
                             EmbeddingT query_embedding,
                             Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kKnn, Move(arguments)), dimension_(dimension), embedding_data_type_(embedding_data_type),
      distance_type_(knn_distance_type), query_embedding_(Move(query_embedding)) // Should call move constructor, otherwise there will be memory leak.
{}

String KnnExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    String expr_str = Format("KNN({}, {}, {}, Float32, {})",
                             arguments_.at(0)->Name(),
                             EmbeddingT::Embedding2String(query_embedding_, embedding_data_type_, dimension_),
                             dimension_,
                             KnnDistanceType2Str(distance_type_));

    return expr_str;
}

} // namespace infinity
