//
// Created by jinhai on 23-1-10.
//

#include "minus.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct MinusFunction {
    template<typename TA, typename TB>
    static inline bool
    Run(TA value, TB &result) {
        if(value == std::numeric_limits<TA>::min()) return false;
        result = -value;
        return true;
    }
};

template<>
inline bool
MinusFunction::Run(HugeIntT value, HugeIntT &result) {
    NotImplementError("No implementation on minus big int")
}

template<>
inline bool
MinusFunction::Run(Decimal16T value, Decimal16T &result) {
    NotImplementError("No implementation on minus decimal16")
}

template<>
inline bool
MinusFunction::Run(Decimal32T value, Decimal32T &result) {
    NotImplementError("No implementation on minus decimal32")
}

template<>
inline bool
MinusFunction::Run(Decimal64T value, Decimal64T &result) {
    NotImplementError("No implementation on minus decimal64")
}

template<>
inline bool
MinusFunction::Run(Decimal128T value, Decimal128T &result) {
    NotImplementError("No implementation on minus decimal128")
}

template<>
inline bool
MinusFunction::Run(FloatT value, FloatT &result) {
    if(std::isinf(value) || std::isnan(value)) return false;
    result = -value;
    return true;
}

template<>
inline bool
MinusFunction::Run(DoubleT value, DoubleT &result) {
    if(std::isinf(value) || std::isnan(value)) return false;
    result = -value;
    return true;
}

template<>
inline bool
MinusFunction::Run(MixedT value, MixedT &result) {
    switch(value.type) {
        case MixedValueType::kInteger: {
            auto int_value_ptr = (IntegerMixedType*)(&value);
            auto int_result_ptr = (IntegerMixedType*)(&result);
            int_result_ptr->value = -(int_value_ptr->value);
            return true;
        }
        case MixedValueType::kFloat: {
            auto float_value_ptr = (FloatMixedType*)(&value);
            auto float_result_ptr = (FloatMixedType*)(&result);
            float_result_ptr->value = -(float_value_ptr->value);
            return true;
        }
        default: {
            result.type = MixedValueType::kNull;
        }
    }
    return false;
}

void
RegisterMinusFunction(const UniquePtr<Catalog> &catalog_ptr) {
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>("-");

    // Minus operator
    ScalarFunction minus_int8(
            "-",
            {DataType(LogicalType::kTinyInt)},
            DataType(LogicalType::kTinyInt),
            &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, TinyIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int8);

    ScalarFunction minus_int16(
            "-",
            {DataType(LogicalType::kSmallInt)},
            DataType(LogicalType::kSmallInt),
            &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, SmallIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int16);

    ScalarFunction minus_int32(
            "-",
            {DataType(LogicalType::kInteger)},
            DataType(LogicalType::kInteger),
            &ScalarFunction::UnaryFunctionWithFailure<IntegerT, IntegerT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int32);

    ScalarFunction minus_int64(
            "-",
            {DataType(LogicalType::kBigInt)},
            DataType(LogicalType::kBigInt),
            &ScalarFunction::UnaryFunctionWithFailure<BigIntT, BigIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int64);

    ScalarFunction minus_int128(
            "-",
            {DataType(LogicalType::kHugeInt)},
            DataType(LogicalType::kHugeInt),
            &ScalarFunction::UnaryFunctionWithFailure<HugeIntT, HugeIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int128);

    ScalarFunction minus_float(
            "-",
            {DataType(LogicalType::kFloat)},
            DataType(LogicalType::kFloat),
            &ScalarFunction::UnaryFunctionWithFailure<FloatT, FloatT, MinusFunction>);
    function_set_ptr->AddFunction(minus_float);

    ScalarFunction minus_double(
            "-",
            {DataType(LogicalType::kDouble)},
            DataType(LogicalType::kDouble),
            &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, MinusFunction>);
    function_set_ptr->AddFunction(minus_double);

    ScalarFunction minus_decimal16(
            "-",
            {DataType(LogicalType::kDecimal16)},
            DataType(LogicalType::kDecimal16),
            &ScalarFunction::UnaryFunctionWithFailure<Decimal16T, Decimal16T, MinusFunction>);
    function_set_ptr->AddFunction(minus_decimal16);

    ScalarFunction minus_decimal32(
            "-",
            {DataType(LogicalType::kDecimal32)},
            DataType(LogicalType::kDecimal32),
            &ScalarFunction::UnaryFunctionWithFailure<Decimal32T, Decimal32T, MinusFunction>);
    function_set_ptr->AddFunction(minus_decimal32);

    ScalarFunction minus_decimal64(
            "-",
            {DataType(LogicalType::kDecimal64)},
            DataType(LogicalType::kDecimal64),
            &ScalarFunction::UnaryFunctionWithFailure<Decimal64T, Decimal64T, MinusFunction>);
    function_set_ptr->AddFunction(minus_decimal64);

    ScalarFunction minus_decimal128(
            "-",
            {DataType(LogicalType::kDecimal128)},
            DataType(LogicalType::kDecimal128),
            &ScalarFunction::UnaryFunctionWithFailure<Decimal128T, Decimal128T, MinusFunction>);
    function_set_ptr->AddFunction(minus_decimal128);

    ScalarFunction minus_mixed(
            "-",
            {DataType(LogicalType::kMixed)},
            DataType(LogicalType::kMixed),
            &ScalarFunction::UnaryFunctionWithFailure<MixedT, MixedT, MinusFunction>);
    function_set_ptr->AddFunction(minus_mixed);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}
}
