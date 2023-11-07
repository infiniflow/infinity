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

#include <cmath>

import stl;
import new_catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module greater;

namespace infinity {

struct GreaterFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = left > right;
    }
};

template <>
inline void GreaterFunction::Run(VarcharT left, VarcharT right, bool &result) {
    if (left.IsInlined()) {
        if (right.IsInlined()) {
            result = (Memcmp(left.prefix, right.prefix, VarcharT::INLINE_LENGTH) > 0);
            return;
        }
    } else if (right.IsInlined()) {
        ;
    } else {
        // Both left and right are not inline
        u16 min_len = Min(right.length, left.length);
        if (Memcmp(left.prefix, right.prefix, VarcharT::PREFIX_LENGTH) > 0) {
            result = (Memcmp(left.ptr, right.ptr, min_len) > 0);
            return;
        }
    }
    result = false;
}

template <>
inline void GreaterFunction::Run(MixedT left, BigIntT right, bool &result) {
    Error<NotImplementException>("Not implement: mixed > bigint");
}

template <>
inline void GreaterFunction::Run(BigIntT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <>
inline void GreaterFunction::Run(MixedT left, DoubleT right, bool &result) {
    Error<NotImplementException>("Not implement: mixed > double");
}

template <>
inline void GreaterFunction::Run(DoubleT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <>
inline void GreaterFunction::Run(MixedT left, VarcharT right, bool &result) {
    Error<NotImplementException>("Not implement: mixed > varchar");
}

template <>
inline void GreaterFunction::Run(VarcharT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <typename CompareType>
static void GenerateGreaterFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = ">";
    ScalarFunction greater_function(func_name,
                                    {data_type, data_type},
                                    DataType(LogicalType::kBoolean),
                                    &ScalarFunction::BinaryFunction<CompareType, CompareType, bool, GreaterFunction>);
    function_set_ptr->AddFunction(greater_function);
}

void RegisterGreaterFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = ">";
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateGreaterFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterFunction<HugeIntT>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateGreaterFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateGreaterFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateGreaterFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateGreaterFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateGreaterFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateGreaterFunction<VarcharT>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateGreaterFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateGreaterFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
    //    GenerateGreaterFunction<TimeT>(function_set_ptr, DataType(LogicalType::kTime));
    //    GenerateGreaterFunction<DateTimeT>(function_set_ptr, DataType(LogicalType::kDateTime));
    //    GenerateGreaterFunction<TimestampT>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateGreaterFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));

    //    GenerateEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_greater_bigint(func_name,
                                      {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                      DataType(kBoolean),
                                      &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_bigint);

    ScalarFunction bigint_greater_mixed(func_name,
                                        {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                        DataType(kBoolean),
                                        &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(bigint_greater_mixed);

    ScalarFunction mix_greater_double(func_name,
                                      {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                      DataType(kBoolean),
                                      &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_double);

    ScalarFunction double_greater_mixed(func_name,
                                        {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                        DataType(kBoolean),
                                        &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(double_greater_mixed);

    ScalarFunction mix_greater_varchar(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_varchar);

    ScalarFunction varchar_greater_mixed(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(varchar_greater_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity