//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

struct SumState {
public:
    u64 count;

    void Initialize() {
        this->count = 0;
    }

    void Update() {

    }

    void Finalize() {
    }
};

void
RegisterSumFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "SUM";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction sum_boolean
            = AggregateFunction::UnaryAggregate<SumState, BooleanT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(sum_boolean);
    AggregateFunction sum_tinyint
            = AggregateFunction::UnaryAggregate<SumState, TinyIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(sum_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
