//
// Created by jinhai on 23-2-27.
//

#include "constant_expr.h"

namespace infinity {

ConstantExpr::~ConstantExpr() {
    if(literal_type_ == LiteralType::kString) {
        free(str_value_);
        return ;
    }
    if(literal_type_ == LiteralType::kDate) {
        free(date_value_);
    }
}

String
ConstantExpr::ToString() const {
    switch(literal_type_) {
        case LiteralType::kBoolean:
            return fmt::format("{}", bool_value_);
        case LiteralType::kDouble:
            return fmt::format("{}", double_value_);
        case LiteralType::kString:
            return fmt::format("{}", str_value_);
        case LiteralType::kInteger:
            return fmt::format("{}", integer_value_);
        case LiteralType::kNull: {
            PlannerError("Null constant value");
        }
        case LiteralType::kDate: {
            return fmt::format("{}", date_value_);
        }
        case LiteralType::kInterval: {
            switch(interval_type_) {
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
                    PlannerError("Invalid interval type.");
                }
            }
        }
        case LiteralType::kIntegerArray: {
            std::stringstream ss;
            SizeT len = long_array_.size();
            if(len <= 0) {
                PlannerError("Invalid long array length")
            }
            for(SizeT i = 0; i < len - 1; ++ i) {
                ss << long_array_[i] << ',';
            }
            ss << long_array_.back();
            return ss.str();
        }
        case LiteralType::kDoubleArray: {
            std::stringstream ss;
            SizeT len = double_array_.size();
            if(len <= 0) {
                PlannerError("Invalid double array length")
            }
            for(SizeT i = 0; i < len - 1; ++ i) {
                ss << double_array_[i] << ',';
            }
            ss << double_array_.back();
            return ss.str();
        }
    }
    PlannerError("Unexpected branch");
}

}
