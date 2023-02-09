//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct SumState {
public:
    i64 value_;

    inline void
    Initialize() {
        this->value_ = 0;
    }

    inline void
    Update(ValueType *__restrict input, SizeT idx) {
        value_ += input[idx];
    }

    inline void
    ConstantUpdate(ValueType *__restrict input, SizeT idx, SizeT count) {
        value_ += input[idx] * count;
    }

    inline ResultType
    Finalize() {
        return value_;
    }

    inline static SizeT
    Size(const DataType& data_type) {
        return sizeof(ValueType);
    }
};

void
RegisterSumFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "SUM";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction sum_tinyint
            = UnaryAggregate<SumState<TinyIntT, BigIntT>, TinyIntT, BigIntT>(func_name,
                                                                             DataType(LogicalType::kTinyInt),
                                                                             DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(sum_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
