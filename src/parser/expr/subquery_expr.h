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

#include "expr.h"
#include "statement/select_statement.h"

namespace infinity {

enum class SubqueryType {
    kExists,
    kNotExists,
    kIn,
    kNotIn,
    kScalar,
    kAny,
};

class SubqueryExpr final : public ParsedExpr {
public:
    explicit SubqueryExpr() : ParsedExpr(ParsedExprType::kSubquery) {}

    ~SubqueryExpr() final;

    [[nodiscard]] std::string ToString() const override;

public:
    SubqueryType subquery_type_{SubqueryType::kScalar};

    ParsedExpr *left_{nullptr};
    SelectStatement *select_{nullptr};
};

} // namespace infinity
