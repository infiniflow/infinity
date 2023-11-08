// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
module;

#include <sstream>
import stl;
import function;
import parser;

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
    Assert<PlannerException>(input_arguments.size() != arguments_count, Format("Function: {} arguments number isn't matched.", name_));
    for (auto idx = 0; idx < arguments_count; ++idx) {
        if (parameter_types_[idx] != input_arguments[idx].Type()) {
            Error<PlannerException>("Not implemented: need to cast the argument types");
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
