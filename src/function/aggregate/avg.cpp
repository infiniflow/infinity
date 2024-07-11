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

module avg;

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
struct AvgState {
public:
    inline void Initialize() {
        Status status = Status::NotSupport("Initialize average state.");
        RecoverableError(status);
    }

    inline void Update(const ValueType *__restrict, SizeT) {
        Status status = Status::NotSupport("Update average state.");
        RecoverableError(status);
    }

    inline void ConstantUpdate(const ValueType *__restrict, SizeT, SizeT) {
        Status status = Status::NotSupport("Constant update average state.");
        RecoverableError(status);
    }

    inline ptr_t Finalize() {
        Status status = Status::NotSupport("Finalize average state.");
        RecoverableError(status);
    }

    inline static SizeT Size(const DataType &data_type) {
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

    inline void Update(const TinyIntT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) {
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    [[nodiscard]] inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
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

    inline void Update(const SmallIntT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
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

    inline void Update(const IntegerT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
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

    inline void Update(const BigIntT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
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

    inline void Update(const FloatT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
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

    inline void Update(const DoubleT *__restrict input, SizeT idx) {
        if (count_ == std::numeric_limits<i64>::max()) {
            String error_message = fmt::format("Data count exceeds: {}", count_);
            UnrecoverableError(error_message);
        }
        this->count_++;
        value_ += input[idx];
    }

    inline void ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) {
        // TODO: Need to check overflow.
        this->count_ += count;
        value_ += (input[idx] * count);
    }

    inline ptr_t Finalize() {
        result_ = value_ / count_;
        return (ptr_t)&result_;
    }

    inline static SizeT Size(const DataType &) { return sizeof(value_) + sizeof(count_) + sizeof(result_); }
};

void RegisterAvgFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "AVG";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

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
        AggregateFunction avg_function =
            UnaryAggregate<AvgState<FloatT, DoubleT>, FloatT, DoubleT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    {
        AggregateFunction avg_function =
            UnaryAggregate<AvgState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(avg_function);
    }

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
