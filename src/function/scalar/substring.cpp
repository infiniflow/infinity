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

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;
import column_vector;

module substring;

namespace infinity {

struct SubstrFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline bool Run(TA, TB, TC, TD &, ColumnVector *) {
        Error<NotImplementException>("Not implement");
    }
};

template <>
inline bool SubstrFunction::Run(VarcharT, BigIntT, BigIntT, VarcharT &, ColumnVector *) {
    // Validate the input before slice the string
    Error<NotImplementException>("Not implement");

//    if (second < 0) {
//        Error<ExecutorException>(Format("substring start offset should >= 0, currently it is {}", second));
//    }
//
//    if (third < 0) {
//        Error<ExecutorException>(Format("substring length should >= 0, currently it is {}", second));
//    }
//
//    if (third == 0) {
//        // Construct empty varchar value;
//        result.InitializeAsEmptyStr();
//        return true;
//    }
//
//    SizeT source_len = first.GetDataLen();
//    if (second >= source_len) {
//        // Construct empty varchar value;
//        result.InitializeAsEmptyStr();
//        return true;
//    }
//
//    SizeT start_offset = second;
//    SizeT end_offset = 0;
//    if (start_offset + third >= source_len) {
//        end_offset = source_len;
//    } else {
//        end_offset = start_offset + third;
//    }
//
//    SizeT copied_length = end_offset - start_offset;
//    ptr_t source_ptr = first.GetDataPtr();
//    if (copied_length <= VarcharT::INLINE_LENGTH) {
//        // inline varchar
//        Memcpy(result.prefix, source_ptr + start_offset, copied_length);
//        result.length = copied_length;
//    } else {
//        Memcpy(result.prefix, source_ptr + start_offset, VarcharT::INLINE_LENGTH);
//        result.ptr = column_vector_ptr->buffer_->fix_heap_mgr_->Allocate(copied_length);
//        Memcpy(result.ptr, source_ptr + start_offset, copied_length);
//    }

    return true;
}

void RegisterSubstringFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "substring";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_substr(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                  {DataType(LogicalType::kVarchar)},
                                  &ScalarFunction::TernaryFunctionToVarlenWithFailure<VarcharT, BigIntT, BigIntT, VarcharT, SubstrFunction>);
    function_set_ptr->AddFunction(varchar_substr);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity