//
// Created by JinHai on 2022/8/5.
//

#include "aggregate_expression.h"

#include <sstream>
#include <utility>

namespace infinity {

AggregateExpression::AggregateExpression(AggregateFunction aggregate_function,
                                         std::vector<std::shared_ptr<BaseExpression>> arguments)
     : BaseExpression(ExpressionType::kAggregate, std::move(arguments)),
     aggregate_function_(std::move(aggregate_function)) {}

bool
AggregateExpression::IsCountStar() const {
    return false;
}

std::string
AggregateExpression::ToString() const {
    return aggregate_function_.name();
}

LogicalType
AggregateExpression::DataType() {
    return aggregate_function_.return_type();
}

}

