//
// Created by jinhai on 23-1-10.
//

#include "plus.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct PlusFunction {
    template<typename TA, typename TB>
    static inline bool
    Run(TA value, TB &result) {
        NotImplementError("Not implemented plus function")
    }
};

template<>
inline bool
PlusFunction::Run(MixedT value, MixedT &result) {
    if (value.type == MixedValueType::kInteger || value.type == MixedValueType::kFloat) {
        result = value;
        return true;
    } else {
        result.type = MixedValueType::kNull;
        return false;
    }
}

void
RegisterPlusFunction(const UniquePtr<Catalog> &catalog_ptr) {
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>("+");

    // Plus operator
    ScalarFunction plus_int8(
            "+",
            {DataType(LogicalType::kTinyInt)},
            DataType(LogicalType::kTinyInt),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int8);

    ScalarFunction plus_int16(
            "+",
            {DataType(LogicalType::kSmallInt)},
            DataType(LogicalType::kSmallInt),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int16);

    ScalarFunction plus_int32(
            "+",
            {DataType(LogicalType::kInteger)},
            DataType(LogicalType::kInteger),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int32);

    ScalarFunction plus_int64(
            "+",
            {DataType(LogicalType::kBigInt)},
            DataType(LogicalType::kBigInt),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int64);

    ScalarFunction plus_int128(
            "+",
            {DataType(LogicalType::kHugeInt)},
            DataType(LogicalType::kHugeInt),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int128);

    ScalarFunction plus_float(
            "+",
            {DataType(LogicalType::kFloat)},
            DataType(LogicalType::kFloat),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_float);

    ScalarFunction plus_double(
            "+",
            {DataType(LogicalType::kDouble)},
            DataType(LogicalType::kDouble),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_double);

    ScalarFunction plus_decimal16(
            "+",
            {DataType(LogicalType::kDecimal16)},
            DataType(LogicalType::kDecimal16),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_decimal16);

    ScalarFunction plus_decimal32(
            "+",
            {DataType(LogicalType::kDecimal32)},
            DataType(LogicalType::kDecimal32),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_decimal32);

    ScalarFunction plus_decimal64(
            "+",
            {DataType(LogicalType::kDecimal64)},
            DataType(LogicalType::kDecimal64),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_decimal64);

    ScalarFunction plus_decimal128(
            "+",
            {DataType(LogicalType::kDecimal128)},
            DataType(LogicalType::kDecimal128),
            &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_decimal128);

    ScalarFunction plus_mixed(
            "+",
            {DataType(LogicalType::kMixed)},
            DataType(LogicalType::kMixed),
            &ScalarFunction::UnaryFunctionWithFailure<MixedT, MixedT, PlusFunction>);
    function_set_ptr->AddFunction(plus_mixed);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
