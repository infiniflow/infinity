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

#include <string>

namespace infinity {

enum class ParsedExprType {
    kConstant,
    kParameter,
    kColumn,
    kFunction,
    kBetween,
    kOperator,
    kSubquery,
    kCase,
    kCast,
    kIn,
    kKnn,
    kMatch,
    kTensorMaxSim,
    kFusion,
    kSearch
};

class ParsedExpr {
public:
    explicit ParsedExpr(ParsedExprType type) : type_(type) {}

    virtual ~ParsedExpr() = default;

    [[nodiscard]] inline std::string GetName() const {
        if (HasAlias())
            return GetAlias();
        return ToString();
    }

    inline std::string GetAlias() const { return alias_; }

    [[nodiscard]] inline bool HasAlias() const { return !alias_.empty(); }

    [[nodiscard]] virtual std::string ToString() const = 0;

    ParsedExprType type_;
    std::string alias_{};
};

} // namespace infinity