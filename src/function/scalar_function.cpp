//
// Created by JinHai on 2022/9/14.
//

#include "scalar_function.h"
#include "common/utility/infinity_assert.h"
#include <utility>
#include <sstream>

namespace infinity {

ScalarFunction::ScalarFunction(std::string name,
                               std::vector<LogicalType> argument_types,
                               LogicalType return_type,
                               ScalarFunctionType function)
                               : Function(std::move(name), FunctionType::kScalar),
                                 parameter_types_(std::move(argument_types)),
                                 return_type_(return_type),
                                 function_(std::move(function))
{}

void
ScalarFunction::CastArgumentTypes(std::vector<BaseExpression>& input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type
    auto arguments_count = input_arguments.size();
    PlannerAssert(input_arguments.size() != arguments_count, "Function :" + name_ + " arguments number isn't matched.");
    for(auto idx = 0; idx < arguments_count; ++ idx) {
        if(parameter_types_[idx] != input_arguments[idx].DataType()) {
            PlannerError("Not implemented: need to cast the argument types");
        }
    }
}

void
ScalarFunction::NoOpFunction(const TransientBlock &input, Chunk &output) {
    // TODO: this should be the pointer copy from input to output.
    output.data() = input.chunks_[0].data();
}

std::string
ScalarFunction::ToString() {

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
