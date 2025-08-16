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

module infinity_core:or_func.impl;

import :or_func;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :status;

import std;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct OrFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        if constexpr (std::is_same_v<std::remove_cv_t<TA>, u8> && std::is_same_v<std::remove_cv_t<TB>, u8> &&
                      std::is_same_v<std::remove_cv_t<TC>, u8>) {
            result = left | right;
        } else if constexpr (std::is_same_v<std::remove_cv_t<TA>, BooleanT> && std::is_same_v<std::remove_cv_t<TB>, BooleanT> &&
                             std::is_same_v<std::remove_cv_t<TC>, BooleanT>) {
            result = left or right;
        } else {
            UnrecoverableError("OR function accepts only u8 and BooleanT.");
        }
    }
};

static void GenerateOrFunction(std::shared_ptr<ScalarFunctionSet> &function_set_ptr) {
    std::string func_name = "OR";
    ScalarFunction or_function(func_name,
                               {DataType(LogicalType::kBoolean), DataType(LogicalType::kBoolean)},
                               {DataType(LogicalType::kBoolean)},
                               &ScalarFunction::BinaryFunction<BooleanT, BooleanT, BooleanT, OrFunction>);
    function_set_ptr->AddFunction(or_function);
}

void RegisterOrFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "OR";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    GenerateOrFunction(function_set_ptr);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity