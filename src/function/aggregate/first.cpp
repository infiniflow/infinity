//
// Created by jinhai on 23-4-7.
//

#include "first.h"

#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template <typename ValueType, typename ResultType>
struct FirstState {
public:
    ValueType value_;
    bool is_set_{false};

    inline void Initialize() { is_set_ = false; }

    inline void Update(const ValueType *__restrict input, SizeT idx) {
        if (is_set_)
            return;

        is_set_ = true;
        value_ = input[idx];
    }

    inline void ConstantUpdate(const ValueType *__restrict input, SizeT idx, SizeT count) {
        if (is_set_)
            return;

        is_set_ = true;
        value_ = input[idx];
    }

    [[nodiscard]] inline ptr_t Finalize() const { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(FirstState<ValueType, ResultType>); }
};

template <>
struct FirstState<VarcharT, VarcharT> {
public:
    VarcharT value_{};
    bool is_set_{false};

    inline void Initialize() { is_set_ = false; }

    inline void Update(const VarcharT *__restrict input, SizeT idx) {
        if (is_set_)
            return;

        is_set_ = true;
        // This assignment will call varchar deep copy
        value_ = input[idx];
    }

    inline void ConstantUpdate(const VarcharT *__restrict input, SizeT idx, SizeT count) {
        if (is_set_)
            return;

        is_set_ = true;
        // This assignment will call varchar deep copy
        value_ = input[idx];
    }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(FirstState<VarcharT, VarcharT>); }
};

template <>
struct FirstState<PathT, PathT> {
public:
    PathT value_{};
    bool is_set_{false};

    inline void Initialize() { is_set_ = false; }

    inline void Update(const PathT *__restrict input, SizeT idx) {
        if (is_set_)
            return;

        is_set_ = true;
        // This assignment will call varchar deep copy
        value_ = input[idx];
    }

    inline void ConstantUpdate(const PathT *__restrict input, SizeT idx, SizeT count) {
        if (is_set_)
            return;

        is_set_ = true;
        // This assignment will call varchar deep copy
        value_ = input[idx];
    }

    inline ptr_t Finalize() { return (ptr_t)&value_; }

    inline static SizeT Size(const DataType &data_type) { return sizeof(FirstState<PathT, PathT>); }
};

void RegisterFirstFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "FIRST";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                                                              DataType(LogicalType::kBoolean),
                                                                                                              DataType(LogicalType::kBoolean));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                                                              DataType(LogicalType::kTinyInt),
                                                                                                              DataType(LogicalType::kTinyInt));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<SmallIntT, SmallIntT>, SmallIntT, SmallIntT>(func_name,
                                                                                                                  DataType(LogicalType::kSmallInt),
                                                                                                                  DataType(LogicalType::kSmallInt));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<IntegerT, IntegerT>, IntegerT, IntegerT>(func_name,
                                                                                                              DataType(LogicalType::kInteger),
                                                                                                              DataType(LogicalType::kInteger));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<BigIntT, BigIntT>, BigIntT, BigIntT>(func_name, DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<HugeIntT, HugeIntT>, HugeIntT, HugeIntT>(func_name,
                                                                                                              DataType(LogicalType::kHugeInt),
                                                                                                              DataType(LogicalType::kHugeInt));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<FloatT, FloatT>, FloatT, FloatT>(func_name, DataType(LogicalType::kFloat), DataType(LogicalType::kFloat));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<DoubleT, DoubleT>, DoubleT, DoubleT>(func_name, DataType(LogicalType::kDouble), DataType(LogicalType::kDouble));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<DecimalT, DecimalT>, DecimalT, DecimalT>(func_name,
                                                                                                              DataType(LogicalType::kDecimal),
                                                                                                              DataType(LogicalType::kDecimal));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<VarcharT, VarcharT>, VarcharT, VarcharT>(func_name,
                                                                                                              DataType(LogicalType::kVarchar),
                                                                                                              DataType(LogicalType::kVarchar));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<DateT, DateT>, DateT, DateT>(func_name, DataType(LogicalType::kDate), DataType(LogicalType::kDate));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<TimeT, TimeT>, TimeT, TimeT>(func_name, DataType(LogicalType::kTime), DataType(LogicalType::kTime));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<DateTimeT, DateTimeT>, DateTimeT, DateTimeT>(func_name,
                                                                                                                  DataType(LogicalType::kDateTime),
                                                                                                                  DataType(LogicalType::kDateTime));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<TimestampT, TimestampT>, TimestampT, TimestampT>(func_name,
                                                                                       DataType(LogicalType::kTimestamp),
                                                                                       DataType(LogicalType::kTimestamp));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<IntervalT, IntervalT>, IntervalT, IntervalT>(func_name,
                                                                                                                  DataType(LogicalType::kInterval),
                                                                                                                  DataType(LogicalType::kInterval));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<PointT, PointT>, PointT, PointT>(func_name, DataType(LogicalType::kPoint), DataType(LogicalType::kPoint));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<LineT, LineT>, LineT, LineT>(func_name, DataType(LogicalType::kLine), DataType(LogicalType::kLine));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function = UnaryAggregate<FirstState<LineSegT, LineSegT>, LineSegT, LineSegT>(func_name,
                                                                                                              DataType(LogicalType::kLineSeg),
                                                                                                              DataType(LogicalType::kLineSeg));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<BoxT, BoxT>, BoxT, BoxT>(func_name, DataType(LogicalType::kBox), DataType(LogicalType::kBox));
        function_set_ptr->AddFunction(first_function);
    }

    {
        AggregateFunction first_function =
            UnaryAggregate<FirstState<CircleT, CircleT>, CircleT, CircleT>(func_name, DataType(LogicalType::kCircle), DataType(LogicalType::kCircle));
        function_set_ptr->AddFunction(first_function);
    }

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity