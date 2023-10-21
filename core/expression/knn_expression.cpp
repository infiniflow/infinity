//
// Created by jinhai on 23-9-20.
//

module;

#include <sstream>

import stl;
import expression_type;
import parser;
import scalar_function;
import infinity_assert;
import infinity_exception;
import third_party;

module knn_expression;

namespace infinity {

String KnnExpression::KnnDistanceType2Str(KnnDistanceType type) {
    switch (type) {
        case KnnDistanceType::kInvalid: {
            Error<PlannerException>("Invalid KNN distance type", __FILE_NAME__, __LINE__);
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
    : BaseExpression(ExpressionType::kKnn, std::move(arguments)), dimension_(dimension), embedding_data_type_(embedding_data_type),
      distance_type_(knn_distance_type),
      query_embedding_(std::move(query_embedding)) // Should call move constructor, otherwise there will be memory leak.
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
