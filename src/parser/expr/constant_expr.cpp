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

#include "constant_expr.h"
#include "parser_assert.h"
#include "spdlog/fmt/fmt.h"
#include <sstream>

namespace infinity {

ConstantExpr::~ConstantExpr() {
    switch (literal_type_) {
        case LiteralType::kString: {
            free(str_value_);
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            free(date_value_);
            break;
        }
        default:
            break;
    }
}

std::string ConstantExpr::ToString() const {
    switch (literal_type_) {
        case LiteralType::kBoolean:
            return fmt::format("{}", bool_value_);
        case LiteralType::kDouble:
            return fmt::format("{}", double_value_);
        case LiteralType::kString:
            return fmt::format("{}", str_value_);
        case LiteralType::kInteger:
            return fmt::format("{}", integer_value_);
        case LiteralType::kNull: {
            ParserError("Null constant value");
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            return fmt::format("{}", date_value_);
        }
        case LiteralType::kInterval: {
            switch (interval_type_) {
                case TimeUnit::kSecond: {
                    return fmt::format("{} seconds", integer_value_);
                }
                case TimeUnit::kMinute: {
                    return fmt::format("{} minutes", integer_value_);
                }
                case TimeUnit::kHour: {
                    return fmt::format("{} hours", integer_value_);
                }
                case TimeUnit::kDay: {
                    return fmt::format("{} days", integer_value_);
                }
                case TimeUnit::kMonth: {
                    return fmt::format("{} months", integer_value_);
                }
                case TimeUnit::kYear: {
                    return fmt::format("{} years", integer_value_);
                }
                default: {
                    ParserError("Invalid interval type.");
                }
            }
        }
        case LiteralType::kIntegerArray: {
            std::stringstream ss;
            size_t len = long_array_.size();
            if (len <= 0) {
                ParserError("Invalid long array length");
            }
            for (size_t i = 0; i < len - 1; ++i) {
                ss << long_array_[i] << ',';
            }
            ss << long_array_.back();
            return ss.str();
        }
        case LiteralType::kDoubleArray: {
            std::stringstream ss;
            size_t len = double_array_.size();
            if (len <= 0) {
                ParserError("Invalid double array length");
            }
            for (size_t i = 0; i < len - 1; ++i) {
                ss << double_array_[i] << ',';
            }
            ss << double_array_.back();
            return ss.str();
        }
    }
    ParserError("Unexpected branch");
}

} // namespace infinity
