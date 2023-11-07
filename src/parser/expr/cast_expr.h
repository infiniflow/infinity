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
#include "type/data_type.h"
#include "type/logical_type.h"
#include "type/type_info.h"
#include <memory>

namespace infinity {

class CastExpr final : public ParsedExpr {
public:
    explicit CastExpr(LogicalType logical_type, std::shared_ptr<TypeInfo> type_info_ptr)
        : ParsedExpr(ParsedExprType::kCast), data_type_(logical_type, std::move(type_info_ptr)) {}

    ~CastExpr() final;

    [[nodiscard]] std::string ToString() const override;

public:
    DataType data_type_;
    ParsedExpr *expr_{nullptr};
};

} // namespace infinity
