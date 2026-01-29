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

module infinity_core:like.impl;

import :like;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

bool LikeOperator(const char *left_ptr, size_t left_len, const char *right_ptr, size_t right_len, char escape_char) {
    size_t left_idx{0}, right_idx{0};

    while (right_idx < right_len) {
        char left_char = (left_idx < left_len) ? left_ptr[left_idx] : '\0';
        char right_char = right_ptr[right_idx];

        // Check if current character is escaped
        if (right_char == escape_char && right_idx + 1 < right_len) {
            char next_char = right_ptr[right_idx + 1];
            if (left_char == next_char) {
                ++left_idx;
                right_idx += 2;
                continue;
            } else {
                return false;
            }
        }

        if (right_char == '_' or (left_char == right_char)) {
            ++left_idx;
            ++right_idx;
        } else if (right_char == '%') {
            ++right_idx;

            // If there are more than one %
            while (right_idx < right_len && right_ptr[right_idx] == '%') {
                ++right_idx;
            }

            // % is the last character
            if (right_idx == right_len) {
                return true;
            }

            // Check any left part is matched with rest of right part.
            while (left_idx < left_len) {
                if (LikeOperator(&left_ptr[left_idx], left_len - left_idx, &right_ptr[right_idx], right_len - right_idx, escape_char)) {
                    return true;
                }
                ++left_idx;
            }

            // Not matched
            return false;
        } else {
            return false;
        }
    }

    while (right_idx < right_len && right_ptr[right_idx] == '%') {
        ++right_idx;
    }

    return left_idx == left_len && right_idx == right_len;
}

template <bool like>
struct LikeFunctionBase {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &right, TC &escape, TD &result) {
        const char *left_str;
        size_t left_len;
        const char *right_str;
        size_t right_len;
        const char *escape_str;
        size_t escape_len;
        GetReaderValue(left, left_str, left_len);
        GetReaderValue(right, right_str, right_len);
        GetReaderValue(escape, escape_str, escape_len);

        // ESCAPE clause is always exactly 1 character
        bool match = LikeOperator(left_str, left_len, right_str, right_len, escape_str[0]);
        if constexpr (like) {
            result.SetValue(!match);
        } else {
            result.SetValue(match);
        }
    }
};

using LikeFunction = LikeFunctionBase<false>;
using NotLikeFunction = LikeFunctionBase<true>;

void RegisterLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_like_function(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                         DataType(LogicalType::kBoolean),
                                         &ScalarFunction::TernaryFunction<VarcharT, VarcharT, VarcharT, BooleanT, LikeFunction>);
    function_set_ptr->AddFunction(varchar_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

void RegisterNotLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "not_like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_not_like_function(func_name,
                                             {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                             DataType(LogicalType::kBoolean),
                                             &ScalarFunction::TernaryFunction<VarcharT, VarcharT, VarcharT, BooleanT, NotLikeFunction>);
    function_set_ptr->AddFunction(varchar_not_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity