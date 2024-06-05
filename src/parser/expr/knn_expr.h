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

#pragma once

#include "expr/constant_expr.h"
#include "parsed_expr.h"
#include "statement/statement_common.h"
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
    explicit KnnExpr(bool own_memory = true) : ParsedExpr(ParsedExprType::kKnn), own_memory_(own_memory) {}

    ~KnnExpr() override;

    [[nodiscard]] std::string ToString() const override;

    bool InitDistanceType(const char *distance_type);

    bool InitEmbedding(const char *data_type, const ConstantExpr *query_vec);

public:
    static std::string KnnDistanceType2Str(KnnDistanceType knn_distance_type);

public:
    const bool own_memory_;

    ParsedExpr *column_expr_{};
    void *embedding_data_ptr_{}; // Pointer to the embedding data ,the data type include float, int ,char ...., so we use void* here
    int64_t dimension_{};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type_{KnnDistanceType::kInvalid};
    int64_t topn_{};
    std::vector<InitParameter *> *opt_params_{};
};

} // namespace infinity