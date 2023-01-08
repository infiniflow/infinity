//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct AddFunction {
    template<typename TA, typename TB, typename TC>
    static inline bool
    Run(TA left, TB right, TC& result) {
        NotImplementError("Not implemented");
    }
};

// TinyIntT + TinyIntT = TinyIntT, and check overflow
template<>
inline bool
AddFunction::Run(TinyIntT left, TinyIntT right, TinyIntT& result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// SmallIntT + SmallIntT = SmallIntT, and check overflow
template<>
inline bool
AddFunction::Run(SmallIntT left, SmallIntT right, SmallIntT& result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// IntegerT + IntegerT = IntegerT, and check overflow
template<>
inline bool
AddFunction::Run(IntegerT left, IntegerT right, IntegerT& result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// BigIntT + BigIntT = BigIntT, and check overflow
template<>
inline bool
AddFunction::Run(BigIntT left, BigIntT right, BigIntT& result) {
    if (__builtin_add_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// HugeIntT + HugeIntT = HugeIntT, and check overflow
template<>
inline bool
AddFunction::Run(HugeIntT left, HugeIntT right, HugeIntT& result) {
    NotImplementError("Not implemented: HugeIntT + HugeIntT = HugeIntT");
}

// FloatT + FloatT = FloatT, and check overflow
template<>
inline bool
AddFunction::Run(FloatT left, FloatT right, FloatT& result) {
    result = left + right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT + DoubleT = DoubleT, and check overflow
template<>
inline bool
AddFunction::Run(DoubleT left, DoubleT right, DoubleT& result) {
    result = left + right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal16 + Decimal16 = Decimal16
template<>
inline bool
AddFunction::Run(Decimal16T left, Decimal16T right, Decimal16T& result) {
    NotImplementError("Decimal16 + Decimal16");
}

// Decimal32 + Decimal32 = Decimal32
template<>
inline bool
AddFunction::Run(Decimal32T left, Decimal32T right, Decimal32T& result) {
    NotImplementError("Decimal32 + Decimal32");
}

// Decimal64 + Decimal64 = Decimal64
template<>
inline bool
AddFunction::Run(Decimal64T left, Decimal64T right, Decimal64T& result) {
    NotImplementError("Decimal64 + Decimal64");
}

// Decimal128 + Decimal128 = Decimal128
template<>
inline bool
AddFunction::Run(Decimal128T left, Decimal128T right, Decimal128T& result) {
    NotImplementError("Decimal128 + Decimal128");
}

// DateTime + Interval
template<>
inline bool
AddFunction::Run(DateTimeT left, IntervalT right, DateTimeT& result) {
    NotImplementError("DateTimeT + IntervalT");
}

// Interval + DateTime
template<>
inline bool
AddFunction::Run(IntervalT left, DateTimeT right, DateTimeT& result) {
    return AddFunction::Run(right, left, result);
}

// Date + Interval
template<>
inline bool
AddFunction::Run(DateT left, IntervalT right, DateT& result) {
    NotImplementError("DateT + IntervalT");
}

// Interval + Date
template<>
inline bool
AddFunction::Run(IntervalT left, DateT right, DateT& result) {
    return AddFunction::Run(right, left, result);
}

// Time + Interval
template<>
inline bool
AddFunction::Run(TimeT left, IntervalT right, TimeT& result) {
    NotImplementError("DateT + IntervalT");
}

// Interval + Time
template<>
inline bool
AddFunction::Run(IntervalT left, TimeT right, TimeT& result) {
    return AddFunction::Run(right, left, result);
}

// TimestampT + Interval
template<>
inline bool
AddFunction::Run(TimestampT left, IntervalT right, TimestampT& result) {
    NotImplementError("TimestampT + IntervalT");
}

// Interval + TimestampT
template<>
inline bool
AddFunction::Run(IntervalT left, TimestampT right, TimestampT& result) {
    return AddFunction::Run(right, left, result);
}

// TimestampTZT + Interval
template<>
inline bool
AddFunction::Run(TimestampTZT left, IntervalT right, TimestampTZT& result) {
    NotImplementError("TimestampT + IntervalT");
}

// Interval + TimestampTZT
template<>
inline bool
AddFunction::Run(IntervalT left, TimestampTZT right, TimestampTZT& result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + i64
template<>
inline bool
AddFunction::Run(MixedT left, BigIntT right, MixedT& result) {
    NotImplementError("MixedT + BigIntT");
}

// i64 + Mixed Type
template<>
inline bool
AddFunction::Run(BigIntT left, MixedT right, MixedT& result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + f64
template<>
inline bool
AddFunction::Run(MixedT left, DoubleT right, MixedT& result) {
    NotImplementError("MixedT + DoubleT");
}

// f64 + Mixed Type
template<>
inline bool
AddFunction::Run(DoubleT left, MixedT right, MixedT& result) {
    return AddFunction::Run(right, left, result);
}

// Mixed Type + Mixed Type
template<>
inline bool
AddFunction::Run(MixedT left, MixedT right, MixedT& result) {
    NotImplementError("MixedT + MixedT");
}

void
RegisterAddFunction(const UniquePtr<Catalog> &catalog_ptr) {
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>("+");

//    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction add_function_int8(
            "+",
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int8);

    ScalarFunction add_function_int16(
            "+",
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int16);

    ScalarFunction add_function_int32(
            "+",
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int32);

    ScalarFunction add_function_int64(
            "+",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int64);

    ScalarFunction add_function_int128(
            "+",
            { DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt) },
            { DataType(LogicalType::kHugeInt) },
            &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int128);


    ScalarFunction add_function_float(
            "+",
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, AddFunction>);
    function_set_ptr->AddFunction(add_function_float);

    ScalarFunction add_function_double(
            "+",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, AddFunction>);
    function_set_ptr->AddFunction(add_function_double);

    ScalarFunction add_function_decimal16(
            "+",
            { DataType(LogicalType::kDecimal16), DataType(LogicalType::kDecimal16) },
            { DataType(LogicalType::kDecimal16) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal16T, Decimal16T, Decimal16T, AddFunction>);
    function_set_ptr->AddFunction(add_function_decimal16);

    ScalarFunction add_function_decimal32(
            "+",
            { DataType(LogicalType::kDecimal32), DataType(LogicalType::kDecimal32) },
            { DataType(LogicalType::kDecimal32) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal32T, Decimal32T, Decimal32T, AddFunction>);
    function_set_ptr->AddFunction(add_function_decimal32);

    ScalarFunction add_function_decimal64(
            "+",
            { DataType(LogicalType::kDecimal64), DataType(LogicalType::kDecimal64) },
            { DataType(LogicalType::kDecimal64) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal64T, Decimal64T, Decimal64T, AddFunction>);
    function_set_ptr->AddFunction(add_function_decimal64);

    ScalarFunction add_function_decimal128(
            "+",
            { DataType(LogicalType::kDecimal128), DataType(LogicalType::kDecimal128) },
            { DataType(LogicalType::kDecimal128) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal128T, Decimal128T, Decimal128T, AddFunction>);
    function_set_ptr->AddFunction(add_function_decimal128);

    ScalarFunction add_function_datetime_interval(
            "+",
            { DataType(LogicalType::kDateTime), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kDateTime) },
            &ScalarFunction::BinaryFunctionWithFailure<DateTimeT, IntervalT, DateTimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_datetime_interval);

    ScalarFunction add_function_interval_datetime(
            "+",
            { DataType(LogicalType::kInterval), DataType(LogicalType::kDateTime) },
            { DataType(LogicalType::kDateTime) },
            &ScalarFunction::BinaryFunctionWithFailure<IntervalT, DateTimeT, DateTimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_datetime);

    ScalarFunction add_function_date_interval(
            "+",
            { DataType(LogicalType::kDate), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kDate) },
            &ScalarFunction::BinaryFunctionWithFailure<DateT, IntervalT, DateT, AddFunction>);
    function_set_ptr->AddFunction(add_function_date_interval);

    ScalarFunction add_function_interval_date(
            "+",
            { DataType(LogicalType::kInterval), DataType(LogicalType::kDate) },
            { DataType(LogicalType::kDate) },
            &ScalarFunction::BinaryFunctionWithFailure<IntervalT, DateT, DateT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_date);

    ScalarFunction add_function_time_interval(
            "+",
            { DataType(LogicalType::kTime), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kTime) },
            &ScalarFunction::BinaryFunctionWithFailure<TimeT, IntervalT, TimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_time_interval);

    ScalarFunction add_function_interval_time(
            "+",
            { DataType(LogicalType::kInterval), DataType(LogicalType::kTime) },
            { DataType(LogicalType::kTime) },
            &ScalarFunction::BinaryFunctionWithFailure<IntervalT, TimeT, TimeT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_time);

    ScalarFunction add_function_timestamp_interval(
            "+",
            { DataType(LogicalType::kTimestamp), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kTimestamp) },
            &ScalarFunction::BinaryFunctionWithFailure<TimestampT, IntervalT, TimestampT, AddFunction>);
    function_set_ptr->AddFunction(add_function_timestamp_interval);

    ScalarFunction add_function_interval_timestamp(
            "+",
            { DataType(LogicalType::kInterval), DataType(LogicalType::kTimestamp) },
            { DataType(LogicalType::kTimestamp) },
            &ScalarFunction::BinaryFunctionWithFailure<IntervalT, TimestampT, TimestampT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_timestamp);

    ScalarFunction add_function_timestamp_tz_interval(
            "+",
            { DataType(LogicalType::kTimestampTZ), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kTimestampTZ) },
            &ScalarFunction::BinaryFunctionWithFailure<TimestampTZT, IntervalT, TimestampTZT, AddFunction>);
    function_set_ptr->AddFunction(add_function_timestamp_tz_interval);

    ScalarFunction add_function_interval_timestamp_tz(
            "+",
            { DataType(LogicalType::kInterval), DataType(LogicalType::kTimestampTZ) },
            { DataType(LogicalType::kTimestampTZ) },
            &ScalarFunction::BinaryFunctionWithFailure<IntervalT, TimestampTZT, TimestampTZT, AddFunction>);
    function_set_ptr->AddFunction(add_function_interval_timestamp_tz);

    ScalarFunction add_function_mixed_bigint(
            "+",
            { DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_bigint);

    ScalarFunction add_function_bigint_mixed(
            "+",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_bigint_mixed);

    ScalarFunction add_function_mixed_double(
            "+",
            { DataType(LogicalType::kMixed), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_double);

    ScalarFunction add_function_double_mixed(
            "+",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_double_mixed);

    ScalarFunction add_function_mixed_mixed(
            "+",
            { DataType(LogicalType::kMixed), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, AddFunction>);
    function_set_ptr->AddFunction(add_function_mixed_mixed);

    // TODO: other type add need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}