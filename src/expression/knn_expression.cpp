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
                             SharedPtr<BaseExpression> optional_filter,
                             String using_index,
                             bool ignore_index)
    : BaseExpression(ExpressionType::kKnn, std::move(arguments)), dimension_(dimension), embedding_data_type_(embedding_data_type),
      distance_type_(knn_distance_type), query_embedding_(std::move(query_embedding)),
      topn_(topn), // Should call move constructor, otherwise there will be memory leak.
      using_index_(std::move(using_index)), ignore_index_(ignore_index), optional_filter_(std::move(optional_filter)) {
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

    String expr_str = fmt::format("MATCH VECTOR ({}, {}, {}, {}, {}{})",
                                  arguments_.at(0)->Name(),
                                  EmbeddingT::Embedding2String(query_embedding_, embedding_data_type_, dimension_),
                                  EmbeddingT::EmbeddingDataType2String(embedding_data_type_),
                                  KnnDistanceType2Str(distance_type_),
                                  topn_,
                                  optional_filter_ ? fmt::format(", WHERE {}", optional_filter_->ToString()) : "");

    return expr_str;
}

u64 KnnExpression::Hash() const {
    u64 h = 0;
    h = std::hash<i64>()(dimension_);
    h ^= std::hash<EmbeddingDataType>()(embedding_data_type_);
    h ^= std::hash<KnnDistanceType>()(distance_type_);
    h ^= std::hash<i32>()(topn_);
    if (optional_filter_) {
        h ^= optional_filter_->Hash();
    }
    return h;
}

bool KnnExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kKnn) {
        return false;
    }
    const auto &other = static_cast<const KnnExpression &>(other_base);
    bool eq = dimension_ == other.dimension_ && embedding_data_type_ == other.embedding_data_type_ && distance_type_ == other.distance_type_ &&
              query_embedding_.Eq(other.query_embedding_, embedding_data_type_, dimension_) && topn_ == other.topn_ &&
              opt_params_ == other.opt_params_;
    if (!eq) {
        return false;
    }
    if (optional_filter_ && other.optional_filter_) {
        return optional_filter_->Eq(*other.optional_filter_);
    }
    if (bool(optional_filter_) ^ bool(other.optional_filter_)) {
        return false;
    }
    return true;
}

} // namespace infinity
