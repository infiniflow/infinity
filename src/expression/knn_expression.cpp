//
// Created by jinhai on 23-9-20.
//

#include "knn_expression.h"

namespace infinity {

String KnnExpression::KnnDistanceType2Str(KnnDistanceType type) {
    switch (type) {
        case KnnDistanceType::kInvalid: {
            PlannerError("Invalid KNN distance type")
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

    String expr_str = fmt::format("KNN({}, {}, {}, Float32, {})",
                                  arguments_.at(0)->Name(),
                                  EmbeddingType::Embedding2String(query_embedding_, embedding_data_type_, dimension_),
                                  dimension_,
                                  KnnDistanceType2Str(distance_type_));

    return expr_str;
}

} // namespace infinity
