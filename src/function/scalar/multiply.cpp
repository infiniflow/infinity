//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct MulFunction {
    template<typename TA, typename TB, typename TC>
    static inline bool
    Run(TA left, TB right, TC& result) {
        NotImplementError("Not implemented");
    }
};

// TinyIntT * TinyIntT = TinyIntT, and check overflow
template<>
inline bool
MulFunction::Run(TinyIntT left, TinyIntT right, TinyIntT& result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// SmallIntT * SmallIntT = SmallIntT, and check overflow
template<>
inline bool
MulFunction::Run(SmallIntT left, SmallIntT right, SmallIntT& result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// IntegerT * IntegerT = IntegerT, and check overflow
template<>
inline bool
MulFunction::Run(IntegerT left, IntegerT right, IntegerT& result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// BigIntT * BigIntT = BigIntT, and check overflow
template<>
inline bool
MulFunction::Run(BigIntT left, BigIntT right, BigIntT& result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// FloatT * FloatT = FloatT, and check overflow
template<>
inline bool
MulFunction::Run(FloatT left, FloatT right, FloatT& result) {
    result = left * right;
    if(std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT * DoubleT = DoubleT, and check overflow
template<>
inline bool
MulFunction::Run(DoubleT left, DoubleT right, DoubleT& result) {
    result = left * right;
    if(std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal16 * Decimal16 = Decimal16
template<>
inline bool
MulFunction::Run(Decimal16T left, Decimal16T right, Decimal16T& result) {
    NotImplementError("Decimal16 * Decimal16");
}

// Decimal32 * Decimal32 = Decimal32
template<>
inline bool
MulFunction::Run(Decimal32T left, Decimal32T right, Decimal32T& result) {
    NotImplementError("Decimal32 * Decimal32");
}

// Decimal64 * Decimal64 = Decimal64
template<>
inline bool
MulFunction::Run(Decimal64T left, Decimal64T right, Decimal64T& result) {
    NotImplementError("Decimal64 * Decimal64");
}

// Decimal128 * Decimal128 = Decimal128
template<>
inline bool
MulFunction::Run(Decimal128T left, Decimal128T right, Decimal128T& result) {
    NotImplementError("Decimal128 * Decimal128");
}

// Mixed Type * i64
template<>
inline bool
MulFunction::Run(MixedT left, BigIntT right, MixedT& result) {
    NotImplementError("MixedT * BigIntT");
}

// i64 * Mixed Type
template<>
inline bool
MulFunction::Run(BigIntT left, MixedT right, MixedT& result) {
    return MulFunction::Run(right, left, result);
}

// Mixed Type * f64
template<>
inline bool
MulFunction::Run(MixedT left, DoubleT right, MixedT& result) {
    NotImplementError("MixedT * DoubleT");
}

// f64 * Mixed Type
template<>
inline bool
MulFunction::Run(DoubleT left, MixedT right, MixedT& result) {
    return MulFunction::Run(right, left, result);
}

// Mixed Type * Mixed Type
template<>
inline bool
MulFunction::Run(MixedT left, MixedT right, MixedT& result) {
    NotImplementError("MixedT * MixedT");
}

void
RegisterMulFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "*";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction mul_function_int8(
            func_name,
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int8);

    ScalarFunction mul_function_int16(
            func_name,
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int16);

    ScalarFunction mul_function_int32(
            func_name,
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int32);

    ScalarFunction mul_function_int64(
            func_name,
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int64);

    ScalarFunction mul_function_int128(
            func_name,
            { DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt) },
            { DataType(LogicalType::kHugeInt) },
            &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int128);

    ScalarFunction mul_function_float(
            func_name,
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_float);

    ScalarFunction mul_function_double(
            func_name,
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_double);

    ScalarFunction mul_function_decimal16(
            func_name,
            { DataType(LogicalType::kDecimal16), DataType(LogicalType::kDecimal16) },
            { DataType(LogicalType::kDecimal16) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal16T, Decimal16T, Decimal16T, MulFunction>);
    function_set_ptr->AddFunction(mul_function_decimal16);

    ScalarFunction mul_function_decimal32(
            func_name,
            { DataType(LogicalType::kDecimal32), DataType(LogicalType::kDecimal32) },
            { DataType(LogicalType::kDecimal32) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal32T, Decimal32T, Decimal32T, MulFunction>);
    function_set_ptr->AddFunction(mul_function_decimal32);

    ScalarFunction mul_function_decimal64(
            func_name,
            { DataType(LogicalType::kDecimal64), DataType(LogicalType::kDecimal64) },
            { DataType(LogicalType::kDecimal64) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal64T, Decimal64T, Decimal64T, MulFunction>);
    function_set_ptr->AddFunction(mul_function_decimal64);

    ScalarFunction mul_function_decimal128(
            func_name,
            { DataType(LogicalType::kDecimal128), DataType(LogicalType::kDecimal128) },
            { DataType(LogicalType::kDecimal128) },
            &ScalarFunction::BinaryFunctionWithFailure<Decimal128T, Decimal128T, Decimal128T, MulFunction>);
    function_set_ptr->AddFunction(mul_function_decimal128);


    ScalarFunction add_function_mixed_bigint(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_bigint);

    ScalarFunction add_function_bigint_mixed(
            func_name,
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_bigint_mixed);

    ScalarFunction add_function_mixed_double(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_double);

    ScalarFunction add_function_double_mixed(
            func_name,
            { DataType(LogicalType::kDouble), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_double_mixed);

    ScalarFunction add_function_mixed_mixed(
            func_name,
            { DataType(LogicalType::kMixed), DataType(LogicalType::kMixed) },
            { DataType(LogicalType::kMixed) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_mixed);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}