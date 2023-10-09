//
// Created by JinHai on 2022/9/14.
//

#include "sum.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template <typename ValueType, typename ResultType>
struct SumState {
public:
    inline void Initialize() { NotImplementError("Initialize sum state.") }

    inline void Update(const ValueType *__restrict input, SizeT idx) { NotImplementError("Update sum state.") }

    inline void ConstantUpdate(const ValueType *__restrict input, SizeT idx, SizeT count) { NotImplementError("Constant update sum state.") }

    inline ptr_t Finalize() { NotImplementError("Finalize sum state.") }

    inline static SizeT Size(const DataType &data_type) { NotImplementError("Average state type size: " + data_type.ToString()) }
};

template <>
struct SumState<TinyIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const TinyIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(TinyIntT); }
};

template <>
struct SumState<SmallIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const SmallIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(SmallIntT); }
};

template <>
struct SumState<IntegerT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const IntegerT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(IntegerT); }
};

template <>
struct SumState<BigIntT, BigIntT> {
public:
    i64 sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const BigIntT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(BigIntT); }
};

template <>
struct SumState<FloatT, DoubleT> {
public:
    DoubleT sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const FloatT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(FloatT); }
};

template <>
struct SumState<DoubleT, DoubleT> {
public:
    DoubleT sum_;

    inline void Initialize() { this->sum_ = 0; }

    inline void Update(const DoubleT *__restrict input, SizeT idx) { sum_ += input[idx]; }

    inline void ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) { sum_ += input[idx] * count; }

    inline ptr_t Finalize() { return (ptr_t)&sum_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(DoubleT); }
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
