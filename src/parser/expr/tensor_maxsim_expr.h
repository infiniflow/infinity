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

#include "parsed_expr.h"
#include "type/complex/embedding_type.h"
namespace infinity {

class TensorMaxSimExpr final : public ParsedExpr {
public:
    explicit TensorMaxSimExpr(bool own_memory = true) : ParsedExpr(ParsedExprType::kTensorMaxSim), own_memory_(own_memory) {}

    ~TensorMaxSimExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    const bool own_memory_;

    ParsedExpr *column_expr_{};
    void *embedding_data_ptr_{}; // Pointer to the embedding data ,the data type include float, int ,char ...., so we use void* here
    uint32_t dimension_{};
    EmbeddingDataType embedding_data_type_{EmbeddingDataType::kElemInvalid};
    std::string options_text_;
};

} // namespace infinity