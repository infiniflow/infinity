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

module greater_equals;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import logger;
import third_party;
import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct GreaterEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        static_assert(false, "Unsupported type");
    }
};

struct PODTypeGreaterEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result.SetValue(left >= right);
    }
};

struct ColumnValueReaderTypeGreaterEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        result.SetValue(ThreeWayCompareReaderValue(left, right) != std::strong_ordering::less);
    }
};

template <>
inline void GreaterEqualsFunction::Run(MixedT, BigIntT, bool &) {
    String error_message = "Not implement: GreaterEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void GreaterEqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <>
inline void GreaterEqualsFunction::Run(MixedT, DoubleT, bool &) {
    String error_message = "Not implement: GreaterEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void GreaterEqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <>
inline void GreaterEqualsFunction::Run(MixedT, VarcharT, bool &) {
    String error_message = "Not implement: GreaterEqualsFunction::Run";
    UnrecoverableError(error_message);
}

template <>
inline void GreaterEqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    GreaterEqualsFunction::Run(right, left, result);
}

template <typename CompareType, typename GreaterEqualsFunction>
static void GenerateGreaterEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = ">=";
    ScalarFunction greater_equals_function(func_name,
                                           {data_type, data_type},
                                           {DataType(LogicalType::kBoolean)},
                                           &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(greater_equals_function);
}

void RegisterGreaterEqualsFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = ">=";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateGreaterEqualsFunction<TinyIntT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterEqualsFunction<SmallIntT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterEqualsFunction<IntegerT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterEqualsFunction<BigIntT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterEqualsFunction<HugeIntT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateGreaterEqualsFunction<FloatT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat16));
    GenerateGreaterEqualsFunction<FloatT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kBFloat16));
    GenerateGreaterEqualsFunction<FloatT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterEqualsFunction<DoubleT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateGreaterEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateGreaterEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateGreaterEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateGreaterEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateGreaterEqualsFunction<VarcharT, ColumnValueReaderTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateGreaterEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateGreaterEqualsFunction<DateT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateGreaterEqualsFunction<TimeT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateGreaterEqualsFunction<DateTimeT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateGreaterEqualsFunction<TimestampT, PODTypeGreaterEqualsFunction>(function_set_ptr, DataType(LogicalType::kTimestamp));
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

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
