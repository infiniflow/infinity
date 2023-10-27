//
// Created by JinHai on 2022/9/14.
//
module;

#include <sstream>

import stl;
import function;
import parser;
import infinity_assert;
import infinity_exception;
import data_block;
import base_expression;
import column_vector;
import third_party;

module scalar_function;

namespace infinity {

ScalarFunction::ScalarFunction(String name, Vector<DataType> argument_types, DataType return_type, ScalarFunctionType function)
    : Function(Move(name), FunctionType::kScalar), parameter_types_(Move(argument_types)), return_type_(Move(return_type)),
      function_(Move(function)) {}

void ScalarFunction::CastArgumentTypes(Vector<BaseExpression> &input_arguments) {
    // Check and add a cast function to cast the input arguments expression type to target type
    auto arguments_count = input_arguments.size();
    Assert<PlannerException>(input_arguments.size() != arguments_count,
                             Format("Function: {} arguments number isn't matched.", name_),
                             __FILE_NAME__,
                             __LINE__);
    for (auto idx = 0; idx < arguments_count; ++idx) {
        if (parameter_types_[idx] != input_arguments[idx].Type()) {
            Error<PlannerException>("Not implemented: need to cast the argument types", __FILE_NAME__, __LINE__);
        }
    }
}

void ScalarFunction::NoOpFunction(const DataBlock &input, SharedPtr<ColumnVector> &output) {
    // TODO: this should be the pointer copy from input to output.

    // Fixme: Output reference the data of input
    output->ShallowCopy(*input.column_vectors[0]);
}

String ScalarFunction::ToString() const {

    std::stringstream ss;
    ss << name_;
    auto parameter_count = parameter_types_.size();
    if (parameter_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for (auto i = 0; i < parameter_count - 1; ++i) {
            ss << parameter_types_[i].ToString() << ", ";
        }
        ss << parameter_types_.back().ToString();
        ss << ")->" << return_type_.ToString();
    }

    return ss.str();
}

} // namespace infinity
