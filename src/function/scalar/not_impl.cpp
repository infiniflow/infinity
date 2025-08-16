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

module infinity_core:not_func.impl;

import :not_func;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

struct NotFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        if constexpr (std::is_same_v<std::remove_cv_t<TA>, u8> && std::is_same_v<std::remove_cv_t<TB>, u8>) {
            result = ~input;
        } else if constexpr (std::is_same_v<std::remove_cv_t<TA>, BooleanT> && std::is_same_v<std::remove_cv_t<TB>, BooleanT>) {
            result = !input;
        } else {
            UnrecoverableError("NOT function accepts only u8 and BooleanT.");
        }
    }
};

static void GenerateNotFunction(std::shared_ptr<ScalarFunctionSet> &function_set_ptr) {
    std::string func_name = "NOT";

    ScalarFunction not_function(func_name,
                                {DataType(LogicalType::kBoolean)},
                                {DataType(LogicalType::kBoolean)},
                                &ScalarFunction::UnaryFunction<BooleanT, BooleanT, NotFunction>);
    function_set_ptr->AddFunction(not_function);
}

void RegisterNotFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "NOT";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    GenerateNotFunction(function_set_ptr);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity