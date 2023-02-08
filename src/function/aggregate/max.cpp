//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

struct MaxState {
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
RegisterMaxFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "MAX";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction max_boolean
            = AggregateFunction::UnaryAggregate<MaxState, BooleanT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(max_boolean);
    AggregateFunction max_tinyint
            = AggregateFunction::UnaryAggregate<MaxState, TinyIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(max_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
