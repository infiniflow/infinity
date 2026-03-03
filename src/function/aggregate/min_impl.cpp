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

module infinity_core:min.impl;

import :new_catalog;
import :infinity_exception;
import :aggregate_function;
import :aggregate_function_set;
import :status;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

template <typename ValueType, typename ResultType>
struct MinState {
public:
    ValueType value_;

    void Initialize() { UnrecoverableError("Not implement: MinState::Initialize"); }

    void Update(const ValueType *__restrict, size_t) { UnrecoverableError("Not implement: MinState::Update"); }

    inline void ConstantUpdate(const ValueType *__restrict, size_t, size_t) { UnrecoverableError("Not implement: MinState::ConstantUpdate"); }

    [[nodiscard]] char *Finalize() const { UnrecoverableError("Not implement: MinState::Finalize"); }

    inline static size_t Size(const DataType &) { UnrecoverableError("Not implement: MinState::Size"); }
};

template <>
struct MinState<BooleanT, BooleanT> {
public:
    BooleanT value_;

    void Initialize() { this->value_ = true; }

    void Update(const BooleanT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const BooleanT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return 1; }
};

template <>
struct MinState<TinyIntT, TinyIntT> {
public:
    TinyIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i8>::max(); }

    void Update(const TinyIntT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const TinyIntT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(TinyIntT); }
};

template <>
struct MinState<SmallIntT, SmallIntT> {
public:
    SmallIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i16>::max(); }

    void Update(const SmallIntT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const SmallIntT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(SmallIntT); }
};

template <>
struct MinState<IntegerT, IntegerT> {
public:
    IntegerT value_;

    void Initialize() { this->value_ = std::numeric_limits<i32>::max(); }

    void Update(const IntegerT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const IntegerT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(IntegerT); }
};

template <>
struct MinState<BigIntT, BigIntT> {
public:
    BigIntT value_;

    void Initialize() { this->value_ = std::numeric_limits<i64>::max(); }

    void Update(const BigIntT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const BigIntT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(BigIntT); }
};

template <>
struct MinState<HugeIntT, HugeIntT> {
public:
    HugeIntT value_;

    void Initialize() {
        this->value_.lower = std::numeric_limits<i64>::max();
        this->value_.upper = std::numeric_limits<i64>::max();
    }

    void Update(const HugeIntT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const HugeIntT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(HugeIntT); }
};

template <>
struct MinState<Float16T, Float16T> {
public:
    Float16T value_;

    void Initialize() { this->value_ = std::numeric_limits<Float16T>::max(); }

    void Update(const Float16T *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const Float16T *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(Float16T); }
};

template <>
struct MinState<BFloat16T, BFloat16T> {
public:
    BFloat16T value_;

    void Initialize() { this->value_ = std::numeric_limits<BFloat16T>::max(); }

    void Update(const BFloat16T *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const BFloat16T *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(BFloat16T); }
};

template <>
struct MinState<FloatT, FloatT> {
public:
    FloatT value_;

    void Initialize() { this->value_ = std::numeric_limits<f32>::max(); }

    void Update(const FloatT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const FloatT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(FloatT); }
};

template <>
struct MinState<DoubleT, DoubleT> {
public:
    DoubleT value_;

    void Initialize() { this->value_ = std::numeric_limits<f64>::max(); }

    void Update(const DoubleT *__restrict input, size_t idx) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline void ConstantUpdate(const DoubleT *__restrict input, size_t idx, size_t) { value_ = input[idx] < value_ ? input[idx] : value_; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(DoubleT); }
};

template <>
struct MinState<VarcharT, VarcharT> {
public:
    static constexpr bool need_column_vector_ = true;
    VarcharT value_{};
    bool is_set_{false};

    void Initialize() {
        is_set_ = false;
        value_ = VarcharT{};
    }

    void Update(const VarcharT *__restrict input, size_t idx, const ColumnVector *column_vector) {
        const VarcharT &new_val = input[idx];
        if (!is_set_) {
            value_ = new_val;
            is_set_ = true;
        } else {
            std::span<const char> current_span = column_vector->GetVarcharInner(value_);
            std::span<const char> new_span = column_vector->GetVarchar(idx);

            uint32_t min_len = std::min(current_span.size(), new_span.size());
            int cmp = strncmp(current_span.data(), new_span.data(), min_len);
            bool new_is_less = (cmp > 0) || (cmp == 0 && current_span.size() > new_span.size());

            if (new_is_less) {
                value_ = new_val;
            }
        }
    }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(MinState<VarcharT, VarcharT>); }
};

template <>
struct MinState<DateT, DateT> {
public:
    DateT value_;

    void Initialize() { this->value_.value = std::numeric_limits<int32_t>::max(); }

