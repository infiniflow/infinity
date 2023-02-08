//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

struct MinState {
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
    String func_name = "MIN";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction min_boolean
            = AggregateFunction::UnaryAggregate<MinState, BooleanT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(min_boolean);
    AggregateFunction min_tinyint
            = AggregateFunction::UnaryAggregate<MinState, TinyIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(min_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
