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

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;
import logger;

module pow;

namespace infinity {

struct PowFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA base, TB exponent, TC &result) {
        result = std::pow(base, exponent);
        return true;
    }
};

template <>
inline bool PowFunction::Run(MixedT, DoubleT, DoubleT &) {
    String error_message = "Not implement: PowFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

void RegisterPowFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "POW";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction pow_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_float);

    ScalarFunction pow_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_double);

    ScalarFunction pow_function_mixed_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             {DataType(LogicalType::kDouble)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_mixed_double);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
