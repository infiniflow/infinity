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

#include <compare>

module less_equals;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct LessEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        static_assert(false, "Unsupported type");
    }
};

struct PODTypeLessEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result.SetValue(left <= right);
    }
};

struct ColumnValueReaderTypeLessEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        result.SetValue(ThreeWayCompareReaderValue(left, right) != std::strong_ordering::greater);
    }
};

template <>
inline void LessEqualsFunction::Run(MixedT, BigIntT, bool &) {
    String error_message = "Not implement: LessEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void LessEqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    LessEqualsFunction::Run(right, left, result);
}

template <>
inline void LessEqualsFunction::Run(MixedT, DoubleT, bool &) {
    String error_message = "Not implement: LessEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void LessEqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    LessEqualsFunction::Run(right, left, result);
}

template <>
inline void LessEqualsFunction::Run(MixedT, VarcharT, bool &) {
    String error_message = "Not implement: LessEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void LessEqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    LessEqualsFunction::Run(right, left, result);
}

template <typename CompareType, typename LessEqualsFunction>
static void GenerateLessEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = "<=";
    ScalarFunction less_function(func_name,
                                 {data_type, data_type},
                                 {DataType(LogicalType::kBoolean)},
                                 &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(less_function);
}

void RegisterLessEqualsFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "<=";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateLessEqualsFunction<TinyIntT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateLessEqualsFunction<SmallIntT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateLessEqualsFunction<IntegerT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateLessEqualsFunction<BigIntT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateLessEqualsFunction<HugeIntT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateLessEqualsFunction<FloatT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat16));
    GenerateLessEqualsFunction<FloatT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kBFloat16));
    GenerateLessEqualsFunction<FloatT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateLessEqualsFunction<DoubleT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateLessEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateLessEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateLessEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateLessEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateLessEqualsFunction<VarcharT, ColumnValueReaderTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateLessEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateLessEqualsFunction<DateT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateLessEqualsFunction<TimeT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateLessEqualsFunction<DateTimeT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateLessEqualsFunction<TimestampT, PODTypeLessEqualsFunction>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateLessEqualsFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));

    //    GenerateEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_less_equals_bigint(func_name,
                                          {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                          DataType(LogicalType::kBoolean),
                                          &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(mix_less_equals_bigint);

    ScalarFunction bigint_less_equals_mixed(func_name,
                                            {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                            DataType(LogicalType::kBoolean),
                                            &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(bigint_less_equals_mixed);

    ScalarFunction mix_less_equals_double(func_name,
                                          {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                          DataType(LogicalType::kBoolean),
                                          &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(mix_less_equals_double);

    ScalarFunction double_less_equals_mixed(func_name,
                                            {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                            DataType(LogicalType::kBoolean),
                                            &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(double_less_equals_mixed);

    ScalarFunction mix_less_equals_varchar(func_name,
                                           {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                           DataType(LogicalType::kBoolean),
                                           &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(mix_less_equals_varchar);

    ScalarFunction varchar_less_equals_mixed(func_name,
                                             {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                             DataType(LogicalType::kBoolean),
                                             &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(varchar_less_equals_mixed);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity