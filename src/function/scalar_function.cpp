//
// Created by JinHai on 2022/9/14.
//

#include "scalar_function.h"
#include "common/utility/infinity_assert.h"
#include <utility>
#include <sstream>

namespace infinity {

ScalarFunction::ScalarFunction(String name,
                               Vector<DataType> argument_types,
                               DataType return_type,
                               ScalarFunctionType function)
                               : Function(std::move(name), FunctionType::kScalar),
                                 parameter_types_(std::move(argument_types)),
                                 return_type_(std::move(return_type)),
                                 function_(std::move(function))
{}

void
ScalarFunction::CastArgumentTypes(Vector<BaseExpression>& input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type
    auto arguments_count = input_arguments.size();
    PlannerAssert(input_arguments.size() != arguments_count, "Function :" + name_ + " arguments number isn't matched.");
    for(auto idx = 0; idx < arguments_count; ++ idx) {
        if(parameter_types_[idx] != input_arguments[idx].Type()) {
            PlannerError("Not implemented: need to cast the argument types");
        }
    }
}

void
ScalarFunction::NoOpFunction(const DataBlock &input, SharedPtr<ColumnVector>& output) {
    // TODO: this should be the pointer copy from input to output.

    // Fixme: Output reference the data of input
    output->ShallowCopy(*input.column_vectors[0]);
}

String
ScalarFunction::ToString() const {

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
        ss << ")->" << return_type_.ToString();
    }

    return ss.str();
}


}
