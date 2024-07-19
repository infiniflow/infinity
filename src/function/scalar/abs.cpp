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

#include <numeric>

module abs;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;

namespace infinity {

struct AbsFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        if (value == std::numeric_limits<SourceType>::min()) {
            return false;
        }
        result = value < 0 ? -value : value;
        return true;
    }
};

struct AbsFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline void Run(SourceType value, TargetType &result) {
        result = value < static_cast<SourceType>(0.0f) ? -value : value;
    }
};

void RegisterAbsFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "ABS";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction abs_int8(func_name,
                            {DataType(LogicalType::kTinyInt)},
                            DataType(LogicalType::kTinyInt),
                            &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, TinyIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int8);

    ScalarFunction abs_int16(func_name,
                             {DataType(LogicalType::kSmallInt)},
                             {DataType(LogicalType::kSmallInt)},
                             &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, SmallIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int16);

    ScalarFunction abs_int32(func_name,
                             {DataType(LogicalType::kInteger)},
                             {DataType(LogicalType::kInteger)},
                             &ScalarFunction::UnaryFunctionWithFailure<IntegerT, IntegerT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int32);

    ScalarFunction abs_int64(func_name,
                             {DataType(LogicalType::kBigInt)},
                             {DataType(LogicalType::kBigInt)},
                             &ScalarFunction::UnaryFunctionWithFailure<BigIntT, BigIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int64);

    ScalarFunction abs_float(func_name,
                             {DataType(LogicalType::kFloat)},
                             {DataType(LogicalType::kFloat)},
                             &ScalarFunction::UnaryFunction<FloatT, FloatT, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_float);

    ScalarFunction abs_double(func_name,
                              {DataType(LogicalType::kDouble)},
                              {DataType(LogicalType::kDouble)},
                              &ScalarFunction::UnaryFunction<DoubleT, DoubleT, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_double);

    ScalarFunction abs_float16(func_name,
                               {DataType(LogicalType::kFloat16)},
                               {DataType(LogicalType::kFloat16)},
                               &ScalarFunction::UnaryFunction<Float16T, Float16T, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_float16);

    ScalarFunction abs_bfloat16(func_name,
                                {DataType(LogicalType::kBFloat16)},
                                {DataType(LogicalType::kBFloat16)},
                                &ScalarFunction::UnaryFunction<BFloat16T, BFloat16T, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity