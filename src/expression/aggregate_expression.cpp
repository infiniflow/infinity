//
// Created by JinHai on 2022/8/5.
//

#include "aggregate_expression.h"

#include <sstream>

namespace infinity {

AggregateExpression::AggregateExpression(AggFuncType agg_func_type,
                                         const std::shared_ptr<BaseExpression> &argument)
     : BaseExpression(ExpressionType::kAggregate, {argument}), agg_func_type_(agg_func_type) {}

bool
AggregateExpression::IsCountStar() const {
    return false;
}

std::string
AggregateExpression::ToString() const {
    std::stringstream ss;

    if(agg_func_type_ == AggFuncType::kCountDistinct) {
        // Aggregate: Count Distinct

    } else if (IsCountStar()) {
        // Count(*)

    } else {
        // Other Aggregate Function
        switch(agg_func_type_) {

            case AggFuncType::kMin:
                ss << "Min";
                break;
            case AggFuncType::kMax:
                ss << "Max";
                break;
            case AggFuncType::kSum:
                ss << "Sum";
                break;
            case AggFuncType::kAvg:
                ss << "Avg";
                break;
            case AggFuncType::kCount:
                ss << "Count";
                break;
            case AggFuncType::kCountDistinct:
                ss << "CountDistinct";
                break;
            case AggFuncType::kAny:
                ss << "Any";
                break;
        }

        ss << "(";
        ss << arguments_[0]->ToString();
        ss << ")";
    }

    return ss.str();
}

LogicalType
AggregateExpression::DataType() {
    return LogicalType(LogicalTypeId::kBigInt);
}

}

