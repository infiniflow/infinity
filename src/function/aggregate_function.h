//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "common/types/data_type.h"
#include "expression/base_expression.h"
#include "function.h"
#include "common/column_vector/column_vector.h"
#include <utility>
#include <vector>

namespace infinity {

using AggregateInitializeFuncType = std::function<void(ptr_t)>;
using AggregateUpdateFuncType = std::function<void(ptr_t, const SharedPtr<ColumnVector>&)>;
using AggregateFinalizeFuncType = std::function<void(ptr_t, ptr_t)>;

class AggregateOperation {
public:
    template<typename AggregateState>
    static inline void
    StateInitialize(ptr_t state) {
        ((AggregateState*)state)->Initialize();
    }

    template<typename AggregateState, typename InputType>
    static inline void
    StateUpdate(ptr_t state,
                const SharedPtr<ColumnVector>& input_column_vector) {
        // Loop execute state update according to the input column vector
        ((AggregateState*)state)->Update();
    }

    template<typename AggregateState, typename ResultType>
    static inline void
    StateFinalize(ptr_t state,
                  ptr_t result) {
        // Loop execute state update according to the input column vector
//        *((ResultType*)result) =
        ((AggregateState*)state)->Finalize();
    }
};

class AggregateFunction: public Function {
public:
    explicit
    AggregateFunction(String name,
                      DataType argument_type,
                      DataType return_type,
                      AggregateInitializeFuncType init_func,
                      AggregateUpdateFuncType update_func,
                      AggregateFinalizeFuncType finalize_func)
                      : Function(std::move(name), FunctionType::kAggregate),
                        argument_type_(std::move(argument_type)),
                        return_type_(std::move(return_type)),
                        init_func_(std::move(init_func)),
                        update_func_(std::move(update_func)),
                        finalize_func_(std::move(finalize_func))
    {}

    void
    CastArgumentTypes(BaseExpression& input_argument);

    [[nodiscard]] const DataType&
    return_type() const {
        return return_type_;
    }

    [[nodiscard]] std::string
    ToString() const override;

public:
    AggregateInitializeFuncType init_func_;
    AggregateUpdateFuncType update_func_;
    AggregateFinalizeFuncType finalize_func_;

    DataType argument_type_;
    DataType return_type_;

public:
    template <typename AggregateState, typename InputType, typename ResultType>
    static inline AggregateFunction
    UnaryAggregate(const String& name,
                   const DataType& input_type,
                   const DataType& return_type) {
        return AggregateFunction(
                name,
                input_type,
                return_type,
                AggregateOperation::StateInitialize<AggregateState>,
                AggregateOperation::StateUpdate<AggregateState, InputType>,
                AggregateOperation::StateFinalize<AggregateState, ResultType>);
    }
};
}

