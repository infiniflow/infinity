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

class MatchTensorExpr final : public ParsedExpr {
public:
    explicit MatchTensorExpr(bool own_memory = true) : ParsedExpr(ParsedExprType::kMatchTensor), own_memory_(own_memory) {}
    [[nodiscard]] std::string ToString() const override;

    const bool own_memory_;
    std::string search_method_;
    std::unique_ptr<ParsedExpr> column_expr_;
    std::unique_ptr<ConstantExpr> tensor_expr_;
    std::string embedding_data_type_;
    std::string options_text_;
};

} // namespace infinity