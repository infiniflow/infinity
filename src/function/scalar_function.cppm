//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import function;
import function_data;
import column_vector;
import infinity_assert;
import infinity_exception;
import parser;
import data_block;
import bitmask;
import unary_operator;
import binary_operator;
import ternary_operator;
import base_expression;
import null_value;

export module scalar_function;

namespace infinity {

//class BaseExpression;

struct ScalarFunctionData {
    explicit ScalarFunctionData(ColumnVector *column_vector_ptr) : column_vector_ptr_(column_vector_ptr) {}

    bool all_converted_{true};
    ColumnVector *column_vector_ptr_{nullptr};
};

template <typename Operator>
struct UnaryOpDirectWrapper {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        return Operator::template Run<SourceValueType, TargetValueType>(input, result);
    }
};

template <typename Operator>
struct BinaryOpDirectWrapper {
    template <typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void Execute(LeftValueType left, RightValueType right, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        return Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result);
    }
};

template <typename Operator>
struct TernaryOpDirectWrapper {
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType>
    inline static void
    Execute(FirstType first, SecondType second, ThirdType third, ResultType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        return Operator::template Run<FirstType, SecondType, ThirdType, ResultType>(first, second, third, result);
    }
};

template <typename Operator>
struct UnaryTryOpWrapper {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        if (Operator::template Run<SourceValueType, TargetValueType>(input, result)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template <typename Operator>
struct BinaryTryOpWrapper {
    template <typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void Execute(LeftValueType left, RightValueType right, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        if (Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template <typename Operator>
struct TernaryTryOpWrapper {
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType>
    inline static void
    Execute(FirstType first, SecondType second, ThirdType third, ResultType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        if (Operator::template Run<FirstType, SecondType, ThirdType, ResultType>(first, second, third, result)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<ResultType>();
    }
};

template <typename Operator>
struct UnaryOpDirectToVarlenWrapper {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        return Operator::template Run<SourceValueType, TargetValueType>(input, result, function_data_ptr->column_vector_ptr_);
    }
};

template <typename Operator>
struct BinaryOpDirectToVarlenWrapper {
    template <typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void Execute(LeftValueType left, RightValueType right, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        return Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result, function_data_ptr->column_vector_ptr_);
    }
};

template <typename Operator>
struct TernaryOpDirectToVarlenWrapper {
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType>
    inline static void
    Execute(FirstType first, SecondType second, ThirdType third, ResultType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        return Operator::template Run<FirstType, SecondType, ThirdType, ResultType>(first,
                                                                                    second,
                                                                                    third,
                                                                                    result,
                                                                                    function_data_ptr->column_vector_ptr_);
    }
};

template <typename Operator>
struct UnaryTryOpToVarlenWrapper {
    template <typename SourceValueType, typename TargetValueType>
    inline static void Execute(SourceValueType input, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        if (Operator::template Run<SourceValueType, TargetValueType>(input, result, function_data_ptr->column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template <typename Operator>
struct BinaryTryOpToVarlenWrapper {
    template <typename LeftValueType, typename RightValueType, typename TargetValueType>
    inline static void Execute(LeftValueType left, RightValueType right, TargetValueType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        if (Operator::template Run<LeftValueType, RightValueType, TargetValueType>(left, right, result, function_data_ptr->column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<TargetValueType>();
    }
};

template <typename Operator>
struct TernaryTryOpToVarlenWrapper {
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType>
    inline static void
    Execute(FirstType first, SecondType second, ThirdType third, ResultType &result, Bitmask *nulls_ptr, SizeT idx, void *state_ptr) {
        auto *function_data_ptr = (ScalarFunctionData *)(state_ptr);
        if (Operator::template Run<FirstType, SecondType, ThirdType, ResultType>(first,
                                                                                 second,
                                                                                 third,
                                                                                 result,
                                                                                 function_data_ptr->column_vector_ptr_)) {
            return;
        }

        nulls_ptr->SetFalse(idx);
        result = NullValue<ResultType>();
    }
};

using ScalarFunctionType = StdFunction<void(const DataBlock &, SharedPtr<ColumnVector> &)>;

export class ScalarFunction : public Function {
public:
    explicit ScalarFunction(String name, Vector<DataType> argument_types, DataType return_type, ScalarFunctionType function);

    void CastArgumentTypes(Vector<BaseExpression> &input_arguments);

    [[nodiscard]] const DataType &return_type() const { return return_type_; }

    [[nodiscard]] String ToString() const override;

public:
    Vector<DataType> parameter_types_;
    DataType return_type_;

    ScalarFunctionType function_;

public:
    // Unary function
    static void NoOpFunction(const DataBlock &input, SharedPtr<ColumnVector> &output);

    // Unary function without any failure.
    template <typename InputType, typename OutputType, typename Operation>
    static inline void UnaryFunction(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 1, "Unary function: input column count isn't one.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        UnaryOperator::Execute<InputType, OutputType, UnaryOpDirectWrapper<Operation>>(input.column_vectors[0],
                                                                                       output,
                                                                                       input.row_count(),
                                                                                       nullptr,
                                                                                       true);
    }

    // Unary function with some failures such as overflow.
    template <typename InputType, typename OutputType, typename Operation>
    static inline void UnaryFunctionWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 1, "Unary function: input column count isn't one.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        UnaryOperator::Execute<InputType, OutputType, UnaryTryOpWrapper<Operation>>(input.column_vectors[0],
                                                                                    output,
                                                                                    input.row_count(),
                                                                                    nullptr,
                                                                                    true);
    }

    // Unary function result is varlen without any failure.
    template <typename InputType, typename OutputType, typename Operation>
    static inline void UnaryFunctionToVarlen(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 1, "Unary function: input column count isn't one.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        UnaryOperator::Execute<InputType, OutputType, UnaryOpDirectToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                               output,
                                                                                               input.row_count(),
                                                                                               &function_data,
                                                                                               true);
    }

    // Unary function result is varlen with some failures such as overflow.
    template <typename InputType, typename OutputType, typename Operation>
    static inline void UnaryFunctionToVarlenWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 1, "Unary function: input column count isn't one.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        UnaryOperator::Execute<InputType, OutputType, UnaryTryOpToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                            output,
                                                                                            input.row_count(),
                                                                                            &function_data,
                                                                                            true);
    }

    // Binary function without any failure.
    template <typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void BinaryFunction(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 2, "Binary function: input column count isn't two.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryOpDirectWrapper<Operation>>(input.column_vectors[0],
                                                                                                   input.column_vectors[1],
                                                                                                   output,
                                                                                                   input.row_count(),
                                                                                                   nullptr,
                                                                                                   true);
    }

    // Binary function with some failures such as overflow.
    template <typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void BinaryFunctionWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 2, "Binary function: input column count isn't two.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryTryOpWrapper<Operation>>(input.column_vectors[0],
                                                                                                input.column_vectors[1],
                                                                                                output,
                                                                                                input.row_count(),
                                                                                                nullptr,
                                                                                                true);
    }

    // Binary function result is varlen without any failure.
    template <typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void BinaryFunctionToVarlen(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 2, "Binary function: input column count isn't two.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryOpDirectToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                                           input.column_vectors[1],
                                                                                                           output,
                                                                                                           input.row_count(),
                                                                                                           &function_data,
                                                                                                           true);
    }

