//
// Created by JinHai on 2022/9/28.
//

module;

#include <cmath>

import stl;
import new_catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module add;

namespace infinity {

struct AddFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA left, TB right, TC &result) {
        Error<NotImplementException>("Not implemented");
    }
};

// TinyIntT + TinyIntT = TinyIntT, and check overflow
template <>
inline bool AddFunction::Run(TinyIntT left, TinyIntT right, TinyIntT &result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// SmallIntT + SmallIntT = SmallIntT, and check overflow
template <>
inline bool AddFunction::Run(SmallIntT left, SmallIntT right, SmallIntT &result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// IntegerT + IntegerT = IntegerT, and check overflow
template <>
inline bool AddFunction::Run(IntegerT left, IntegerT right, IntegerT &result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// BigIntT + BigIntT = BigIntT, and check overflow
template <>
inline bool AddFunction::Run(BigIntT left, BigIntT right, BigIntT &result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// HugeIntT + HugeIntT = HugeIntT, and check overflow
template <>
inline bool AddFunction::Run(HugeIntT left, HugeIntT right, HugeIntT &result) {
    Error<NotImplementException>("Not implemented: HugeIntT + HugeIntT = HugeIntT");
}

// FloatT + FloatT = FloatT, and check overflow
template <>
inline bool AddFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = left + right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT + DoubleT = DoubleT, and check overflow
template <>
inline bool AddFunction::Run(DoubleT left, DoubleT right, DoubleT &result) {
    result = left + right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal + Decimal = Decimal
template <>
inline bool AddFunction::Run(DecimalT left, DecimalT right, DecimalT &result) {
    Error<NotImplementException>("Not implemented: Decimal + Decimal");
}

// Date + Interval
template <>
inline bool AddFunction::Run(DateT left, IntervalT right, DateT &result) {
    return DateT::Add(left, right, result);
}

// Interval + Date
template <>
inline bool AddFunction::Run(IntervalT left, DateT right, DateT &result) {
    return DateT::Add(right, left, result);
}

// DateTime + Interval
template <>
inline bool AddFunction::Run(DateTimeT left, IntervalT right, DateTimeT &result) {
    Error<NotImplementException>("Not implemented: DateTimeT + IntervalT");
}

// Interval + DateTime
template <>
inline bool AddFunction::Run(IntervalT left, DateTimeT right, DateTimeT &result) {
    return AddFunction::Run(right, left, result);
}

// TimestampT + Interval
template <>
inline bool AddFunction::Run(TimestampT left, IntervalT right, TimestampT &result) {
    Error<NotImplementException>("Not implemented: TimestampT + IntervalT");
}

// Interval + TimestampT
template <>
inline bool AddFunction::Run(IntervalT left, TimestampT right, TimestampT &result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + i64
template <>
inline bool AddFunction::Run(MixedT left, BigIntT right, MixedT &result) {
    Error<NotImplementException>("Not implemented: MixedT + BigIntT");
}

// i64 + Mixed Type
template <>
inline bool AddFunction::Run(BigIntT left, MixedT right, MixedT &result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + f64
template <>
inline bool AddFunction::Run(MixedT left, DoubleT right, MixedT &result) {
    Error<NotImplementException>("Not implemented: MixedT + DoubleT");
}

// f64 + Mixed Type
template <>
inline bool AddFunction::Run(DoubleT left, MixedT right, MixedT &result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + Mixed Type
template <>
inline bool AddFunction::Run(MixedT left, MixedT right, MixedT &result) {
    Error<NotImplementException>("Not implemented: MixedT + MixedT");
}

void RegisterAddFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "+";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    // Add function
    ScalarFunction add_function_int8(func_name,
                                     {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                     {DataType(LogicalType::kTinyInt)},
                                     &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int8);

    ScalarFunction add_function_int16(func_name,
                                      {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                      {DataType(LogicalType::kSmallInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int16);

    ScalarFunction add_function_int32(func_name,
                                      {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                      {DataType(LogicalType::kInteger)},
                                      &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int32);

    ScalarFunction add_function_int64(func_name,
                                      {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                      {DataType(LogicalType::kBigInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int64);

    ScalarFunction add_function_int128(func_name,
                                       {DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt)},
                                       {DataType(LogicalType::kHugeInt)},
                                       &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int128);

    ScalarFunction add_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, AddFunction>);
    function_set_ptr->AddFunction(add_function_float);

    ScalarFunction add_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, AddFunction>);
    function_set_ptr->AddFunction(add_function_double);

    ScalarFunction add_function_Decimal(func_name,
                                        {DataType(LogicalType::kDecimal), DataType(LogicalType::kDecimal)},
                                        {DataType(LogicalType::kDecimal)},
                                        &ScalarFunction::BinaryFunctionWithFailure<DecimalT, DecimalT, DecimalT, AddFunction>);
    function_set_ptr->AddFunction(add_function_Decimal);

    ScalarFunction add_function_date_interval(func_name,
                                              {DataType(LogicalType::kDate), DataType(LogicalType::kInterval)},
                                              {DataType(LogicalType::kDate)},
                                              &ScalarFunction::BinaryFunctionWithFailure<DateT, IntervalT, DateT, AddFunction>);
    function_set_ptr->AddFunction(add_function_date_interval);

    ScalarFunction add_function_interval_date(func_name,
                                              {DataType(LogicalType::kInterval), DataType(LogicalType::kDate)},
                                              {DataType(LogicalType::kDate)},
                                              &ScalarFunction::BinaryFunctionWithFailure<DateT, IntervalT, DateT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_date);

    ScalarFunction add_function_datetime_interval(func_name,
                                                  {DataType(LogicalType::kDateTime), DataType(LogicalType::kInterval)},
                                                  {DataType(LogicalType::kDateTime)},
                                                  &ScalarFunction::BinaryFunctionWithFailure<DateTimeT, IntervalT, DateTimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_datetime_interval);

    ScalarFunction add_function_interval_datetime(func_name,
                                                  {DataType(LogicalType::kInterval), DataType(LogicalType::kDateTime)},
                                                  {DataType(LogicalType::kDateTime)},
                                                  &ScalarFunction::BinaryFunctionWithFailure<IntervalT, DateTimeT, DateTimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_datetime);

    ScalarFunction add_function_timestamp_interval(func_name,
                                                   {DataType(LogicalType::kTimestamp), DataType(LogicalType::kInterval)},
                                                   {DataType(LogicalType::kTimestamp)},
                                                   &ScalarFunction::BinaryFunctionWithFailure<TimestampT, IntervalT, TimestampT, AddFunction>);
    function_set_ptr->AddFunction(add_function_timestamp_interval);

    ScalarFunction add_function_interval_timestamp(func_name,
                                                   {DataType(LogicalType::kInterval), DataType(LogicalType::kTimestamp)},
                                                   {DataType(LogicalType::kTimestamp)},
                                                   &ScalarFunction::BinaryFunctionWithFailure<IntervalT, TimestampT, TimestampT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_timestamp);

    ScalarFunction add_function_mixed_bigint(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_bigint);

    ScalarFunction add_function_bigint_mixed(func_name,
                                             {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<BigIntT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_bigint_mixed);

    ScalarFunction add_function_mixed_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_double);

    ScalarFunction add_function_double_mixed(func_name,
                                             {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<DoubleT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_double_mixed);

    ScalarFunction add_function_mixed_mixed(func_name,
                                            {DataType(LogicalType::kMixed), DataType(LogicalType::kMixed)},
                                            {DataType(LogicalType::kMixed)},
                                            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity