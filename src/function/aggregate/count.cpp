//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct CountState {
public:
    u64 count_;

    void
    Initialize() {
        this->count_ = 0;
    }

    void
    Update(ValueType *__restrict input, SizeT idx) {
        this->count_ ++;
    }

    inline void
    ConstantUpdate(ValueType *__restrict input, SizeT idx, SizeT count) {
        this->count_ =+ count;
    }

    ResultType
    Finalize() {
        return count_;
    }

    inline static SizeT
    Size(DataType data_type) {
        return sizeof(ValueType);
    }
};

void
RegisterCountFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "COUNT";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction count_boolean
            = UnaryAggregate<CountState<BooleanT, BigIntT>, BooleanT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kBoolean),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(count_boolean);
    AggregateFunction count_tinyint
            = UnaryAggregate<CountState<TinyIntT, BigIntT>, TinyIntT, BigIntT>(func_name,
                                                                               DataType(LogicalType::kTinyInt),
                                                                               DataType(LogicalType::kBigInt));
    function_set_ptr->AddFunction(count_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
