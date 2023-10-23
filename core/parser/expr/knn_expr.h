//
// Created by jinhai on 23-9-21.
//

#pragma once

#include "expr.h"
#include "type/complex/embedding_type.h"

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

    [[nodiscard]] std::string ToString() const override;

public:
    static std::string KnnDistanceType2Str(KnnDistanceType knn_distance_type);

public:
    ParsedExpr *column_expr_{};
    void *embedding_data_ptr_{};
    int64_t dimension_{};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type_{KnnDistanceType::kInvalid};
};

} // namespace infinity