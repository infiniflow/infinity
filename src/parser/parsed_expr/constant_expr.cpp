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

String ConstantExpr::ToString() const {
    switch(literal_type_) {
        case LiteralType::kBoolean:
            return fmt::format("{}", bool_value_);
        case LiteralType::kFloat:
            return fmt::format("{}", float_value_);
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
                case IntervalExprType::kSecond: {
                    return fmt::format("{} seconds", integer_value_);
                }
                case IntervalExprType::kMinute: {
                    return fmt::format("{} minutes", integer_value_);
                }
                case IntervalExprType::kHour: {
                    return fmt::format("{} hours", integer_value_);
                }
                case IntervalExprType::kDay: {
                    return fmt::format("{} days", integer_value_);
                }
                case IntervalExprType::kMonth: {
                    return fmt::format("{} months", integer_value_);
                }
                case IntervalExprType::kYear: {
                    return fmt::format("{} years", integer_value_);
                }
            }
            break;
        }
    }
    PlannerError("Unexpected branch");
}

}
