//
// Created by JinHai on 2022/8/5.
//

module;

#include <sstream>

import stl;
import expression_type;
import aggregate_function;
import parser;

module aggregate_expression;

namespace infinity {

AggregateExpression::AggregateExpression(AggregateFunction aggregate_function, Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kAggregate, Move(arguments)), aggregate_function_(Move(aggregate_function)) {}

bool AggregateExpression::IsCountStar() const { return false; }

String AggregateExpression::ToString() const {
    std::stringstream ss;
    ss << aggregate_function_.name() << "(";
    if (arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for (auto i = 0; i < arguments_count - 1; ++i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }

    return ss.str();
}

DataType AggregateExpression::Type() const { return aggregate_function_.return_type(); }

} // namespace infinity
