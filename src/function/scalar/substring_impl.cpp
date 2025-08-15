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

module infinity_core:substring.impl;

import :substring;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :column_vector;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

struct SubstrFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline bool Run(TA &first, TB &second, TC &third, TD &result, ColumnVector *first_ptr, ColumnVector *result_ptr) {
        std::string error_message = "Not implement: SubstrFunction::Run";
        UnrecoverableError(error_message);
    }
};

template <>
inline bool
SubstrFunction::Run(VarcharT &first, BigIntT &second, BigIntT &third, VarcharT &result, ColumnVector *first_ptr, ColumnVector *result_ptr) {
    if (second < 0) {
        UnrecoverableError(fmt::format("substring start offset should >= 0, currently it is {}", second));
    }

    if (third < 0) {
        UnrecoverableError(fmt::format("substring length should >= 0, currently it is {}", second));
    }

    std::span<const char> first_v = first_ptr->GetVarcharInner(first);
    if (third == 0) {
        // Construct empty varchar value;
        std::span<const char> substr_span = std::span<const char>(first_v.data(), 0);
        result_ptr->AppendVarcharInner(substr_span, result);
        return true;
    }

    size_t source_len = first_v.size();
    if ((size_t)second >= source_len) {
        // Construct empty varchar value;
        std::span<const char> substr_span = std::span<const char>(first_v.data(), 0);
        ;
        result_ptr->AppendVarcharInner(substr_span, result);
        return true;
    }

    size_t start_offset = second;
    size_t end_offset = 0;
    if (start_offset + third >= source_len) {
        end_offset = source_len;
    } else {
        end_offset = start_offset + third;
    }

    std::span<const char> substr_span = std::span<const char>(first_v.data() + start_offset, end_offset - start_offset);
    result_ptr->AppendVarcharInner(substr_span, result);

    return true;
}

void RegisterSubstringFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "substring";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_substr(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                  {DataType(LogicalType::kVarchar)},
                                  &ScalarFunction::TernaryFunctionVarlenToVarlenWithFailure<VarcharT, BigIntT, BigIntT, VarcharT, SubstrFunction>);
    function_set_ptr->AddFunction(varchar_substr);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity