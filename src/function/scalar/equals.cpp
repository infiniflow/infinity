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

import stl;
import catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module equals;

namespace infinity {

struct EqualsFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = (left == right);
    }
};

template <>
inline void EqualsFunction::Run(VarcharT left, VarcharT right, bool &result) {
    if (left.length_ != right.length_) {
        result = false;
        return;
    }
    if (left.IsInlined()) {
        result = (Memcmp(left.short_.data_, right.short_.data_, left.length_) == 0);
        return;
    } else {
        // Both left and right are not inline
        if(left.IsValue() && right.IsValue()) {
            if (Memcmp(left.value_.prefix_, right.value_.prefix_, VARCHAR_PREFIX_LEN) == 0) {
                result = (Memcmp(left.value_.ptr_, right.value_.ptr_, left.length_) == 0);
                return;
            }
        } else {
            Error<NotImplementException>("Column vector varchar can't be compared");
        }
    }
    result = false;
}

template <>
inline void EqualsFunction::Run(MixedT, BigIntT, bool &) {
    Error<NotImplementException>("Not implement: mixed == bigint");
}

template <>
inline void EqualsFunction::Run(BigIntT left, MixedT right, bool &result) {
    EqualsFunction::Run(right, left, result);
}

template <>
inline void EqualsFunction::Run(MixedT, DoubleT, bool &) {
    Error<NotImplementException>("Not implement: mixed == double");
}

template <>
inline void EqualsFunction::Run(DoubleT left, MixedT right, bool &result) {
    EqualsFunction::Run(right, left, result);
}

template <>
inline void EqualsFunction::Run(MixedT, VarcharT, bool &) {
    Error<NotImplementException>("Not implement: mixed == varchar");
}

template <>
inline void EqualsFunction::Run(VarcharT left, MixedT right, bool &result) {
    EqualsFunction::Run(right, left, result);
}

template <typename CompareType>
static void GenerateEqualsFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr, DataType data_type) {
    String func_name = "=";
    ScalarFunction equals_function(func_name,
                                   {data_type, data_type},
                                   DataType(kBoolean),
                                   &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(equals_function);
}

void RegisterEqualsFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "=";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateEqualsFunction<BooleanT>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateEqualsFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateEqualsFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateEqualsFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateEqualsFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateEqualsFunction<HugeIntT>(function_set_ptr, DataType(LogicalType::kHugeInt));
    GenerateEqualsFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateEqualsFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    //    GenerateEqualsFunction<Decimal16T>(function_set_ptr, DataType(LogicalType::kDecimal16));
    //    GenerateEqualsFunction<Decimal32T>(function_set_ptr, DataType(LogicalType::kDecimal32));
    //    GenerateEqualsFunction<Decimal64T>(function_set_ptr, DataType(LogicalType::kDecimal64));
    //    GenerateEqualsFunction<Decimal128T>(function_set_ptr, DataType(LogicalType::kDecimal128));

    GenerateEqualsFunction<VarcharT>(function_set_ptr, DataType(LogicalType::kVarchar));
    //    GenerateEqualsFunction<CharT>(function_set_ptr, DataType(LogicalType::kChar));

    GenerateEqualsFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
    GenerateEqualsFunction<TimeT>(function_set_ptr, DataType(LogicalType::kTime));
    GenerateEqualsFunction<DateTimeT>(function_set_ptr, DataType(LogicalType::kDateTime));
    GenerateEqualsFunction<TimestampT>(function_set_ptr, DataType(LogicalType::kTimestamp));
    //    GenerateEqualsFunction<TimestampTZT>(function_set_ptr, DataType(LogicalType::kTimestampTZ));
    //    GenerateEqualsFunction<IntervalT>(function_set_ptr, DataType(LogicalType::kInterval));

    //    GenerateEqualsFunction<PointT>(function_set_ptr, DataType(LogicalType::kPoint));
    //    GenerateEqualsFunction<LineT>(function_set_ptr, DataType(LogicalType::kLine));
    //    GenerateEqualsFunction<LineSegT>(function_set_ptr, DataType(LogicalType::kLineSeg));
    //    GenerateEqualsFunction<BoxT>(function_set_ptr, DataType(LogicalType::kBox));
    //    GenerateEqualsFunction<PathT>(function_set_ptr, DataType(LogicalType::kPath));
    //    GenerateEqualsFunction<PolygonT>(function_set_ptr, DataType(LogicalType::kPolygon));
    //    GenerateEqualsFunction<CircleT>(function_set_ptr, DataType(LogicalType::kCircle));
    //
    //    GenerateEqualsFunction<BitmapT>(function_set_ptr, DataType(LogicalType::kBitmap));
    //    GenerateEqualsFunction<UuidT>(function_set_ptr, DataType(LogicalType::kUuid));
    //    GenerateEqualsFunction<BlobT>(function_set_ptr, DataType(LogicalType::kBlob));
    //    GenerateEqualsFunction<EmbeddingT>(function_set_ptr, DataType(LogicalType::kEmbedding));

    //    GenerateEqualsFunction<MixedT>(function_set_ptr, DataType(LogicalType::kMixed));

    ScalarFunction mix_equals_bigint(func_name,
                                     {DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt)},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<MixedT, BigIntT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(mix_equals_bigint);

    ScalarFunction bigint_equals_mixed(func_name,
                                       {DataType(LogicalType::kBigInt), DataType(LogicalType::kMixed)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<BigIntT, MixedT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(bigint_equals_mixed);

    ScalarFunction mix_equals_double(func_name,
                                     {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                     DataType(kBoolean),
                                     &ScalarFunction::BinaryFunction<MixedT, DoubleT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(mix_equals_double);

    ScalarFunction double_equals_mixed(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kMixed)},
                                       DataType(kBoolean),
                                       &ScalarFunction::BinaryFunction<DoubleT, MixedT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(double_equals_mixed);

    ScalarFunction mix_equals_varchar(func_name,
                                      {DataType(LogicalType::kMixed), DataType(LogicalType::kVarchar)},
                                      DataType(kBoolean),
                                      &ScalarFunction::BinaryFunction<MixedT, VarcharT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(mix_equals_varchar);

    ScalarFunction varchar_equals_mixed(func_name,
                                        {DataType(LogicalType::kVarchar), DataType(LogicalType::kMixed)},
                                        DataType(kBoolean),
                                        &ScalarFunction::BinaryFunction<VarcharT, MixedT, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(varchar_equals_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
