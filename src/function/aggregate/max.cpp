//
// Created by JinHai on 2022/9/14.
//

#include <utility>

#include "max.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct MaxState {
public:
    ValueType value_;

    void
    Initialize() {
        NotImplementError("Initialize max state.")
    }

    void
    Update(const ValueType *__restrict input, SizeT idx) {
        NotImplementError("Update max state.")
    }

    inline void
    ConstantUpdate(const ValueType *__restrict input, SizeT idx, SizeT count) {
        NotImplementError("Constant update max state.")
    }

    [[nodiscard]] ptr_t
    Finalize() const {
        NotImplementError("Finalize max state.")
    }

    inline static SizeT
    Size(const DataType& data_type) {
        NotImplementError("Max state type size: " + data_type.ToString())
    }
};

template<>
struct MaxState<BooleanT, BooleanT> {
public:
    BooleanT value_;

    void
    Initialize() {
        this->value_ = false;
    }

    void
    Update(const BooleanT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const BooleanT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(BooleanT);
    }
};

template<>
struct MaxState<TinyIntT, TinyIntT> {
public:
    TinyIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<TinyIntT>::min();
    }

    void
    Update(const TinyIntT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(TinyIntT);
    }
};

template<>
struct MaxState<SmallIntT, SmallIntT> {
public:
    SmallIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<SmallIntT>::min();
    }

    void
    Update(const SmallIntT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(SmallIntT);
    }
};

template<>
struct MaxState<IntegerT, IntegerT> {
public:
    IntegerT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<IntegerT>::min();
    }

    void
    Update(const IntegerT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(IntegerT);
    }
};

template<>
struct MaxState<BigIntT, BigIntT> {
public:
    BigIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<BigIntT>::min();
    }

    void
    Update(const BigIntT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(BigIntT);
    }
};

template<>
struct MaxState<HugeIntT, HugeIntT> {
public:
    HugeIntT value_;

    void
    Initialize() {
        this->value_.lower = std::numeric_limits<BigIntT>::min();
        this->value_.upper = std::numeric_limits<BigIntT>::min();
    }

    void
    Update(const HugeIntT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const HugeIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(HugeIntT);
    }
};

template<>
struct MaxState<FloatT, FloatT> {
public:
    FloatT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<FloatT>::min();
    }

    void
    Update(const FloatT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(FloatT);
    }
};

template<>
struct MaxState<DoubleT, DoubleT> {
public:
    DoubleT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<DoubleT>::min();
    }

    void
    Update(const DoubleT *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(DoubleT);
    }
};


void
RegisterMaxFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "MAX";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                                   DataType(LogicalType::kBoolean),
                                                                                   DataType(LogicalType::kBoolean));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                                   DataType(LogicalType::kTinyInt),
                                                                                   DataType(LogicalType::kTinyInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<SmallIntT, SmallIntT>, SmallIntT, SmallIntT>(func_name,
                                                                                       DataType(LogicalType::kSmallInt),
                                                                                       DataType(LogicalType::kSmallInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<IntegerT, IntegerT>, IntegerT, IntegerT>(func_name,
                                                                                   DataType(LogicalType::kInteger),
                                                                                   DataType(LogicalType::kInteger));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kBigInt),
                                                                               DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                                                                                   DataType(LogicalType::kHugeInt),
                                                                                   DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<FloatT, FloatT>, FloatT, FloatT>(func_name,
                                                                           DataType(LogicalType::kFloat),
                                                                           DataType(LogicalType::kFloat));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MaxState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name,
                                                                               DataType(LogicalType::kDouble),
                                                                               DataType(LogicalType::kDouble));
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
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
