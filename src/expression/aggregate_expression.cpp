//
// Created by JinHai on 2022/8/5.
//

#include "aggregate_expression.h"

#include <sstream>
#include <utility>

namespace infinity {

AggregateExpression::AggregateExpression(AggregateFunction aggregate_function,
                                         Vector<SharedPtr<BaseExpression>> arguments)
     : BaseExpression(ExpressionType::kAggregate, std::move(arguments)),
     aggregate_function_(std::move(aggregate_function)) {}

bool
AggregateExpression::IsCountStar() const {
    return false;
}

String
AggregateExpression::ToString() const {
    std::stringstream ss;
    ss << aggregate_function_.name() << "(";
    if(arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for(auto i = 0; i < arguments_count - 1; ++ i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }

    return ss.str();
}

DataType
AggregateExpression::Type() const {
    return aggregate_function_.return_type();
}

}

