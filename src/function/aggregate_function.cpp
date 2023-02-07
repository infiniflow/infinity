//
// Created by JinHai on 2022/9/14.
//

#include "aggregate_function.h"
#include "common/utility/infinity_assert.h"

#include <sstream>
#include <utility>

namespace infinity {

AggregateFunction::AggregateFunction(String name,
                                     DataType argument_type,
                                     DataType return_type)
                                     : Function(std::move(name), FunctionType::kAggregate),
                                       parameter_type_(std::move(argument_type)),
                                       return_type_(std::move(return_type))
{}

void
AggregateFunction::CastArgumentTypes(BaseExpression& input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type

    PlannerError("Not implemented: need to cast the argument types");
}

std::string
AggregateFunction::ToString() const {

    std::stringstream ss;
    ss << name_;
    ss << "(";
    ss << parameter_type_.ToString();
    ss << ")";

    return ss.str();
}

}