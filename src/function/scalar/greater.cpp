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

module greater;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct GreaterFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        static_assert(false, "Unsupported type");
    }
};

struct PODTypeGreaterFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result.SetValue(left > right);
    }
};

struct ColumnValueReaderTypeGreaterFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        result.SetValue(ThreeWayCompareReaderValue(left, right) == std::strong_ordering::greater);
    }
};

template <>
inline void GreaterFunction::Run(MixedT, BigIntT, bool &) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

template <>
inline void GreaterFunction::Run(BigIntT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <>
inline void GreaterFunction::Run(MixedT, DoubleT, bool &) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

template <>
inline void GreaterFunction::Run(DoubleT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <>
inline void GreaterFunction::Run(MixedT, VarcharT, bool &) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

template <>
inline void GreaterFunction::Run(VarcharT left, MixedT right, bool &result) {
    GreaterFunction::Run(right, left, result);
}

template <typename CompareType, typename GreaterFunction>
static void GenerateGreaterFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = ">";
    ScalarFunction greater_function(func_name,
                                    {data_type, data_type},
                                    DataType(LogicalType::kBoolean),
                                    &ScalarFunction::BinaryFunction<CompareType, CompareType, bool, GreaterFunction>);
    function_set_ptr->AddFunction(greater_function);
}

void RegisterGreaterFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = ">";
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateGreaterFunction<TinyIntT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterFunction<SmallIntT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterFunction<IntegerT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterFunction<BigIntT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterFunction<HugeIntT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateGreaterFunction<FloatT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kFloat16));
    GenerateGreaterFunction<FloatT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kBFloat16));
    GenerateGreaterFunction<FloatT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterFunction<DoubleT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateGreaterFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateGreaterFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateGreaterFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateGreaterFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateGreaterFunction<VarcharT, ColumnValueReaderTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateGreaterFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateGreaterFunction<DateT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateGreaterFunction<TimeT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateGreaterFunction<DateTimeT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateGreaterFunction<TimestampT, PODTypeGreaterFunction>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateGreaterFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));

    //    GenerateEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_greater_bigint(func_name,
                                      {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                      DataType(LogicalType::kBoolean),
                                      &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_bigint);

    ScalarFunction bigint_greater_mixed(func_name,
                                        {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                        DataType(LogicalType::kBoolean),
                                        &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(bigint_greater_mixed);

    ScalarFunction mix_greater_double(func_name,
                                      {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                      DataType(LogicalType::kBoolean),
                                      &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_double);

    ScalarFunction double_greater_mixed(func_name,
                                        {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                        DataType(LogicalType::kBoolean),
                                        &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(double_greater_mixed);

    ScalarFunction mix_greater_varchar(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                       DataType(LogicalType::kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(mix_greater_varchar);

    ScalarFunction varchar_greater_mixed(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                         DataType(LogicalType::kBoolean),
                                         &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, GreaterFunction>);
    function_set_ptr->AddFunction(varchar_greater_mixed);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity