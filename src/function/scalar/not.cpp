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

#include <type_traits>

module not_func;

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

namespace infinity {

struct NotFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        if constexpr (std::is_same_v<std::remove_cv_t<TA>, u8> && std::is_same_v<std::remove_cv_t<TB>, u8>) {
            result = ~input;
        } else if constexpr (std::is_same_v<std::remove_cv_t<TA>, BooleanT> && std::is_same_v<std::remove_cv_t<TB>, BooleanT>) {
            result = !input;
        } else {
            String error_message = "NOT function accepts only u8 and BooleanT.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
};

static void GenerateNotFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr) {
    String func_name = "NOT";

    ScalarFunction not_function(func_name,
                                {DataType(LogicalType::kBoolean)},
                                {DataType(LogicalType::kBoolean)},
                                &ScalarFunction::UnaryFunction<BooleanT, BooleanT, NotFunction>);
    function_set_ptr->AddFunction(not_function);
}

void RegisterNotFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "NOT";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateNotFunction(function_set_ptr);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity