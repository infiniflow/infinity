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
import new_catalog;

import infinity_exception;
import aggregate_function;
import aggregate_function_set;
import parser;
import third_party;

module count;

namespace infinity {

template <typename ValueType, typename ResultType>
struct CountState {
public:
    i64 count_;

    void Initialize() { this->count_ = 0; }

    void Update(ValueType *__restrict, SizeT) { count_++; }

    inline void ConstantUpdate(ValueType *__restrict, SizeT, SizeT count) { count_ += count; }

    inline ptr_t Finalize() { return (ptr_t)&count_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

void RegisterCountFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "COUNT";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction count_function = UnaryAggregate<CountState<BooleanT, BigIntT>, BooleanT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kBoolean),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<TinyIntT, BigIntT>, TinyIntT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kTinyInt),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<SmallIntT, BigIntT>, SmallIntT, BigIntT>(func_name,
                                                                                                              DataType(LogicalType::kSmallInt),
                                                                                                              DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<IntegerT, BigIntT>, IntegerT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kInteger),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<HugeIntT, BigIntT>, HugeIntT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kHugeInt),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<FloatT, BigIntT>, FloatT, BigIntT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<DoubleT, BigIntT>, DoubleT, BigIntT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<DecimalT, BigIntT>, DecimalT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kDecimal),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<VarcharT, BigIntT>, VarcharT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kVarchar),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<DateT, BigIntT>, DateT, BigIntT>(func_name, DataType(LogicalType::kDate), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<TimeT, BigIntT>, TimeT, BigIntT>(func_name, DataType(LogicalType::kTime), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<DateTimeT, BigIntT>, DateTimeT, BigIntT>(func_name,
                                                                                                              DataType(LogicalType::kDateTime),
                                                                                                              DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<TimestampT, BigIntT>, TimestampT, BigIntT>(func_name,
                                                                                                                DataType(LogicalType::kTimestamp),
                                                                                                                DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<ArrayT, BigIntT>, ArrayT, BigIntT>(func_name, DataType(LogicalType::kArray), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<TupleT, BigIntT>, TupleT, BigIntT>(func_name, DataType(LogicalType::kTuple), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<PointT, BigIntT>, PointT, BigIntT>(func_name, DataType(LogicalType::kPoint), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<LineT, BigIntT>, LineT, BigIntT>(func_name, DataType(LogicalType::kLine), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<LineSegT, BigIntT>, LineSegT, BigIntT>(func_name,
                                                                                                            DataType(LogicalType::kLineSeg),
                                                                                                            DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<BoxT, BigIntT>, BoxT, BigIntT>(func_name, DataType(LogicalType::kBox), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        //        AggregateFunction count_function =
        //            UnaryAggregate<CountState<PathT, BigIntT>, PathT, BigIntT>(func_name, DataType(LogicalType::kPath),
        //            DataType(LogicalType::kBigInt));
        //        function_set_ptr->AddFunction(count_function);
    }
    {
        //        AggregateFunction count_function = UnaryAggregate<CountState<PolygonT, BigIntT>, PolygonT, BigIntT>(func_name,
        //                                                                                                            DataType(LogicalType::kPolygon),
        //                                                                                                            DataType(LogicalType::kBigInt));
        //        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<CircleT, BigIntT>, CircleT, BigIntT>(func_name, DataType(LogicalType::kCircle), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        //        AggregateFunction count_function =
        //            UnaryAggregate<CountState<BitmapT, BigIntT>, BitmapT, BigIntT>(func_name, DataType(LogicalType::kBitmap),
        //            DataType(LogicalType::kBigInt));
        //        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<UuidT, BigIntT>, UuidT, BigIntT>(func_name, DataType(LogicalType::kUuid), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        //        AggregateFunction count_function =
        //            UnaryAggregate<CountState<BlobT, BigIntT>, BlobT, BigIntT>(func_name, DataType(LogicalType::kBlob),
        //            DataType(LogicalType::kBigInt));
        //        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function = UnaryAggregate<CountState<EmbeddingT, BigIntT>, EmbeddingT, BigIntT>(func_name,
                                                                                                                DataType(LogicalType::kEmbedding),
                                                                                                                DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<RowID, BigIntT>, RowID, BigIntT>(func_name, DataType(LogicalType::kRowID), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    {
        AggregateFunction count_function =
            UnaryAggregate<CountState<MixedT, BigIntT>, MixedT, BigIntT>(func_name, DataType(LogicalType::kMixed), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(count_function);
    }
    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
