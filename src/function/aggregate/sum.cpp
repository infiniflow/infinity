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
import aggregate_function;
import aggregate_function_set;
import parser;
import third_party;

module sum;

namespace infinity {

template <typename ValueType, typename ResultType>
struct SumState {
public:
    inline void Initialize() { Error<NotImplementException>("Not implemented"); }

    inline void Update(const ValueType *__restrict, SizeT) { Error<NotImplementException>("Not implemented"); }

    inline void ConstantUpdate(const ValueType *__restrict, SizeT, SizeT) { Error<NotImplementException>("Not implemented"); }

    inline ptr_t Finalize() { Error<NotImplementException>("Not implemented"); }

    inline static SizeT Size(const DataType &) { Error<NotImplementException>("Not implemented"); }
};

template <>
struct SumState<TinyIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const TinyIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

template <>
struct SumState<SmallIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const SmallIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

template <>
struct SumState<IntegerT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const IntegerT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

template <>
struct SumState<BigIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const BigIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(i64); }
};

template <>
struct SumState<FloatT, DoubleT> {
public:
    DoubleT sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const FloatT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(DoubleT); }
};

template <>
struct SumState<DoubleT, DoubleT> {
public:
    DoubleT sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const DoubleT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &) { return sizeof(DoubleT); }
};

void RegisterSumFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "SUM";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction sum_function = UnaryAggregate<SumState<TinyIntT, BigIntT>, TinyIntT, BigIntT>(func_name,
                                                                                                        DataType(LogicalType::kTinyInt),
                                                                                                        DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function = UnaryAggregate<SumState<SmallIntT, BigIntT>, SmallIntT, BigIntT>(func_name,
                                                                                                          DataType(LogicalType::kSmallInt),
                                                                                                          DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function = UnaryAggregate<SumState<IntegerT, BigIntT>, IntegerT, BigIntT>(func_name,
                                                                                                        DataType(LogicalType::kInteger),
                                                                                                        DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function =
            UnaryAggregate<SumState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(sum_function);
    }

    {
#if 0
        AggregateFunction sum_function
                = UnaryAggregate<SumState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                                                                                  DataType(LogicalType::kHugeInt),
                                                                                  DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(sum_function);
#endif
    }

    {
        AggregateFunction sum_function =
            UnaryAggregate<SumState<FloatT, DoubleT>, FloatT, DoubleT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function =
            UnaryAggregate<SumState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(sum_function);
    }
#if 0
    {
        AggregateFunction sum_function
                = UnaryAggregate<SumState<Decimal16T, Decimal16T>, Decimal16T, Decimal16T>(func_name,
                                                                                           DataType(LogicalType::kDecimal16),
                                                                                           DataType(LogicalType::kDecimal16));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function
                = UnaryAggregate<SumState<Decimal32T, Decimal32T>, Decimal32T, Decimal32T>(func_name,
                                                                                           DataType(LogicalType::kDecimal32),
                                                                                           DataType(LogicalType::kDecimal32));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function
                = UnaryAggregate<SumState<Decimal64T, Decimal64T>, Decimal64T, Decimal64T>(func_name,
                                                                                           DataType(LogicalType::kDecimal64),
                                                                                           DataType(LogicalType::kDecimal64));
        function_set_ptr->AddFunction(sum_function);
    }

    {
        AggregateFunction sum_function
                = UnaryAggregate<SumState<Decimal128T, Decimal128T>, Decimal128T, Decimal128T>(func_name,
                                                                                               DataType(LogicalType::kDecimal128),
                                                                                               DataType(LogicalType::kDecimal128));
        function_set_ptr->AddFunction(sum_function);
    }
#endif
    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
