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

module max;

namespace infinity {

template <typename ValueType, typename ResultType>
struct MaxState {
public:
    ValueType value_;

    void Initialize() { Error<NotImplementException>("Not implemented"); }

    void Update(const ValueType *__restrict input, SizeT idx) { Error<NotImplementException>("Not implemented"); }

    inline void ConstantUpdate(const ValueType *__restrict input, SizeT idx, SizeT count) { Error<NotImplementException>("Not implemented"); }

    [[nodiscard]] ptr_t Finalize() const { Error<NotImplementException>("Not implemented"); }

    inline static SizeT Size(const DataType &data_type) { Error<NotImplementException>("Not implemented"); }
};

template <>
struct MaxState<BooleanT, BooleanT> {
public:
    BooleanT value_;

    void Initialize() { this->value_ = false; }

    void Update(const BooleanT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const BooleanT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(BooleanT); }
};

template <>
struct MaxState<TinyIntT, TinyIntT> {
public:
    TinyIntT value_;

    void Initialize() { this->value_ = i8_min; }

    void Update(const TinyIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(TinyIntT); }
};

template <>
struct MaxState<SmallIntT, SmallIntT> {
public:
    SmallIntT value_;

    void Initialize() { this->value_ = i16_min; }

    void Update(const SmallIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(SmallIntT); }
};

template <>
struct MaxState<IntegerT, IntegerT> {
public:
    IntegerT value_;

    void Initialize() { this->value_ = i32_min; }

    void Update(const IntegerT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(IntegerT); }
};

template <>
struct MaxState<BigIntT, BigIntT> {
public:
    BigIntT value_;

    void Initialize() { this->value_ = i64_min; }

    void Update(const BigIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(BigIntT); }
};

template <>
struct MaxState<HugeIntT, HugeIntT> {
public:
    HugeIntT value_;

    void Initialize() {
        this->value_.lower = i64_min;
        this->value_.upper = i64_min;
    }

    void Update(const HugeIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const HugeIntT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(HugeIntT); }
};

template <>
struct MaxState<FloatT, FloatT> {
public:
    FloatT value_;

    void Initialize() { this->value_ = f32_min; }

    void Update(const FloatT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(FloatT); }
};

template <>
struct MaxState<DoubleT, DoubleT> {
public:
    DoubleT value_;

    void Initialize() { this->value_ = f64_min; }

    void Update(const DoubleT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(DoubleT); }
};

void RegisterMaxFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "MAX";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction max_function = UnaryAggregate<MaxState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                                                          DataType(LogicalType::kBoolean),
                                                                                                          DataType(LogicalType::kBoolean));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MaxState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                                                          DataType(LogicalType::kTinyInt),
                                                                                                          DataType(LogicalType::kTinyInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MaxState<SmallIntT, SmallIntT>, SmallIntT, SmallIntT>(func_name,
                                                                                                              DataType(LogicalType::kSmallInt),
                                                                                                              DataType(LogicalType::kSmallInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MaxState<IntegerT, IntegerT>, IntegerT, IntegerT>(func_name,
                                                                                                          DataType(LogicalType::kInteger),
                                                                                                          DataType(LogicalType::kInteger));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MaxState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MaxState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                                                                                                          DataType(LogicalType::kHugeInt),
                                                                                                          DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MaxState<FloatT, FloatT>, FloatT, FloatT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kFloat));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MaxState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(max_function);
    }
#if 0

    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<DecimalT, DecimalT>, DecimalT, DecimalT>(func_name,
                                                                                   DataType(LogicalType::kDecimal),
                                                                                   DataType(LogicalType::kDecimal));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<VarcharT, VarcharT>, VarcharT, VarcharT>(func_name,
                                                                                   DataType(LogicalType::kVarchar),
                                                                                   DataType(LogicalType::kVarchar));
        function_set_ptr->AddFunction(max_function);
    }
#endif
    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
