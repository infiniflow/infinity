//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct MinState {
public:
    ValueType value_;

    inline void
    Initialize() {
        this->value_ = 0;
    }

    inline void
    Update(ValueType *__restrict input, SizeT idx) {
        value_ = input[idx] < value_ ? input[idx]: value_;
    }

    inline void
    ConstantUpdate(ValueType *__restrict input, SizeT idx, SizeT count) {
        value_ = input[idx] < value_ ? input[idx]: value_;
    }

    inline ResultType
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
    String func_name = "MIN";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction min_boolean
            = UnaryAggregate<MinState<BooleanT, BooleanT>, BooleanT, BooleanT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBoolean));
    function_set_ptr->AddFunction(min_boolean);
    AggregateFunction min_tinyint
            = UnaryAggregate<MinState<TinyIntT, TinyIntT>, TinyIntT, TinyIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kTinyInt));
    function_set_ptr->AddFunction(min_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
