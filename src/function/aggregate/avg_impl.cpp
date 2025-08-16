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

module infinity_core:avg.impl;

import :avg;
import :new_catalog;
import :status;
import :infinity_exception;
import :aggregate_function;
import :aggregate_function_set;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

template <typename ValueType, typename ResultType>
struct AvgState {
public:
    inline void Initialize() {
        Status status = Status::NotSupport("Initialize average state.");
        RecoverableError(status);
    }

    inline void Update(const ValueType *__restrict, size_t) {
        Status status = Status::NotSupport("Update average state.");
        RecoverableError(status);
    }

    inline void ConstantUpdate(const ValueType *__restrict, size_t, size_t) {
        Status status = Status::NotSupport("Constant update average state.");
        RecoverableError(status);
    }

    inline char *Finalize() {
        Status status = Status::NotSupport("Finalize average state.");
        RecoverableError(status);
    }

    inline static size_t Size(const DataType &data_type) {
        Status status = Status::NotSupport(fmt::format("Average state type size: {}", data_type.ToString()));
        RecoverableError(status);
    }
};

template <>
struct AvgState<TinyIntT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const TinyIntT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const TinyIntT *__restrict input, size_t idx, size_t count) {
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    [[nodiscard]] inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<SmallIntT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const SmallIntT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const SmallIntT *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<IntegerT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const IntegerT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const IntegerT *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<BigIntT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const BigIntT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const BigIntT *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<Float16T, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const Float16T *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += static_cast<float>(input[idx]);
    }

    inline void ConstantUpdate(const Float16T *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += static_cast<float>(input[idx]) * count;
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<BFloat16T, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const BFloat16T *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += static_cast<float>(input[idx]);
    }

    inline void ConstantUpdate(const BFloat16T *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += static_cast<float>(input[idx]) * count;
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<FloatT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const FloatT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const FloatT *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

template <>
struct AvgState<DoubleT, DoubleT> {
public:
    double value_{};
    i64 count_{};
    double result_{};

    inline void Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void Update(const DoubleT *__restrict input, size_t idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            UnrecoverableError(fmt::format("Data count exceeds: {}", count_));
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const DoubleT *__restrict input, size_t idx, size_t count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline char *Finalize() {
        result_ = value_ / count_;
        return (char *)&result_;
    }

    inline static size_t Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

void RegisterAvgFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "AVG";

    std::shared_ptr<AggregateFunctionSet> function_set_ptr = std::make_shared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction avg_function = UnaryAggregate<AvgState<TinyIntT, DoubleT>, TinyIntT, DoubleT>(func_name,
                                                                                                        DataType(LogicalType::kTinyInt),
                                                                                                        DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function = UnaryAggregate<AvgState<SmallIntT, DoubleT>, SmallIntT, DoubleT>(func_name,
                                                                                                          DataType(LogicalType::kSmallInt),
                                                                                                          DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function = UnaryAggregate<AvgState<IntegerT, DoubleT>, IntegerT, DoubleT>(func_name,
                                                                                                        DataType(LogicalType::kInteger),
                                                                                                        DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function =
            UnaryAggregate<AvgState<BigIntT, DoubleT>, BigIntT, DoubleT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        // HugeIntT average function isn't implemented
#if 0
        AggregateFunction avg_function
                = UnaryAggregate<AvgState<HugeIntT, DoubleT>, HugeIntT, DoubleT>(func_name,
                                                                               DataType(LogicalType::kHugeInt),
                                                                               DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
#endif
    }

    {
        // Decimal average function isn't implemented
#if 0
        AggregateFunction avg_function
                = UnaryAggregate<AvgState<DecimalT, DoubleT>, DecimalT, DoubleT>(func_name,
                                                                                     DataType(LogicalType::kDecimal),
                                                                                     DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
#endif
    }

    {
        AggregateFunction avg_function = UnaryAggregate<AvgState<Float16T, DoubleT>, Float16T, DoubleT>(func_name,
                                                                                                        DataType(LogicalType::kFloat16),
                                                                                                        DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function = UnaryAggregate<AvgState<BFloat16T, DoubleT>, BFloat16T, DoubleT>(func_name,
                                                                                                          DataType(LogicalType::kBFloat16),
                                                                                                          DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function =
            UnaryAggregate<AvgState<FloatT, DoubleT>, FloatT, DoubleT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function =
            UnaryAggregate<AvgState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
