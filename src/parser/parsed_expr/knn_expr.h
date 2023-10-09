//
// Created by jinhai on 23-9-21.
//

#pragma once

#include "common/types/complex/embedding_type.h"
#include "parsed_expr.h"

namespace infinity {

enum class KnnDistanceType {
    kInvalid,
    kL2, // Euclidean distance
    kCosine,
    kInnerProduct,
    kHamming,
};

class KnnExpr : public ParsedExpr {
public:
    explicit KnnExpr() : ParsedExpr(ParsedExprType::kKnn) {}

    ~KnnExpr() override;

    [[nodiscard]] String ToString() const override;

public:
    static String KnnDistanceType2Str(KnnDistanceType knn_distance_type);

public:
    ParsedExpr *column_expr_{};
    void *embedding_data_ptr_{};
    i64 dimension_{};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type_{KnnDistanceType::kInvalid};
};

} // namespace infinity