//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import function;
import function_data;
import column_vector;

import infinity_exception;
import base_expression;
import parser;

export module aggregate_function;

namespace infinity {

using AggregateInitializeFuncType = StdFunction<void(ptr_t)>;
using AggregateUpdateFuncType = StdFunction<void(ptr_t, const SharedPtr<ColumnVector> &)>;
using AggregateFinalizeFuncType = StdFunction<ptr_t(ptr_t)>;

class AggregateOperation {
public:
    template <typename AggregateState>
    static inline void StateInitialize(const ptr_t state) {
        ((AggregateState *)state)->Initialize();
    }

    template <typename AggregateState, typename InputType>
    static inline void StateUpdate(const ptr_t state, const SharedPtr<ColumnVector> &input_column_vector) {
        // Loop execute state update according to the input column vector

        switch (input_column_vector->vector_type_) {
            case ColumnVectorType::kFlat: {
                SizeT row_count = input_column_vector->Size();
                auto *input_ptr = (InputType *)(input_column_vector->data());
                for (SizeT idx = 0; idx < row_count; ++idx) {
                    ((AggregateState *)state)->Update(input_ptr, idx);
                }
                break;
            }
            case ColumnVectorType::kConstant: {
                auto *input_ptr = (InputType *)(input_column_vector->data());
                ((AggregateState *)state)->Update(input_ptr, 0);
                break;
            }
            case ColumnVectorType::kHeterogeneous: {
                Error<NotImplementException>("Heterogeneous type isn't implemented");
            }
            default: {
                Error<NotImplementException>("Other type of column vector isn't implemented");
            }
        }
    }

    template <typename AggregateState, typename ResultType>
    static inline ptr_t StateFinalize(const ptr_t state) {
        // Loop execute state update according to the input column vector
        ptr_t result = ((AggregateState *)state)->Finalize();
        return result;
    }
};

export class AggregateFunction : public Function {
public:
    explicit AggregateFunction(String name,
                               DataType argument_type,
                               DataType return_type,
                               SizeT state_size,
                               AggregateInitializeFuncType init_func,
                               AggregateUpdateFuncType update_func,
                               AggregateFinalizeFuncType finalize_func)
        : Function(Move(name), FunctionType::kAggregate), argument_type_(Move(argument_type)), return_type_(Move(return_type)),
          state_size_(state_size), init_func_(Move(init_func)), update_func_(Move(update_func)), finalize_func_(Move(finalize_func)) {
        state_data_ = SharedPtr<char[]>(new char[state_size_]());
    }

    void CastArgumentTypes(BaseExpression &input_argument);

    [[nodiscard]] const DataType &return_type() const { return return_type_; }

    [[nodiscard]] String ToString() const override;

    [[nodiscard]] ptr_t GetState() const { return state_data_.get(); }

public:
    AggregateInitializeFuncType init_func_;
    AggregateUpdateFuncType update_func_;
    AggregateFinalizeFuncType finalize_func_;

    DataType argument_type_;
    DataType return_type_;

    SharedPtr<char[]> state_data_{nullptr};
    SizeT state_size_{};
};

export template <typename AggregateState, typename InputType, typename ResultType>
inline AggregateFunction UnaryAggregate(const String &name, const DataType &input_type, const DataType &return_type) {
    return AggregateFunction(name,
                             input_type,
                             return_type,
                             AggregateState::Size(input_type),
                             AggregateOperation::StateInitialize<AggregateState>,
                             AggregateOperation::StateUpdate<AggregateState, InputType>,
                             AggregateOperation::StateFinalize<AggregateState, ResultType>);
}
} // namespace infinity
