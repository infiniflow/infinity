//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct ModuloFunction {
    template<typename TA, typename TB, typename TC>
    static inline bool
    Run(TA left, TB right, TC& result) {
        if(right == 0) {
            return false;
        }
        if(left == std::numeric_limits<TA>::min() && right == -1) {
            return false;
        }
        result = left % right;
        return true;
    }
};

template<>
inline bool
ModuloFunction::Run(HugeIntT left, HugeIntT right, HugeIntT &result) {
    NotImplementError("Not Implement huge int modulo operator")
}

template<>
inline bool
ModuloFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = std::fmod(left, right);
    if(std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

template<>
inline bool
ModuloFunction::Run(DoubleT left, DoubleT right, DoubleT &result) {
    result = std::fmod(left, right);
    if(std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

void
RegisterModuloFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("%");

    ScalarFunction mod_function_int8(
            "%",
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_int8);

    ScalarFunction mod_function_int16(
            "%",
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_int16);

    ScalarFunction mod_function_int32(
            "%",
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_int32);

    ScalarFunction mod_function_int64(
            "%",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_int64);

    ScalarFunction mod_function_int128(
            "%",
            { DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt) },
            { DataType(LogicalType::kHugeInt) },
            &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_int128);

    ScalarFunction mod_function_float(
            "%",
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_float);

    ScalarFunction mod_function_double(
            "%",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, ModuloFunction>);
    function_set_ptr->AddFunction(mod_function_double);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}