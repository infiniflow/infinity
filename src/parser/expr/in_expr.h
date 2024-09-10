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
#include <string>
#include <vector>

namespace infinity {

class InExpr : public ParsedExpr {
public:
    explicit InExpr(bool in = true) : ParsedExpr(ParsedExprType::kIn), in_(in) {}

    ~InExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    ParsedExpr *left_{nullptr};
    std::vector<ParsedExpr *> *arguments_{nullptr};
    bool in_{false};
};

} // namespace infinity
