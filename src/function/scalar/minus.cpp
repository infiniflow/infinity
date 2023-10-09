//
// Created by jinhai on 23-1-10.
//

#include "minus.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct MinusFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA value, TB &result) {
        if (value == std::numeric_limits<TA>::min())
            return false;
        result = -value;
        return true;
    }
};

template <>
inline bool MinusFunction::Run(HugeIntT value, HugeIntT &result) {
    NotImplementError("No implementation on minus big int")
}

template <>
inline bool MinusFunction::Run(DecimalT value, DecimalT &result) {
    NotImplementError("No implementation on minus Decimal")
}

template <>
inline bool MinusFunction::Run(FloatT value, FloatT &result) {
    if (std::isinf(value) || std::isnan(value))
        return false;
    result = -value;
    return true;
}

template <>
inline bool MinusFunction::Run(DoubleT value, DoubleT &result) {
    if (std::isinf(value) || std::isnan(value))
        return false;
    result = -value;
    return true;
}

template <>
inline bool MinusFunction::Run(MixedT value, MixedT &result) {
    switch (value.type) {
        case MixedValueType::kInteger: {
            auto int_value_ptr = (IntegerMixedType *)(&value);
            auto int_result_ptr = (IntegerMixedType *)(&result);
            int_result_ptr->value = -(int_value_ptr->value);
            return true;
        }
        case MixedValueType::kFloat: {
            auto float_value_ptr = (FloatMixedType *)(&value);
            auto float_result_ptr = (FloatMixedType *)(&result);
            float_result_ptr->value = -(float_value_ptr->value);
            return true;
        }
        default: {
            result.type = MixedValueType::kNull;
        }
    }
    return false;
}

void RegisterMinusFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "-";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    // Minus operator
    ScalarFunction minus_int8(func_name,
                              {DataType(LogicalType::kTinyInt)},
                              DataType(LogicalType::kTinyInt),
                              &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, TinyIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int8);

    ScalarFunction minus_int16(func_name,
                               {DataType(LogicalType::kSmallInt)},
                               DataType(LogicalType::kSmallInt),
                               &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, SmallIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int16);

    ScalarFunction minus_int32(func_name,
                               {DataType(LogicalType::kInteger)},
                               DataType(LogicalType::kInteger),
                               &ScalarFunction::UnaryFunctionWithFailure<IntegerT, IntegerT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int32);

    ScalarFunction minus_int64(func_name,
                               {DataType(LogicalType::kBigInt)},
                               DataType(LogicalType::kBigInt),
                               &ScalarFunction::UnaryFunctionWithFailure<BigIntT, BigIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int64);

    ScalarFunction minus_int128(func_name,
                                {DataType(LogicalType::kHugeInt)},
                                DataType(LogicalType::kHugeInt),
                                &ScalarFunction::UnaryFunctionWithFailure<HugeIntT, HugeIntT, MinusFunction>);
    function_set_ptr->AddFunction(minus_int128);

    ScalarFunction minus_float(func_name,
                               {DataType(LogicalType::kFloat)},
                               DataType(LogicalType::kFloat),
                               &ScalarFunction::UnaryFunctionWithFailure<FloatT, FloatT, MinusFunction>);
    function_set_ptr->AddFunction(minus_float);

    ScalarFunction minus_double(func_name,
                                {DataType(LogicalType::kDouble)},
                                DataType(LogicalType::kDouble),
                                &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, MinusFunction>);
    function_set_ptr->AddFunction(minus_double);

    ScalarFunction minus_decimal(func_name,
                                 {DataType(LogicalType::kDecimal)},
                                 DataType(LogicalType::kDecimal),
                                 &ScalarFunction::UnaryFunctionWithFailure<DecimalT, DecimalT, MinusFunction>);
    function_set_ptr->AddFunction(minus_decimal);

    ScalarFunction minus_mixed(func_name,
                               {DataType(LogicalType::kMixed)},
                               DataType(LogicalType::kMixed),
                               &ScalarFunction::UnaryFunctionWithFailure<MixedT, MixedT, MinusFunction>);
    function_set_ptr->AddFunction(minus_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}
} // namespace infinity
