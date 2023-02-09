//
// Created by JinHai on 2022/9/14.
//

#include "count.h"
#include "function/aggregate_function.h"
#include "function/aggregate_function_set.h"

namespace infinity {

template<typename ValueType, typename ResultType>
struct AvgState {
public:
    i64 value_{};
    u64 count_{};

    inline void
    Initialize() {
        this->value_ = 0;
        this->count_ = 0;
    }

    inline void
    Update(ValueType *__restrict input, SizeT idx) {
        this->count_ ++;
        value_ += input[idx];
    }

    inline void
    ConstantUpdate(ValueType *__restrict input, SizeT idx, SizeT count) {
        this->count_ =+ count;
        value_ += (input[idx] * count);
    }

    inline ResultType
    Finalize() {
        return value_ / (ResultType)count_;
    }

    inline static SizeT
    Size(DataType data_type) {
        return sizeof(ValueType);
    }
};

void
RegisterAvgFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "AVG";

    SharedPtr<AggregateFunctionSet> function_set_ptr = MakeShared<AggregateFunctionSet>(func_name);

    AggregateFunction avg_tinyint
            = UnaryAggregate<AvgState<TinyIntT, DoubleT>, TinyIntT, DoubleT>(func_name,
                                                                             DataType(LogicalType::kTinyInt),
                                                                             DataType(LogicalType::kDouble));
    function_set_ptr->AddFunction(avg_tinyint);
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
