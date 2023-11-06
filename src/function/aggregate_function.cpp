//
// Created by JinHai on 2022/9/14.
//

module;

import std;
import base_expression;

import infinity_exception;

module aggregate_function;

namespace infinity {

void AggregateFunction::CastArgumentTypes(BaseExpression &input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type

    Error<PlannerException>("Not implemented: need to cast the argument types");
}

std::string AggregateFunction::ToString() const {

    std::stringstream ss;
    ss << name_;
    ss << "(";
    ss << argument_type_.ToString();
    ss << ")->" << return_type_.ToString();

    return ss.str();
}

} // namespace infinity