    // Binary function result is varlen with some failures such as overflow.
    template <typename LeftType, typename RightType, typename OutputType, typename Operation>
    static inline void BinaryFunctionToVarlenWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 2, "Binary function: input column count isn't two.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        BinaryOperator::Execute<LeftType, RightType, OutputType, BinaryTryOpToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                                        input.column_vectors[1],
                                                                                                        output,
                                                                                                        input.row_count(),
                                                                                                        &function_data,
                                                                                                        true);
    }

    // Ternary function without any failure.
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operation>
    static inline void TernaryFunction(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 3, "Ternary function: input column count isn't three.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        TernaryOperator::Execute<FirstType, SecondType, ThirdType, ResultType, TernaryOpDirectWrapper<Operation>>(input.column_vectors[0],
                                                                                                                  input.column_vectors[1],
                                                                                                                  input.column_vectors[2],
                                                                                                                  output,
                                                                                                                  input.row_count(),
                                                                                                                  nullptr,
                                                                                                                  true);
    }

    // Ternary function with some failures such as overflow.
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operation>
    static inline void TernaryFunctionWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 3, "Ternary function: input column count isn't three.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        TernaryOperator::Execute<FirstType, SecondType, ThirdType, ResultType, TernaryTryOpWrapper<Operation>>(input.column_vectors[0],
                                                                                                               input.column_vectors[1],
                                                                                                               input.column_vectors[2],
                                                                                                               output,
                                                                                                               input.row_count(),
                                                                                                               nullptr,
                                                                                                               true);
    }

    // Ternary function result is varlen without any failure.
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operation>
    static inline void TernaryFunctionToVarlen(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 3, "Ternary function: input column count isn't three.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        TernaryOperator::Execute<FirstType, SecondType, ThirdType, ResultType, TernaryOpDirectToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                                                          input.column_vectors[1],
                                                                                                                          input.column_vectors[2],
                                                                                                                          output,
                                                                                                                          input.row_count(),
                                                                                                                          &function_data,
                                                                                                                          true);
    }

    // Ternary function result is varlen with some failures such as overflow.
    template <typename FirstType, typename SecondType, typename ThirdType, typename ResultType, typename Operation>
    static inline void TernaryFunctionToVarlenWithFailure(const DataBlock &input, SharedPtr<ColumnVector> &output) {
        Assert<ExecutorException>(input.column_count() == 3, "Ternary function: input column count isn't three.", __FILE_NAME__, __LINE__);
        Assert<ExecutorException>(input.Finalized(), "Input data block is finalized", __FILE_NAME__, __LINE__);
        ScalarFunctionData function_data(output.get());
        TernaryOperator::Execute<FirstType, SecondType, ThirdType, ResultType, TernaryTryOpToVarlenWrapper<Operation>>(input.column_vectors[0],
                                                                                                                       input.column_vectors[1],
                                                                                                                       input.column_vectors[2],
                                                                                                                       output,
                                                                                                                       input.row_count(),
                                                                                                                       &function_data,
                                                                                                                       true);
    }
};
} // namespace infinity
