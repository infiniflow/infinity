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

#include <compare>
#include <type_traits>

module inequals;

import logical_type;
import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct InEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        static_assert(false, "Unsupported type");
    }
};

struct BooleanInEqualsFunction {
    // keep compatible with template Run call
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        static_assert(false, "Unsupported type");
    }
};

template <>
inline void BooleanInEqualsFunction::Run<u8, u8, u8>(u8 left, u8 right, u8 &result) {
    result = (left ^ right);
}

template <>
inline void BooleanInEqualsFunction::Run<bool, bool, bool>(bool left, bool right, bool &result) {
    result = (left != right);
}

struct PODTypeInEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result.SetValue(left != right);
    }
};

struct ColumnValueReaderTypeInEqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        result.SetValue(!CheckReaderValueEquality(left, right));
    }
};

template <>
inline void InEqualsFunction::Run(MixedT, BigIntT, bool &) {
    Status status = Status::NotSupport("Not implement: mixed <> bigint");
    RecoverableError(status);
}

template <>
inline void InEqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <>
inline void InEqualsFunction::Run(MixedT, DoubleT, bool &) {
    Status status = Status::NotSupport("Not implement: mixed <> double");
    RecoverableError(status);
}

template <>
inline void InEqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <>
inline void InEqualsFunction::Run(MixedT, VarcharT, bool &) {
    Status status = Status::NotSupport("Not implement: mixed <> varchar");
    RecoverableError(status);
}

template <>
inline void InEqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    InEqualsFunction::Run(right, left, result);
}

template <typename CompareType, typename InEqualsFunction>
static void GenerateInEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = "<>";
    ScalarFunction inequals_function(func_name,
                                     {data_type, data_type},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(inequals_function);
}

void RegisterInEqualsFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "<>";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateInEqualsFunction<BooleanT, BooleanInEqualsFunction>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateInEqualsFunction<TinyIntT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateInEqualsFunction<SmallIntT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateInEqualsFunction<IntegerT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateInEqualsFunction<BigIntT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateInEqualsFunction<HugeIntT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateInEqualsFunction<FloatT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat16));
    GenerateInEqualsFunction<FloatT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kBFloat16));
    GenerateInEqualsFunction<FloatT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateInEqualsFunction<DoubleT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateInEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateInEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateInEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateInEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateInEqualsFunction<VarcharT, ColumnValueReaderTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateInEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateInEqualsFunction<DateT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateInEqualsFunction<TimeT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateInEqualsFunction<DateTimeT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateInEqualsFunction<TimestampT, PODTypeInEqualsFunction>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateInEqualsFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));
    //    GenerateInEqualsFunction<IntervalT>(function_set_ptr, DataType(LogicalType::kInterval));

    //    GenerateInEqualsFunction<PointT>(function_set_ptr, DataType(LogicalType::kPoint));
    //    GenerateInEqualsFunction<LineT>(function_set_ptr, DataType(LogicalType::kLine));
    //    GenerateInEqualsFunction<LineSegT>(function_set_ptr, DataType(LogicalType::kLineSeg));
    //    GenerateInEqualsFunction<BoxT>(function_set_ptr, DataType(LogicalType::kBox));
    //    GenerateInEqualsFunction<PathT>(function_set_ptr, DataType(LogicalType::kPath));
    //    GenerateInEqualsFunction<PolygonT>(function_set_ptr, DataType(LogicalType::kPolygon));
    //    GenerateInEqualsFunction<CircleT>(function_set_ptr, DataType(LogicalType::kCircle));
    //
    //    GenerateInEqualsFunction<BitmapT>(function_set_ptr, DataType(LogicalType::kBitmap));
    //    GenerateInEqualsFunction<UuidT>(function_set_ptr, DataType(LogicalType::kUuid));
    //    GenerateInEqualsFunction<BlobT>(function_set_ptr, DataType(LogicalType::kBlob));
    //    GenerateInEqualsFunction<EmbeddingT>(function_set_ptr, DataType(LogicalType::kEmbedding));

    //    GenerateInEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_inequals_bigint(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_bigint);

    ScalarFunction bigint_inequals_mixed(func_name,
                                         {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(bigint_inequals_mixed);

    ScalarFunction mix_inequals_double(func_name,
                                       {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_double);

    ScalarFunction double_inequals_mixed(func_name,
                                         {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                         DataType(kBoolean),
                                         &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(double_inequals_mixed);

    ScalarFunction mix_inequals_varchar(func_name,
                                        {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                        DataType(kBoolean),
                                        &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(mix_inequals_varchar);

    ScalarFunction varchar_inequals_mixed(func_name,
                                          {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                          DataType(kBoolean),
                                          &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, InEqualsFunction>);
    function_set_ptr->AddFunction(varchar_inequals_mixed);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
