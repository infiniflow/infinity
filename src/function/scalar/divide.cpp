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

module divide;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import status;
import internal_types;
import data_type;
import logger;

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
        result = DoubleT(left) / DoubleT(right);
        return true;
    }
};

template <>
inline bool DivFunction::Run(FloatT left, FloatT right, FloatT &result) {
    result = left / DoubleT(right);
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
inline bool DivFunction::Run(HugeIntT, HugeIntT, HugeIntT &) {
    Status status = Status::NotSupport("Not implement huge int divide operator.");
    LOG_ERROR(status.message());
    RecoverableError(status);
    return false;
}

template <>
inline bool DivFunction::Run(HugeIntT, HugeIntT, DoubleT &) {
    Status status = Status::NotSupport("Not implement huge int divide operator.");
    LOG_ERROR(status.message());
    RecoverableError(status);
    return false;
}

void RegisterDivFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "/";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction div_function_int8(func_name,
                                     {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                     {DataType(LogicalType::kDouble)},
                                     &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, TinyIntT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int8);

    ScalarFunction div_function_int16(func_name,
                                      {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                      {DataType(LogicalType::kDouble)},
                                      &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, SmallIntT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int16);

    ScalarFunction div_function_int32(func_name,
                                      {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                      {DataType(LogicalType::kDouble)},
                                      &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int32);

    ScalarFunction div_function_int64(func_name,
                                      {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                      {DataType(LogicalType::kDouble)},
                                      &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int64);

    ScalarFunction div_function_int128(func_name,
                                       {DataType(LogicalType::kHugeInt), DataType(LogicalType::kHugeInt)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<HugeIntT, HugeIntT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_int128);

    ScalarFunction div_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kDouble)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_float);

    ScalarFunction div_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, DivFunction>);
    function_set_ptr->AddFunction(div_function_double);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity