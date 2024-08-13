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

import scalar_function;
import logger;
import infinity_exception;
import third_party;
import statement_common;

module knn_expression;

namespace infinity {

String KnnExpression::KnnDistanceType2Str(KnnDistanceType type) {
    switch (type) {
        case KnnDistanceType::kInvalid: {
            String error_message = "Invalid KNN distance type";
            UnrecoverableError(error_message);
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
                             Vector<SharedPtr<BaseExpression>> arguments,
                             i64 topn,
                             Vector<InitParameter *> *opt_params,
                             String using_index,
                             bool ignore_index)
    : BaseExpression(ExpressionType::kKnn, std::move(arguments)), dimension_(dimension), embedding_data_type_(embedding_data_type),
      distance_type_(knn_distance_type), query_embedding_(std::move(query_embedding)),
      topn_(topn), // Should call move constructor, otherwise there will be memory leak.
      using_index_(std::move(using_index)),
      ignore_index_(ignore_index)
{
    if (opt_params) {
        for (auto &param : *opt_params) {
            opt_params_.emplace_back(*param);
        }
    }
}

String KnnExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    String expr_str = fmt::format("MATCH VECTOR ({}, {}, {}, {}, {})",
                                  arguments_.at(0)->Name(),
                                  EmbeddingT::Embedding2String(query_embedding_, embedding_data_type_, dimension_),
                                  EmbeddingT::EmbeddingDataType2String(embedding_data_type_),
                                  KnnDistanceType2Str(distance_type_),
                                  topn_);

    return expr_str;
}

} // namespace infinity
