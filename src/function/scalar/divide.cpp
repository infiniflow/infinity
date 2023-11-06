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

module divide;

namespace infinity {

struct DivFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA left, TB right, TC &result) {
        if (right == 0) {
            return false;
        }
        if (left == std::numeric_limits<TA>::min() && right == -1) {
            return false;
        }
        result = left / right;
        return true;
    }
};

template <>
inline bool DivFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = left / right;
    if (std::isnan(result) || std::isinf(result))
        return false;
    return true;
}

template <>
inline bool DivFunction::Run(DoubleT left, DoubleT right, DoubleT &result) {
    result = left / right;
    if (std::isnan(result) || std::isinf(result))
        return false;
    return true;
}

template <>
inline bool DivFunction::Run(HugeIntT left, HugeIntT right, HugeIntT &result) {
    Error<NotImplementException>("Not implement huge int divide operator.");
}

void RegisterDivFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "/";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction div_function_int8(func_name,
                                     {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                     {DataType(LogicalType::kTinyInt)},
                                     &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int8);

    ScalarFunction div_function_int16(func_name,
                                      {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                      {DataType(LogicalType::kSmallInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int16);

    ScalarFunction div_function_int32(func_name,
                                      {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                      {DataType(LogicalType::kInteger)},
                                      &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int32);

    ScalarFunction div_function_int64(func_name,
                                      {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                      {DataType(LogicalType::kBigInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int64);

    ScalarFunction div_function_int128(func_name,
                                       {DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt)},
                                       {DataType(LogicalType::kHugeInt)},
                                       &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int128);

    ScalarFunction div_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, DivFunction>);
    function_set_ptr->AddFunction(div_function_float);

    ScalarFunction div_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_double);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity