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
#include "type/datetime/interval_type.h"
#include <vector>

namespace infinity {

enum class LiteralType {
    kBoolean,
    kDouble,
    kString,
    kInteger,
    kNull,
    kDate,
    kTime,
    kDateTime,
    kTimestamp,
    kIntegerArray,
    kDoubleArray,
    kInterval,
};

class ConstantExpr : public ParsedExpr {
public:
    explicit ConstantExpr(LiteralType literal_type) : ParsedExpr(ParsedExprType::kConstant), literal_type_(literal_type) {}

    ~ConstantExpr() override;

    [[nodiscard]] std::string ToString() const override;

public:
    LiteralType literal_type_;

    bool bool_value_{false};
    int64_t integer_value_{0};
    double double_value_{0};
    char *str_value_{nullptr};
    TimeUnit interval_type_{TimeUnit::kInvalidUnit};
    char *date_value_{nullptr};
    std::vector<int64_t> long_array_{};
    std::vector<double> double_array_{};
};

} // namespace infinity
