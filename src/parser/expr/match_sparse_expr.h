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

#include "constant_expr.h"
#include "parsed_expr.h"
#include "statement/statement_common.h"

namespace infinity {

enum class SparseMetricType : uint8_t {
    kInnerProduct,
    kInvalid,
};

class MatchSparseExpr final : public ParsedExpr {
public:
    explicit MatchSparseExpr(bool own_memory = true) : ParsedExpr(ParsedExprType::kMatchSparse), own_memory_(own_memory) {}

    void SetSearchColumn(ParsedExpr *&column_expr);

    void SetQuerySparse(char *&raw_data_type, ConstantExpr *raw_sparse_expr);

    void SetMetricType(char *&raw_metric_type);

    void SetOptParams(size_t topn, std::vector<InitParameter *> *&opt_params);

    static std::string MetricTypeToString(SparseMetricType metric);

    [[nodiscard]] std::string ToString() const override;

public:
    const bool own_memory_;
    std::unique_ptr<ParsedExpr> column_expr_;
    
    EmbeddingDataType embedding_data_type_ = EmbeddingDataType::kElemInvalid;
    EmbeddingDataType embedding_indice_type_ = EmbeddingDataType::kElemInt64; // indice type is always int64
    std::unique_ptr<char[]> query_sparse_indice_ptr_;
    std::unique_ptr<char[]> query_sparse_data_ptr_;
    uint32_t nnz_ = 0;
    int64_t max_indice_ = 0;

    SparseMetricType metric_type_;
    size_t topn_;

    std::vector<std::unique_ptr<InitParameter>> opt_params_{};
};

} // namespace infinity