// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
module infinity_core:reverse.impl;

import :reverse;
import :stl;
import :new_catalog;
import :status;
import logical_type;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :third_party;
import internal_types;
import data_type;
import :column_vector;

namespace infinity {

struct ReverseFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void ReverseFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    const char *input = left_v.data();
    SizeT input_len = left_v.size();
    String reversed_str(input, input_len);
    std::reverse(reversed_str.begin(), reversed_str.end());
    result_ptr->AppendVarcharInner(reversed_str, result);
}

void RegisterReverseFunction(NewCatalog *catalog_ptr) {
    String func_name = "reverse";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction resverse_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, ReverseFunction>);
    function_set_ptr->AddFunction(resverse_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity