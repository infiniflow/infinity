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
#include "type/datetime/interval_type.h"
#include "type/serialize.h"
#include <cstdint>
#include <cstring>
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
            return fmt::format("Null");
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

void ConstantExpr::WriteAdv(char *&ptr) const {
    WriteBufAdv<LiteralType>(ptr, literal_type_);
    switch (literal_type_) {
        case LiteralType::kBoolean: {
            WriteBufAdv<bool>(ptr, bool_value_);
            break;
        }
        case LiteralType::kDouble: {
            WriteBufAdv<double>(ptr, double_value_);
            break;
        }
        case LiteralType::kString: {
            WriteBufAdv<std::string>(ptr, std::string(str_value_));
            break;
        }
        case LiteralType::kInteger: {
            WriteBufAdv<int64_t>(ptr, integer_value_);
            break;
        }
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            WriteBufAdv<std::string>(ptr, std::string(date_value_));
            break;
        }
        case LiteralType::kIntegerArray: {
            WriteBufAdv<int64_t>(ptr, long_array_.size());
            for (const auto &val : long_array_) {
                WriteBufAdv<int64_t>(ptr, val);
            }
            break;
        }
        case LiteralType::kDoubleArray: {
            WriteBufAdv<int64_t>(ptr, double_array_.size());
            for (const auto &val : double_array_) {
                WriteBufAdv<double>(ptr, val);
            }
            break;
        }
        case LiteralType::kInterval: {
            WriteBufAdv<TimeUnit>(ptr, interval_type_);
            WriteBufAdv<int64_t>(ptr, integer_value_);
            break;
        }
    }
}

std::shared_ptr<ParsedExpr> ConstantExpr::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    LiteralType literal_type = ReadBufAdv<LiteralType>(ptr);
    auto const_expr = new ConstantExpr(literal_type);
    switch (literal_type) {
        case LiteralType::kBoolean: {
            bool bool_value = ReadBufAdv<bool>(ptr);
            const_expr->bool_value_ = bool_value;
            break;
        }
        case LiteralType::kDouble: {
            double double_value = ReadBufAdv<double>(ptr);
            const_expr->double_value_ = double_value;
            break;
        }
        case LiteralType::kString: {
            std::string str_value = ReadBufAdv<std::string>(ptr);
            const_expr->str_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kInteger: {
            int64_t integer_value = ReadBufAdv<int64_t>(ptr);
            const_expr->integer_value_ = integer_value;
            break;
        }
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            std::string date_value = ReadBufAdv<std::string>(ptr);
            const_expr->date_value_ = strdup(date_value.c_str());
            break;
        }
        case LiteralType::kIntegerArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            for (size_t i = 0; i < len; ++i) {
                int64_t val = ReadBufAdv<int64_t>(ptr);
                const_expr->long_array_.push_back(val);
            }
            break;
        }
        case LiteralType::kDoubleArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            for (size_t i = 0; i < len; ++i) {
                double val = ReadBufAdv<double>(ptr);
                const_expr->double_array_.push_back(val);
            }
            break;
        }
        case LiteralType::kInterval: {
            TimeUnit interval_type = ReadBufAdv<TimeUnit>(ptr);
            int64_t integer_value = ReadBufAdv<int64_t>(ptr);
            const_expr->interval_type_ = interval_type;
            const_expr->integer_value_ = integer_value;
            break;
        }
    }
    maxbytes = ptr_end - ptr;
    ParserAssert(maxbytes >= 0, "ptr goes out of range when reading constant expression");
    return std::shared_ptr<ParsedExpr>(const_expr);
}

} // namespace infinity
