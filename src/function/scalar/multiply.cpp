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

module multiply;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct MulFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA, TB, TC &) {
        String error_message = "Not implement: MulFunction::Run";
        UnrecoverableError(error_message);
        return false;
    }
};

// TinyIntT * TinyIntT = TinyIntT, and check overflow
template <>
inline bool MulFunction::Run(TinyIntT left, TinyIntT right, TinyIntT &result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// SmallIntT * SmallIntT = SmallIntT, and check overflow
template <>
inline bool MulFunction::Run(SmallIntT left, SmallIntT right, SmallIntT &result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// IntegerT * IntegerT = IntegerT, and check overflow
template <>
inline bool MulFunction::Run(IntegerT left, IntegerT right, IntegerT &result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// BigIntT * BigIntT = BigIntT, and check overflow
template <>
inline bool MulFunction::Run(BigIntT left, BigIntT right, BigIntT &result) {
    if (__builtin_mul_overflow(left, right, &result)) {
        return false;
    }
    return true;
}

// FloatT * FloatT = FloatT, and check overflow
template <>
inline bool MulFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = left * right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// DoubleT * DoubleT = DoubleT, and check overflow
template <>
inline bool MulFunction::Run(DoubleT left, DoubleT right, DoubleT &result) {
    result = left * right;
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }
    return true;
}

// Decimal * Decimal = Decimal
template <>
inline bool MulFunction::Run(DecimalT, DecimalT, DecimalT &) {
    String error_message = "Not implement: MulFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

// Mixed Type * i64
template <>
inline bool MulFunction::Run(MixedT, BigIntT, MixedT &) {
    String error_message = "Not implement: MulFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

// i64 * Mixed Type
template <>
inline bool MulFunction::Run(BigIntT left, MixedT right, MixedT &result) {
    return MulFunction::Run(right, left, result);
}

// Mixed Type * f64
template <>
inline bool MulFunction::Run(MixedT, DoubleT, MixedT &) {
    String error_message = "Not implement: MulFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

// f64 * Mixed Type
template <>
inline bool MulFunction::Run(DoubleT left, MixedT right, MixedT &result) {
    return MulFunction::Run(right, left, result);
}

// Mixed Type * Mixed Type
template <>
inline bool MulFunction::Run(MixedT, MixedT, MixedT &) {
    String error_message = "Not implement: MulFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

void RegisterMulFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "*";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction mul_function_int8(func_name,
                                     {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                     {DataType(LogicalType::kTinyInt)},
                                     &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, TinyIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int8);

    ScalarFunction mul_function_int16(func_name,
                                      {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                      {DataType(LogicalType::kSmallInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, SmallIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int16);

    ScalarFunction mul_function_int32(func_name,
                                      {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                      {DataType(LogicalType::kInteger)},
                                      &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, IntegerT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int32);

    ScalarFunction mul_function_int64(func_name,
                                      {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                      {DataType(LogicalType::kBigInt)},
                                      &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, BigIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int64);

    ScalarFunction mul_function_int128(func_name,
                                       {DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt)},
                                       {DataType(LogicalType::kHugeInt)},
                                       &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, HugeIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int128);

    ScalarFunction mul_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_float);

    ScalarFunction mul_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_double);

    ScalarFunction mul_function_Decimal(func_name,
                                        {DataType(LogicalType::kDecimal), DataType(LogicalType::kDecimal)},
                                        {DataType(LogicalType::kDecimal)},
                                        &ScalarFunction::BinaryFunctionWithFailure<DecimalT, DecimalT, DecimalT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_Decimal);

    ScalarFunction add_function_mixed_bigint(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, BigIntT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_bigint);

    ScalarFunction add_function_bigint_mixed(func_name,
                                             {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<BigIntT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_bigint_mixed);

    ScalarFunction add_function_mixed_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_double);

    ScalarFunction add_function_double_mixed(func_name,
                                             {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                             {DataType(LogicalType::kMixed)},
                                             &ScalarFunction::BinaryFunctionWithFailure<DoubleT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_double_mixed);

    ScalarFunction add_function_mixed_mixed(func_name,
                                            {DataType(LogicalType::kMixed), DataType(LogicalType::kMixed)},
                                            {DataType(LogicalType::kMixed)},
                                            &ScalarFunction::BinaryFunctionWithFailure<MixedT, MixedT, MixedT, MulFunction>);
    function_set_ptr->AddFunction(add_function_mixed_mixed);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity