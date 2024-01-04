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
import catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module less;

namespace infinity {

struct LessFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = left < right;
    }
};

template <>
inline void LessFunction::Run(VarcharT, VarcharT, bool &) {
    Error<NotImplementException>("Not implement: varchar < varchar");
//    if (left.IsInlined()) {
//        if (right.IsInlined()) {
//            result = (Memcmp(left.prefix, right.prefix, VarcharT::INLINE_LENGTH) < 0);
//            return;
//        }
//    } else if (right.IsInlined()) {
//        ;
//    } else {
//        // Both left and right are not inline
//        u16 min_len = Min(right.length, left.length);
//        if (Memcmp(left.prefix, right.prefix, VarcharT::PREFIX_LENGTH) < 0) {
//            result = (Memcmp(left.ptr, right.ptr, min_len) < 0);
//            return;
//        }
//    }
//    result = false;
}

template <>
inline void LessFunction::Run(MixedT, BigIntT, bool &) {
    Error<NotImplementException>("Not implement: mixed < bigint");
}

template <>
inline void LessFunction::Run(BigIntT left, MixedT right, bool &result) {
    LessFunction::Run(right, left, result);
}

template <>
inline void LessFunction::Run(MixedT, DoubleT, bool &) {
    Error<NotImplementException>("Not implement: mixed < double");
}

template <>
inline void LessFunction::Run(DoubleT left, MixedT right, bool &result) {
    LessFunction::Run(right, left, result);
}

template <>
inline void LessFunction::Run(MixedT, VarcharT, bool &) {
    Error<NotImplementException>("Not implement: mixed < varchar");
}

template <>
inline void LessFunction::Run(VarcharT left, MixedT right, bool &result) {
    LessFunction::Run(right, left, result);
}

template <typename CompareType>
static void GenerateLessFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = "<";

    ScalarFunction less_function(func_name,
                                 {data_type, data_type},
                                 {DataType(LogicalType::kBoolean)},
                                 &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(less_function);
}

void RegisterLessFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "<";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateLessFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateLessFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateLessFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateLessFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateLessFunction<HugeIntT>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateLessFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateLessFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateLessFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateLessFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateLessFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateLessFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateLessFunction<VarcharT>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateLessFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateLessFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateLessFunction<TimeT>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateLessFunction<DateTimeT>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateLessFunction<TimestampT>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateLessFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));

    //    GenerateEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_less_bigint(func_name,
                                   {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                   DataType(kBoolean),
                                   &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(mix_less_bigint);

    ScalarFunction bigint_less_mixed(func_name,
                                     {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(bigint_less_mixed);

    ScalarFunction mix_less_double(func_name,
                                   {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                   DataType(kBoolean),
                                   &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(mix_less_double);

    ScalarFunction double_less_mixed(func_name,
                                     {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(double_less_mixed);

    ScalarFunction mix_less_varchar(func_name,
                                    {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                    DataType(kBoolean),
                                    &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(mix_less_varchar);

    ScalarFunction varchar_less_mixed(func_name,
                                      {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                      DataType(kBoolean),
                                      &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(varchar_less_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity