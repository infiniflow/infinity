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

module substract;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import logger;
import third_party;
import internal_types;
import data_type;

namespace infinity {

struct SubFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA, TB, TC &) {
        String error_message = "Not implement: SubFunction::Run";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
};

template <>
inline bool SubFunction::Run(TinyIntT left, TinyIntT right, TinyIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template <>
inline bool SubFunction::Run(SmallIntT left, SmallIntT right, SmallIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template <>
inline bool SubFunction::Run(IntegerT left, IntegerT right, IntegerT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template <>
inline bool SubFunction::Run(BigIntT left, BigIntT right, BigIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// HugeIntT - HugeIntT = HugeIntT, and check overflow
template <>
inline bool SubFunction::Run(HugeIntT, HugeIntT, HugeIntT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// FloatT - FloatT = FloatT, and check overflow
template <>
inline bool SubFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = left - right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT - DoubleT = DoubleT, and check overflow
template <>
inline bool SubFunction::Run(DoubleT left, DoubleT right, DoubleT &result) {
    result = left - right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal - Decimal = Decimal
template <>
inline bool SubFunction::Run(DecimalT, DecimalT, DecimalT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// DateT - Interval
template <>
inline bool SubFunction::Run(DateT left, IntervalT right, DateT &result) {
    return DateT::Subtract(left, right, result);
}

// TimeT - Interval
template <>
inline bool SubFunction::Run(TimeT left, IntervalT right, TimeT &result) {
    return TimeT::Subtract(left, right, result);
}

// DateTime - Interval
template <>
inline bool SubFunction::Run(DateTimeT left, IntervalT right, DateTimeT &result) {
    return DateTimeT::Subtract(left, right, result);
}

// TimestampT - Interval
template <>
inline bool SubFunction::Run(TimestampT left, IntervalT right, TimestampT &result) {
    return TimestampT::Subtract(left, right, result);
}

// Mixed Type - i64
template <>
inline bool SubFunction::Run(MixedT, BigIntT, MixedT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// i64 - Mixed Type
template <>
inline bool SubFunction::Run(BigIntT, MixedT, MixedT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// Mixed Type - f64
template <>
inline bool SubFunction::Run(MixedT, DoubleT, MixedT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// f64 - Mixed Type
template <>
inline bool SubFunction::Run(DoubleT, MixedT, MixedT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

// Mixed Type - Mixed Type
template <>
inline bool SubFunction::Run(MixedT, MixedT, MixedT &) {
    String error_message = "Not implement: SubFunction::Run";
    LOG_CRITICAL(error_message);
    UnrecoverableError(error_message);
    return false;
}

void RegisterSubtractFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "-";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    //    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction sub_function_int8(func_name,
                                     {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                     {DataType(LogicalType::kTinyInt)},
                                     &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int8);

    ScalarFunction sub_function_int16(func_name,
                                      {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                      {DataType(LogicalType::kSmallInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int16);

    ScalarFunction sub_function_int32(func_name,
                                      {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                      {DataType(LogicalType::kInteger)},
                                      &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int32);

    ScalarFunction sub_function_int64(func_name,
                                      {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                      {DataType(LogicalType::kBigInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int64);

    ScalarFunction sub_function_int128(func_name,
                                       {DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt)},
                                       {DataType(LogicalType::kHugeInt)},
                                       &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int128);

    ScalarFunction sub_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_float);

    ScalarFunction sub_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<double, double, double, SubFunction>);
    function_set_ptr->AddFunction(sub_function_double);

    ScalarFunction sub_function_decimal(func_name,
                                        {DataType(LogicalType::kDecimal), DataType(LogicalType::kDecimal)},
                                        {DataType(LogicalType::kDecimal)},
                                        &ScalarFunction::BinaryFunctionWithFailure<DecimalT, DecimalT, DecimalT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_decimal);

    ScalarFunction sub_function_date_interval(func_name,
                                              {DataType(LogicalType::kDate), DataType(LogicalType::kInterval)},
                                              {DataType(LogicalType::kDate)},
                                              &ScalarFunction::BinaryFunctionWithFailure<DateT, IntervalT, DateT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_date_interval);

    ScalarFunction sub_function_time_interval(func_name,
                                              {DataType(LogicalType::kTime), DataType(LogicalType::kInterval)},
                                              {DataType(LogicalType::kTime)},
                                              &ScalarFunction::BinaryFunctionWithFailure<TimeT, IntervalT, TimeT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_time_interval);

    ScalarFunction sub_function_datetime_interval(func_name,
                                                  {DataType(LogicalType::kDateTime), DataType(LogicalType::kInterval)},
                                                  {DataType(LogicalType::kDateTime)},
                                                  &ScalarFunction::BinaryFunctionWithFailure<DateTimeT, IntervalT, DateTimeT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_datetime_interval);

    ScalarFunction sub_function_timestamp_interval(func_name,
                                                   {DataType(LogicalType::kTimestamp), DataType(LogicalType::kInterval)},
                                                   {DataType(LogicalType::kTimestamp)},
                                                   &ScalarFunction::BinaryFunctionWithFailure<TimestampT, IntervalT, TimestampT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_timestamp_interval);

    ScalarFunction sub_function_mixed_bigint(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_bigint);

    ScalarFunction sub_function_bigint_mixed(func_name,
                                             {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_bigint_mixed);

    ScalarFunction sub_function_mixed_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_double);

    ScalarFunction sub_function_double_mixed(func_name,
                                             {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_double_mixed);

    ScalarFunction sub_function_mixed_mixed(func_name,
                                            {DataType(LogicalType::kMixed), DataType(LogicalType::kMixed)},
                                            {DataType(LogicalType::kMixed)},
                                            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_mixed);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity