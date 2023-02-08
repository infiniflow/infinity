//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

struct AvgState {
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
RegisterAvgFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "AVG";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction avg_tinyint
            = AggregateFunction::UnaryAggregate<AvgState, TinyIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kDouble));
    function_set_ptr->AddFunction(avg_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
