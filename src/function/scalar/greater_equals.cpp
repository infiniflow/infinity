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

module greater_equals;

namespace infinity {

struct GreaterEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = left >= right;
    }
};

template <>
inline void GreaterEqualsFunction::Run(VarcharT, VarcharT, bool &) {
    Error<NotImplementException>("Not implement: varchar >= varchar");

//    if (left.IsInlined()) {
//        if (right.IsInlined()) {
//            result = (Memcmp(left.prefix, right.prefix, VarcharT::INLINE_LENGTH) >= 0);
//            return;
//        }
//    } else if (right.IsInlined()) {
//        ;
//    } else {
//        // Both left and right are not inline
//        u16 min_len = Min(right.length, left.length);
//        if (Memcmp(left.prefix, right.prefix, VarcharT::PREFIX_LENGTH) >= 0) {
//            result = (Memcmp(left.ptr, right.ptr, min_len) >= 0);
//            return;
//        }
//    }
//    result = false;
}

template <>
inline void GreaterEqualsFunction::Run(MixedT, BigIntT, bool &) {
    Error<NotImplementException>("Not implement: mixed >= bigint");
}

template <>
inline void GreaterEqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <>
inline void GreaterEqualsFunction::Run(MixedT, DoubleT, bool &) {
    Error<NotImplementException>("Not implement: mixed >= double");
}

template <>
inline void GreaterEqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <>
inline void GreaterEqualsFunction::Run(MixedT, VarcharT, bool &) {
    Error<NotImplementException>("Not implement: mixed >= varchar");
}

template <>
inline void GreaterEqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <typename CompareType>
static void GenerateGreaterEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = ">=";
    ScalarFunction greater_equals_function(func_name,
                                           {data_type, data_type},
                                           {DataType(LogicalType::kBoolean)},
                                           &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(greater_equals_function);
}

void RegisterGreaterEqualsFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = ">=";

    SharedPtr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    GenerateGreaterEqualsFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterEqualsFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterEqualsFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterEqualsFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterEqualsFunction<HugeIntT>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateGreaterEqualsFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterEqualsFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateGreaterEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateGreaterEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateGreaterEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateGreaterEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateGreaterEqualsFunction<VarcharT>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateGreaterEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateGreaterEqualsFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateGreaterEqualsFunction<TimeT>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateGreaterEqualsFunction<DateTimeT>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateGreaterEqualsFunction<TimestampT>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateGreaterEqualsFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));

    //    GenerateGreaterEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_greater_equals_bigint(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                             DataType(kBoolean),
                                             &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(mix_greater_equals_bigint);

    ScalarFunction bigint_greater_equals_mixed(func_name,
                                               {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                               DataType(kBoolean),
                                               &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(bigint_greater_equals_mixed);

    ScalarFunction mix_greater_equals_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             DataType(kBoolean),
                                             &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(mix_greater_equals_double);

    ScalarFunction double_greater_equals_mixed(func_name,
                                               {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                               DataType(kBoolean),
                                               &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(double_greater_equals_mixed);

    ScalarFunction mix_greater_equals_varchar(func_name,
                                              {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                              DataType(kBoolean),
                                              &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(mix_greater_equals_varchar);

    ScalarFunction varchar_greater_equals_mixed(func_name,
                                                {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                                DataType(kBoolean),
                                                &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(varchar_greater_equals_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity