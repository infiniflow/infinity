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
#include <cstdio>
module trunc;
import stl;
import catalog;
import status;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;

namespace infinity {

struct TruncFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA left, TB right, TC &result, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }

};

template <>
inline void TruncFunction::Run(DoubleT left, BigIntT right, VarcharT &result, ColumnVector *result_ptr) {
    constexpr int MaxRight = 17;
    constexpr int MinBufferSize = 50;
    
    if (right < static_cast<BigIntT>(0) || std::isnan(right) || std::isinf(right)) {
        Status status = Status::InvalidDataType();
        RecoverableError(status);
        return;
    }
    
    char buffer[MinBufferSize];  
    buffer[0] =' ';
    
    right = (right > MaxRight) ? MaxRight : right;  
    
    int len = std::snprintf(buffer + 1, sizeof(buffer) - 2, "%.*f", (int)right, left);
    if (len < 0) {
        Status status = Status::InvalidDataType();
        RecoverableError(status);
        return;
    }
    std::string truncated_str(buffer, len + 1);
    result_ptr->AppendVarcharInner(truncated_str, result);

}

template <>
inline void TruncFunction::Run(FloatT left, BigIntT right, VarcharT &result, ColumnVector *result_ptr) {
    constexpr int MaxRight = 7;
    constexpr int MinBufferSize = 20;
    
    if (right < static_cast<BigIntT>(0) || std::isnan(right) || std::isinf(right)) {
        Status status = Status::InvalidDataType();
        RecoverableError(status);
        return;
    } 
    char buffer[MinBufferSize]; 
    buffer[0] =' '; 
    right = (right >  MaxRight) ?  MaxRight : right;
    int len = std::snprintf(buffer + 1, sizeof(buffer) - 2, "%.*f", (int)right, left);
    if (len < 0) {
        Status status = Status::InvalidDataType();
        RecoverableError(status);
        return;
    }
    std::string truncated_str(buffer, len + 1);
    result_ptr->AppendVarcharInner(truncated_str, result);
}


void RegisterTruncFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "trunc";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction truncate_double_bigint(func_name,
                              {DataType(LogicalType::kDouble), DataType(LogicalType::kBigInt)},
                              DataType(LogicalType::kVarchar),
                              &ScalarFunction::BinaryFunctionToVarlen<DoubleT, BigIntT, VarcharT, TruncFunction>);
    function_set_ptr->AddFunction(truncate_double_bigint);

    ScalarFunction truncate_float_bigint(func_name,
                              {DataType(LogicalType::kFloat), DataType(LogicalType::kBigInt)},
                              DataType(LogicalType::kVarchar),
                              &ScalarFunction::BinaryFunctionToVarlen<FloatT, BigIntT, VarcharT, TruncFunction>);
    function_set_ptr->AddFunction(truncate_float_bigint);


    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity