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
#include "json.hpp"
#include "type/data_type.h"
#include "type/datetime/interval_type.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace infinity {

enum class LiteralType : int32_t {
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

    int32_t GetSizeInBytes() const;

    void WriteAdv(char *&ptr) const;

    static std::shared_ptr<ParsedExpr> ReadAdv(char *&ptr, int32_t maxbytes);

    nlohmann::json Serialize() const;

    static std::shared_ptr<ParsedExpr> Deserialize(const nlohmann::json &constant_expr);

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
