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
import new_catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module like;

namespace infinity {

bool LikeOperator(const ptr_t left_ptr, SizeT left_len, const ptr_t right_ptr, SizeT right_len) {
    SizeT left_idx{0}, right_idx{0};

    while (right_idx < right_len) {
        char left_char = left_ptr[left_idx];
        char right_char = right_ptr[right_idx];
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
                if (LikeOperator(&left_ptr[left_idx], left_len - left_idx, &right_ptr[right_idx], right_len - right_idx)) {
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

struct LikeFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        Error<NotImplementException>("Not implement");
    }
};

template <>
inline void LikeFunction::Run(VarcharT &left, VarcharT &right, bool &result) {
    ptr_t left_ptr = left.GetDataPtr();
    SizeT left_len = left.GetDataLen();
    ptr_t right_ptr = right.GetDataPtr();
    SizeT right_len = right.GetDataLen();

    result = LikeOperator(left_ptr, left_len, right_ptr, right_len);
}

struct NotLikeFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        Error<NotImplementException>("Not implement");
    }
};

template <>
inline void NotLikeFunction::Run(VarcharT &left, VarcharT &right, bool &result) {
    ptr_t left_ptr = left.GetDataPtr();
    SizeT left_len = left.GetDataLen();
    ptr_t right_ptr = right.GetDataPtr();
    SizeT right_len = right.GetDataLen();

    result = !LikeOperator(left_ptr, left_len, right_ptr, right_len);
}

void RegisterLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "like";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_like_function(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, LikeFunction>);
    function_set_ptr->AddFunction(varchar_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

void RegisterNotLikeFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "not_like";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_not_like_function(func_name,
                                             {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                             DataType(kBoolean),
                                             &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, NotLikeFunction>);
    function_set_ptr->AddFunction(varchar_not_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity