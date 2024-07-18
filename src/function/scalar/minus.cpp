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

module minus;

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
inline bool MinusFunction::Run(HugeIntT, HugeIntT &) {
    String error_message = "Not implement: MinusFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool MinusFunction::Run(DecimalT, DecimalT &) {
    String error_message = "Not implement: MinusFunction::Run";
    UnrecoverableError(error_message);
    return false;
}

template <>
inline bool MinusFunction::Run(Float16T value, Float16T &result) {
    result = -value;
    if (const auto f = static_cast<float>(result); std::isinf(f) || std::isnan(f)) {
        return false;
    }
    return true;
}

template <>
inline bool MinusFunction::Run(BFloat16T value, BFloat16T &result) {
    result = -value;
    if (const auto f = static_cast<float>(result); std::isinf(f) || std::isnan(f)) {
        return false;
    }
    return true;
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

SharedPtr<ScalarFunctionSet> GetMinusFunctionSet() {
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

    ScalarFunction minus_float16(func_name,
                                 {DataType(LogicalType::kFloat16)},
                                 DataType(LogicalType::kFloat16),
                                 &ScalarFunction::UnaryFunctionWithFailure<Float16T, Float16T, MinusFunction>);
    function_set_ptr->AddFunction(minus_float16);

    ScalarFunction minus_bfloat16(func_name,
                                  {DataType(LogicalType::kBFloat16)},
                                  DataType(LogicalType::kBFloat16),
                                  &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, BFloat16T, MinusFunction>);
    function_set_ptr->AddFunction(minus_bfloat16);

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

    return function_set_ptr;
}

void RegisterMinusFunction(const UniquePtr<Catalog> &catalog_ptr) {
    auto function_set_ptr = GetMinusFunctionSet();
    Catalog::AddFunctionSet(catalog_ptr.get(), std::move(function_set_ptr));
}

void AppendRegisterMinusFunction(const UniquePtr<Catalog> &catalog_ptr) {
    auto function_set_ptr = GetMinusFunctionSet();
    Catalog::AppendToScalarFunctionSet(catalog_ptr.get(), std::move(function_set_ptr));
}

} // namespace infinity
