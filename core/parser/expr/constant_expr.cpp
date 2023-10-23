//
// Created by jinhai on 23-2-27.
//

#include "constant_expr.h"
#include "parser_assert.h"
#include "spdlog/fmt/fmt.h"
#include <sstream>

namespace infinity {

ConstantExpr::~ConstantExpr() {
    if (literal_type_ == LiteralType::kString) {
        free(str_value_);
        return;
    }
    if (literal_type_ == LiteralType::kDate) {
        free(date_value_);
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
        case LiteralType::kDate: {
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
