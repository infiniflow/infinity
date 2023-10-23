//
// Created by jinhai on 23-9-20.
//

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
                  Vector<SharedPtr<BaseExpression>> arguments);

    inline DataType Type() const override { return DataType(LogicalType::kFloat); }

    String ToString() const override;

public:
    i64 dimension_{0};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type_{KnnDistanceType::kInvalid};
    EmbeddingT query_embedding_;
};

} // namespace infinity