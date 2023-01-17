//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct SubFunction {
    template<typename TA, typename TB, typename TC>
    static inline bool
    Run(TA left, TB right, TC& result) {
        NotImplementError("No implementation");
    }
};

template<>
inline bool
SubFunction::Run(TinyIntT left, TinyIntT right, TinyIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template<>
inline bool
SubFunction::Run(SmallIntT left, SmallIntT right, SmallIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template<>
inline bool
SubFunction::Run(IntegerT left, IntegerT right, IntegerT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

template<>
inline bool
SubFunction::Run(BigIntT left, BigIntT right, BigIntT &result) {
    if (__builtin_sub_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// HugeIntT - HugeIntT = HugeIntT, and check overflow
template<>
inline bool
SubFunction::Run(HugeIntT left, HugeIntT right, HugeIntT &result) {
    NotImplementError("Not implemented: HugeIntT - HugeIntT = HugeIntT");
}

// FloatT - FloatT = FloatT, and check overflow
template<>
inline bool
SubFunction::Run(FloatT left, FloatT right, FloatT& result) {
    result = left - right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT - DoubleT = DoubleT, and check overflow
template<>
inline bool
SubFunction::Run(DoubleT left, DoubleT right, DoubleT& result) {
    result = left - right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal16 - Decimal16 = Decimal16
template<>
inline bool
SubFunction::Run(Decimal16T left, Decimal16T right, Decimal16T& result) {
    NotImplementError("Decimal16 - Decimal16");
}

// Decimal32 - Decimal32 = Decimal32
template<>
inline bool
SubFunction::Run(Decimal32T left, Decimal32T right, Decimal32T& result) {
    NotImplementError("Decimal32 - Decimal32");
}

// Decimal64 - Decimal64 = Decimal64
template<>
inline bool
SubFunction::Run(Decimal64T left, Decimal64T right, Decimal64T& result) {
    NotImplementError("Decimal64 - Decimal64");
}

// Decimal128 - Decimal128 = Decimal128
template<>
inline bool
SubFunction::Run(Decimal128T left, Decimal128T right, Decimal128T& result) {
    NotImplementError("Decimal128 - Decimal128");
}

// DateTime - Interval
template<>
inline bool
SubFunction::Run(DateTimeT left, IntervalT right, DateTimeT& result) {
    NotImplementError("DateTimeT - IntervalT");
}

// TimestampT - Interval
template<>
inline bool
SubFunction::Run(TimestampT left, IntervalT right, TimestampT& result) {
    NotImplementError("TimestampT - IntervalT");
}

// TimestampTZT - Interval
template<>
inline bool
SubFunction::Run(TimestampTZT left, IntervalT right, TimestampTZT& result) {
    NotImplementError("TimestampT - IntervalT");
}

// Mixed Type - i64
template<>
inline bool
SubFunction::Run(MixedT left, BigIntT right, MixedT& result) {
    NotImplementError("MixedT - BigIntT");
}

// i64 - Mixed Type
template<>
inline bool
SubFunction::Run(BigIntT left, MixedT right, MixedT& result) {
        NotImplementError("BigIntT - MixedT");
}

// Mixed Type - f64
template<>
inline bool
SubFunction::Run(MixedT left, DoubleT right, MixedT& result) {
    NotImplementError("MixedT - DoubleT");
}

// f64 - Mixed Type
template<>
inline bool
SubFunction::Run(DoubleT left, MixedT right, MixedT& result) {
    NotImplementError("DoubleT - MixedT");
}

// Mixed Type - Mixed Type
template<>
inline bool
SubFunction::Run(MixedT left, MixedT right, MixedT& result) {
    NotImplementError("MixedT - MixedT");
}

void
RegisterSubtractFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "-";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

//    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction sub_function_int8(
            func_name,
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int8);

    ScalarFunction sub_function_int16(
            func_name,
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int16);

    ScalarFunction sub_function_int32(
            func_name,
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int32);

    ScalarFunction sub_function_int64(
            func_name,
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int64);

    ScalarFunction sub_function_int128(
            func_name,
            { DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt) },
            { DataType(LogicalType::kHugeInt) },
            &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int128);

    ScalarFunction sub_function_float(
            func_name,
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_float);

    ScalarFunction sub_function_double(
            func_name,
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<double, double, double, SubFunction>);
    function_set_ptr->AddFunction(sub_function_double);

    ScalarFunction sub_function_decimal16(
            func_name,
            { DataType(LogicalType::kDecimal16), DataType(LogicalType::kDecimal16) },
            { DataType(LogicalType::kDecimal16) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal16T, Decimal16T, Decimal16T, SubFunction>);
    function_set_ptr->AddFunction(sub_function_decimal16);

    ScalarFunction sub_function_decimal32(
            func_name,
            { DataType(LogicalType::kDecimal32), DataType(LogicalType::kDecimal32) },
            { DataType(LogicalType::kDecimal32) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal32T, Decimal32T, Decimal32T, SubFunction>);
    function_set_ptr->AddFunction(sub_function_decimal32);

    ScalarFunction sub_function_decimal64(
            func_name,
            { DataType(LogicalType::kDecimal64), DataType(LogicalType::kDecimal64) },
            { DataType(LogicalType::kDecimal64) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal64T, Decimal64T, Decimal64T, SubFunction>);
    function_set_ptr->AddFunction(sub_function_decimal64);

    ScalarFunction sub_function_decimal128(
            func_name,
            { DataType(LogicalType::kDecimal128), DataType(LogicalType::kDecimal128) },
            { DataType(LogicalType::kDecimal128) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal128T, Decimal128T, Decimal128T, SubFunction>);
    function_set_ptr->AddFunction(sub_function_decimal128);

    ScalarFunction sub_function_datetime_interval(
            func_name,
            { DataType(LogicalType::kDateTime), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kDateTime) },
            &ScalarFunction::BinaryFunctionWithFailure<DateTimeT, IntervalT, DateTimeT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_datetime_interval);

    ScalarFunction sub_function_timestamp_interval(
            func_name,
            { DataType(LogicalType::kTimestamp), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kTimestamp) },
            &ScalarFunction::BinaryFunctionWithFailure<TimestampT, IntervalT, TimestampT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_timestamp_interval);

    ScalarFunction sub_function_timestamptz_interval(
            func_name,
            { DataType(LogicalType::kTimestampTZ), DataType(LogicalType::kInterval) },
            { DataType(LogicalType::kTimestampTZ) },
            &ScalarFunction::BinaryFunctionWithFailure<TimestampTZT, IntervalT, TimestampTZT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_timestamptz_interval);

    ScalarFunction sub_function_mixed_bigint(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_bigint);

    ScalarFunction sub_function_bigint_mixed(
            func_name,
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_bigint_mixed);

    ScalarFunction sub_function_mixed_double(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_double);

    ScalarFunction sub_function_double_mixed(
            func_name,
            { DataType(LogicalType::kDouble), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_double_mixed);

    ScalarFunction sub_function_mixed_mixed(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_mixed_mixed);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}