    void Update(const DateT *__restrict input, size_t idx) { value_.value = input[idx].value < value_.value ? input[idx].value : value_.value; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(DateT); }
};

template <>
struct MinState<TimeT, TimeT> {
public:
    TimeT value_;

    void Initialize() { this->value_.value = std::numeric_limits<int32_t>::max(); }

    void Update(const TimeT *__restrict input, size_t idx) { value_.value = input[idx].value < value_.value ? input[idx].value : value_.value; }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(TimeT); }
};

template <>
struct MinState<DateTimeT, DateTimeT> {
public:
    DateTimeT value_;

    void Initialize() {
        value_.date.value = std::numeric_limits<int32_t>::max();
        value_.time.value = 0;
    }

    void Update(const DateTimeT *__restrict input, size_t idx) {
        bool is_less =
            (input[idx].date.value < value_.date.value) || (input[idx].date.value == value_.date.value && input[idx].time.value < value_.time.value);
        if (is_less) {
            value_ = input[idx];
        }
    }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(DateTimeT); }
};

template <>
struct MinState<TimestampT, TimestampT> {
public:
    TimestampT value_;

    void Initialize() {
        value_.date.value = std::numeric_limits<int32_t>::max();
        value_.time.value = 0;
    }

    void Update(const TimestampT *__restrict input, size_t idx) {
        bool is_less =
            (input[idx].date.value < value_.date.value) || (input[idx].date.value == value_.date.value && input[idx].time.value < value_.time.value);
        if (is_less) {
            value_ = input[idx];
        }
    }

    inline char *Finalize() { return (char *)&value_; }

    inline static size_t Size(const DataType &) { return sizeof(TimestampT); }
};

void RegisterMinFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "MIN";

    std::shared_ptr<AggregateFunctionSet> function_set_ptr = std::make_shared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction max_function = UnaryAggregate<MinState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                                                          DataType(LogicalType::kBoolean),
                                                                                                          DataType(LogicalType::kBoolean));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                                                          DataType(LogicalType::kTinyInt),
                                                                                                          DataType(LogicalType::kTinyInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<SmallIntT, SmallIntT>, SmallIntT, SmallIntT>(func_name,
                                                                                                              DataType(LogicalType::kSmallInt),
                                                                                                              DataType(LogicalType::kSmallInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<IntegerT, IntegerT>, IntegerT, IntegerT>(func_name,
                                                                                                          DataType(LogicalType::kInteger),
                                                                                                          DataType(LogicalType::kInteger));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MinState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                                                                                                          DataType(LogicalType::kHugeInt),
                                                                                                          DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MinState<FloatT, FloatT>, FloatT, FloatT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kFloat));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function =
            UnaryAggregate<MinState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<Float16T, Float16T>, Float16T, Float16T>(func_name,
                                                                                                          DataType(LogicalType::kFloat16),
                                                                                                          DataType(LogicalType::kFloat16));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function = UnaryAggregate<MinState<BFloat16T, BFloat16T>, BFloat16T, BFloat16T>(func_name,
                                                                                                              DataType(LogicalType::kBFloat16),
                                                                                                              DataType(LogicalType::kBFloat16));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction min_function =
            UnaryAggregate<MinState<DateT, DateT>, DateT, DateT>(func_name, DataType(LogicalType::kDate), DataType(LogicalType::kDate));
        function_set_ptr->AddFunction(min_function);
    }
    {
        AggregateFunction min_function =
            UnaryAggregate<MinState<TimeT, TimeT>, TimeT, TimeT>(func_name, DataType(LogicalType::kTime), DataType(LogicalType::kTime));
        function_set_ptr->AddFunction(min_function);
    }
    {
        AggregateFunction min_function = UnaryAggregate<MinState<DateTimeT, DateTimeT>, DateTimeT, DateTimeT>(func_name,
                                                                                                              DataType(LogicalType::kDateTime),
                                                                                                              DataType(LogicalType::kDateTime));
        function_set_ptr->AddFunction(min_function);
    }
    {
        AggregateFunction min_function = UnaryAggregate<MinState<TimestampT, TimestampT>, TimestampT, TimestampT>(func_name,
                                                                                                                  DataType(LogicalType::kTimestamp),
                                                                                                                  DataType(LogicalType::kTimestamp));
        function_set_ptr->AddFunction(min_function);
    }
    {
        AggregateFunction min_function = UnaryAggregate<MinState<VarcharT, VarcharT>, VarcharT, VarcharT>(func_name,
                                                                                                          DataType(LogicalType::kVarchar),
                                                                                                          DataType(LogicalType::kVarchar));
        function_set_ptr->AddFunction(min_function);
    }
    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
