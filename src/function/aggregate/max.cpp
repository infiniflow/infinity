//
// Created by JinHai on 2022/9/14.
//

#include <utility>

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct MaxState {
public:
    ValueType value_;

    void
    Initialize() {
        this->value_ = 0;
    }

    void
    Update(ValueType *__restrict input, SizeT idx) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(ValueType *__restrict input, SizeT idx, SizeT count) {
        value_ = value_ < input[idx] ? input[idx]: value_;
    }

    ResultType
    Finalize() {
        return value_;
    }

    inline static SizeT
    Size(DataType data_type) {
        return sizeof(ValueType);
    }
};

void
RegisterMaxFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "MAX";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction max_boolean
            = UnaryAggregate<MaxState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBoolean));
    function_set_ptr->AddFunction(max_boolean);
    AggregateFunction max_tinyint
            = UnaryAggregate<MaxState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kTinyInt));
    function_set_ptr->AddFunction(max_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
