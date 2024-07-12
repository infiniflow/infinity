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

module max;

import stl;
import catalog;
import status;
import infinity_exception;
import aggregate_function;
import aggregate_function_set;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;

namespace infinity {

template <typename ValueType, typename ResultType>
struct MaxState {
public:
    ValueType value_;

    void Initialize() {
        String error_message = "Not implement: Max::Initialize";
        UnrecoverableError(error_message);
    }

    void Update(const ValueType *__restrict, SizeT) {
        String error_message = "Not implement: Max::Update";
        UnrecoverableError(error_message);
    }

    inline void ConstantUpdate(const ValueType *__restrict, SizeT, SizeT) {
        String error_message = "Not implement: Max::ConstantUpdate";
        UnrecoverableError(error_message);
    }

    [[nodiscard]] ptr_t Finalize() const {
        String error_message = "Not implement: Max::Finalize";
        UnrecoverableError(error_message);
    }

    inline static SizeT Size(const DataType &) {
        String error_message = "Not implement: Max::Size";
        UnrecoverableError(error_message);
    }
};

template <>
struct MaxState<BooleanT, BooleanT> {
public:
    BooleanT value_;

    void Initialize() { this->value_ = false; }

    void Update(const BooleanT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const BooleanT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(BooleanT); }
};

template <>
struct MaxState<TinyIntT, TinyIntT> {
public:
    TinyIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i8>::min(); }

    void Update(const TinyIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(TinyIntT); }
};

template <>
struct MaxState<SmallIntT, SmallIntT> {
public:
    SmallIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i16>::min(); }

    void Update(const SmallIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(SmallIntT); }
};

template <>
struct MaxState<IntegerT, IntegerT> {
public:
    IntegerT value_;

    void Initialize() { this->value_ = std::numeric_limits<i32>::min(); }

    void Update(const IntegerT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(IntegerT); }
};

template <>
struct MaxState<BigIntT, BigIntT> {
public:
    BigIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i64>::min(); }

    void Update(const BigIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(BigIntT); }
};

template <>
struct MaxState<HugeIntT, HugeIntT> {
public:
    HugeIntT value_;

    void Initialize() {
        this->value_.lower = std::numeric_limits<i64>::min();
        this->value_.upper = std::numeric_limits<i64>::min();
    }

    void Update(const HugeIntT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const HugeIntT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(HugeIntT); }
};

template <>
struct MaxState<FloatT, FloatT> {
public:
    FloatT value_;

    void Initialize() { this->value_ = std::numeric_limits<f32>::min(); }

    void Update(const FloatT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(FloatT); }
};

template <>
struct MaxState<DoubleT, DoubleT> {
public:
    DoubleT value_;

    void Initialize() { this->value_ = std::numeric_limits<f64>::min(); }

    void Update(const DoubleT *__restrict input, SizeT idx) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline void ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT) { value_ = value_ < input[idx] ? input[idx] : value_; }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &) { return sizeof(DoubleT); }
};

void RegisterMaxFunction(const UniquePtr<Catalog> &catalog_ptr) {
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
    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
