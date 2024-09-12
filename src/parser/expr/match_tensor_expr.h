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
#include <memory>

namespace infinity {

enum class MatchTensorSearchMethod : uint8_t {
    kInvalid,
    kMaxSim,
};

class MatchTensorExpr final : public ParsedExpr {
public:
    explicit MatchTensorExpr(bool own_memory = true) : ParsedExpr(ParsedExprType::kMatchTensor), own_memory_(own_memory) {}
    [[nodiscard]] std::string ToString() const override;
    void SetSearchMethod(char *&raw_search_method);
    void SetSearchMethodStr(std::string search_method);
    void SetSearchColumn(ParsedExpr *&column_expr) noexcept;
    void SetOptionalFilter(ParsedExpr *&filter_expr) noexcept;
    void SetExtraOptions(char *&raw_options_text);
    void SetQueryTensor(char *&raw_embedding_data_type, ConstantExpr *&raw_tensor_expr);
    void SetQueryTensorStr(std::string embedding_data_type, const ConstantExpr *tensor_expr);
    static std::string SearchMethodToString(MatchTensorSearchMethod method);
    const bool own_memory_;
    MatchTensorSearchMethod search_method_enum_ = MatchTensorSearchMethod::kInvalid;
    std::unique_ptr<ParsedExpr> column_expr_;
    EmbeddingDataType embedding_data_type_ = EmbeddingDataType::kElemInvalid;
    std::unique_ptr<char[]> query_tensor_data_ptr_;
    uint32_t dimension_ = 0;
    std::string options_text_;
    std::unique_ptr<ParsedExpr> filter_expr_;
};

} // namespace infinity