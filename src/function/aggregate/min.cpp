//
// Created by JinHai on 2022/9/14.
//

#include "min.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct MinState {
public:
    ValueType value_;

    void
    Initialize() {
        NotImplementError("Initialize min state.")
    }

    void
    Update(const ValueType *__restrict input, SizeT idx) {
        NotImplementError("Update min state.")
    }

    inline void
    ConstantUpdate(const ValueType *__restrict input, SizeT idx, SizeT count) {
        NotImplementError("Constant update min state.")
    }

    [[nodiscard]] ptr_t
    Finalize() const {
        NotImplementError("Finalize min state.")
    }

    inline static SizeT
    Size(const DataType& data_type) {
        NotImplementError("Min state type size: " + data_type.ToString())
    }
};

template<>
struct MinState<BooleanT, BooleanT> {
public:
    BooleanT value_;

    void
    Initialize() {
        this->value_ = true;
    }

    void
    Update(const BooleanT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const BooleanT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline ptr_t
    Finalize() {
        return (ptr_t)&value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return 1;
    }
};

template<>
struct MinState<TinyIntT, TinyIntT> {
public:
    TinyIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<TinyIntT>::max();
    }

    void
    Update(const TinyIntT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const TinyIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<SmallIntT, SmallIntT> {
public:
    SmallIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<SmallIntT>::max();
    }

    void
    Update(const SmallIntT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const SmallIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<IntegerT, IntegerT> {
public:
    IntegerT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<IntegerT>::max();
    }

    void
    Update(const IntegerT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const IntegerT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<BigIntT, BigIntT> {
public:
    BigIntT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<BigIntT>::max();
    }

    void
    Update(const BigIntT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const BigIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<HugeIntT, HugeIntT> {
public:
    HugeIntT value_;

    void
    Initialize() {
        this->value_.lower = std::numeric_limits<BigIntT>::max();
        this->value_.upper = std::numeric_limits<BigIntT>::max();
    }

    void
    Update(const HugeIntT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const HugeIntT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<FloatT, FloatT> {
public:
    FloatT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<FloatT>::max();
    }

    void
    Update(const FloatT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const FloatT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
struct MinState<DoubleT, DoubleT> {
public:
    DoubleT value_;

    void
    Initialize() {
        this->value_ = std::numeric_limits<DoubleT>::max();
    }

    void
    Update(const DoubleT *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx] : value_;
    }

    inline void
    ConstantUpdate(const DoubleT *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx] : value_;
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
RegisterMinFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "MIN";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                DataType(LogicalType::kBoolean),
                DataType(LogicalType::kBoolean));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                DataType(LogicalType::kTinyInt),
                DataType(LogicalType::kTinyInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<SmallIntT, SmallIntT>, SmallIntT, SmallIntT>(func_name,
                DataType(LogicalType::kSmallInt),
                DataType(LogicalType::kSmallInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<IntegerT, IntegerT>, IntegerT, IntegerT>(func_name,
                DataType(LogicalType::kInteger),
                DataType(LogicalType::kInteger));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name,
                DataType(LogicalType::kBigInt),
                DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                DataType(LogicalType::kHugeInt),
                DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<FloatT, FloatT>, FloatT, FloatT>(func_name,
                DataType(LogicalType::kFloat),
                DataType(LogicalType::kFloat));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                                  = UnaryAggregate<MinState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name,
                DataType(LogicalType::kDouble),
                DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(max_function);
    }
#if 0
    {
        AggregateFunction max_function
                = UnaryAggregate<MinState<DecimalT, DecimalT>, DecimalT, DecimalT>(func_name,
                                                                                   DataType(LogicalType::kDecimal),
                                                                                   DataType(LogicalType::kDecimal));
        function_set_ptr->AddFunction(max_function);
    }
    {
        AggregateFunction max_function
                = UnaryAggregate<MinState<VarcharT, VarcharT>, VarcharT, VarcharT>(func_name,
                                                                                   DataType(LogicalType::kVarchar),
                                                                                   DataType(LogicalType::kVarchar));
        function_set_ptr->AddFunction(max_function);
    }
#endif
    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
