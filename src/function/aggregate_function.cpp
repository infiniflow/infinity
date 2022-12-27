//
// Created by JinHai on 2022/9/14.
//

#include "aggregate_function.h"
#include "common/utility/infinity_assert.h"

#include <sstream>
#include <utility>

namespace infinity {

AggregateFunction::AggregateFunction(std::string name,
                                     Vector<DataType> argument_types,
                                     DataType return_type)
                                     : Function(std::move(name), FunctionType::kAggregate),
                                       parameter_types_(std::move(argument_types)),
                                       return_type_(std::move(return_type))
{}

void
AggregateFunction::CastArgumentTypes(std::vector<BaseExpression>& input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type
    auto arguments_count = input_arguments.size();
    PlannerAssert(input_arguments.size() != arguments_count, "Function :" + name_ + " arguments number isn't matched.");
    for(auto idx = 0; idx < arguments_count; ++ idx) {
        if(parameter_types_[idx] != input_arguments[idx].Type()) {
            PlannerError("Not implemented: need to cast the argument types");
        }
    }
}

std::string
AggregateFunction::ToString() const {

    std::stringstream ss;
    ss << name_;
    auto parameter_count = parameter_types_.size();
    if(parameter_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for(auto i = 0; i < parameter_count - 1; ++ i) {
            ss << parameter_types_[i].ToString() << ", ";
        }
        ss << parameter_types_.back().ToString();
        ss << ")";
    }

    return ss.str();
}

}