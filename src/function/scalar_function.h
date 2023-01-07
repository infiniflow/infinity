//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function.h"
#include "expression/base_expression.h"
#include "executor/operation_state.h"
#include "common/utility/infinity_assert.h"
#include "common/column_vector/operator/unary_operator.h"
#include "common/column_vector/operator/binary_operator.h"
#include "common/types/null_value.h"
#include "storage/data_block.h"

#include <vector>

namespace infinity {

struct ScalarFunctionData {
    explicit
    ScalarFunctionData(ColumnVector* column_vector_ptr)
            : column_vector_ptr_(column_vector_ptr) {}

    bool all_converted_{true};
    ColumnVector* column_vector_ptr_{nullptr};
};


template<typename Operator>
struct UnaryOpDirectWrapper {
    template<typename SourceValueType, typename TargetValueType>
    inline static void
    Execute(SourceValueType input, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        return Operator::template Run<SourceValueType, TargetValueType>(input, result);
    }
};

template<typename Operator>
struct BinaryOpDirectWrapper {
    template<typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void
    Execute(LeftValueType left, RightValueType right, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        return Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result);
    }
};

template<typename Operator>
struct UnaryTryOpWrapper {
    template<typename SourceValueType, typename TargetValueType>
    inline static void
    Execute(SourceValueType input, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        if(Operator::template Run<SourceValueType, TargetValueType>(input, result)) {
            return ;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template<typename Operator>
struct BinaryTryOpWrapper {
    template<typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void
    Execute(LeftValueType left, RightValueType right, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        if(Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result)) {
            return ;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template<typename Operator>
struct UnaryOpDirectToVarlenWrapper {
    template<typename SourceValueType, typename TargetValueType>
    inline static void
    Execute(SourceValueType input, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        auto* function_data_ptr = (ScalarFunctionData*)(state_ptr);
        return Operator::template Run<SourceValueType, TargetValueType>(input, result, function_data_ptr->column_vector_ptr_);
    }
};

template<typename Operator>
struct BinaryOpDirectToVarlenWrapper {
    template<typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void
    Execute(LeftValueType left, RightValueType right, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        auto* function_data_ptr = (ScalarFunctionData*)(state_ptr);
        return Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result, function_data_ptr->column_vector_ptr_);
    }
};

template<typename Operator>
struct UnaryTryOpToVarlenWrapper {
    template<typename SourceValueType, typename TargetValueType>
    inline static void
    Execute(SourceValueType input, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        auto* function_data_ptr = (ScalarFunctionData*)(state_ptr);
        if(Operator::template Run<SourceValueType, TargetValueType>(input, result, function_data_ptr->column_vector_ptr_)) {
            return ;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template<typename Operator>
struct BinaryTryOpToVarlenWrapper {
    template<typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void
    Execute(LeftValueType left, RightValueType right, TargetValueType& result, Bitmask *nulls_ptr, size_t idx, void *state_ptr) {
        auto* function_data_ptr = (ScalarFunctionData*)(state_ptr);
        if(Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result, function_data_ptr->column_vector_ptr_)) {
            return ;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

using ScalarFunctionType = std::function<void(const DataBlock&, ColumnVector&)>;

class ScalarFunction : public Function {
public:
    explicit ScalarFunction(String name,
                            Vector<DataType> argument_types,
                            DataType return_type,
                            ScalarFunctionType function);

    void
    CastArgumentTypes(Vector<BaseExpression>& input_arguments);

    [[nodiscard]] const DataType&
    return_type() const { return return_type_; }

    [[nodiscard]] String
    ToString() const override;
public:

    Vector<DataType> parameter_types_;
    DataType return_type_;

    ScalarFunctionType function_;

public:
    // Unary function
    static void
    NoOpFunction(const DataBlock& input, ColumnVector& output);

    // Unary function without any failure.
    template<typename InputType, typename OutputType, typename Operation>
    static inline void
    UnaryFunction(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 1, "Unary function: input column count isn't one.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        UnaryOperator::Execute<InputType, OutputType, UnaryOpDirectWrapper<Operation>>(
                input.column_vectors[0],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Unary function with some failures such as overflow.
    template<typename InputType, typename OutputType, typename Operation>
    static inline void
    UnaryFunctionWithFailure(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 1, "Unary function: input column count isn't one.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        UnaryOperator::Execute<InputType, OutputType, UnaryTryOpWrapper<Operation>>(
                input.column_vectors[0],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Unary function result is varlen without any failure.
    template<typename InputType, typename OutputType, typename Operation>
    static inline void
    UnaryFunctionToVarlen(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 1, "Unary function: input column count isn't one.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        UnaryOperator::Execute<InputType, OutputType, UnaryOpDirectToVarlenWrapper<Operation>>(
                input.column_vectors[0],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Unary function result is varlen with some failures such as overflow.
    template<typename InputType, typename OutputType, typename Operation>
    static inline void
    UnaryFunctionToVarlenWithFailure(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 1, "Unary function: input column count isn't one.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        UnaryOperator::Execute<InputType, OutputType, UnaryTryOpToVarlenWrapper<Operation>>(
                input.column_vectors[0],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Binary function without any failure.
    template<typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void
    BinaryFunction(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryOpDirectWrapper<Operation>>(
                input.column_vectors[0],
                input.column_vectors[1],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Binary function with some failures such as overflow.
    template<typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void
    BinaryFunctionWithFailure(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryTryOpWrapper<Operation>>(
                input.column_vectors[0],
                input.column_vectors[1],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Binary function result is varlen without any failure.
    template<typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void
    BinaryFunctionToVarlen(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryOpDirectToVarlenWrapper<Operation>>(
                input.column_vectors[0],
                input.column_vectors[1],
                output,
                input.row_count(),
                nullptr,
                true);
    }

    // Binary function result is varlen with some failures such as overflow.
    template<typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void
    BinaryFunctionToVarlenWithFailure(const DataBlock& input, ColumnVector& output) {
        ExecutorAssert(input.column_count() == 2, "Binary function: input column count isn't two.");
        ExecutorAssert(input.Finalized(), "Input data block is finalized");
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryTryOpToVarlenWrapper<Operation>>(
                input.column_vectors[0],
                input.column_vectors[1],
                output,
                input.row_count(),
                nullptr,
                true);
    }
};
}

