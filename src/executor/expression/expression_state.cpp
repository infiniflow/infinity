//
// Created by JinHai on 2022/10/26.
//

#include "common/utility/infinity_assert.h"
#include "expression_state.h"

namespace infinity {

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BaseExpression> &expression, SizeT block_count) {

    switch(expression->type()) {
        case ExpressionType::kAggregate:
            return CreateState(std::static_pointer_cast<AggregateExpression>(expression), block_count);
        case ExpressionType::kCast:
            return CreateState(std::static_pointer_cast<CastExpression>(expression), block_count);
        case ExpressionType::kCase:
            return CreateState(std::static_pointer_cast<CaseExpression>(expression), block_count);
        case ExpressionType::kFunction:
            return CreateState(std::static_pointer_cast<FunctionExpression>(expression), block_count);
        case ExpressionType::kValue:
            return CreateState(std::static_pointer_cast<ValueExpression>(expression), block_count);
        case ExpressionType::kReference:
            return CreateState(std::static_pointer_cast<ReferenceExpression>(expression), block_count);
        default:
            ExecutorError("Unknown expression type: " + expression->Name());
    }

}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<AggregateExpression>& agg_expr, SizeT block_count) {
    ExecutorAssert(agg_expr->arguments().size() == 1, "Aggregate function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(agg_expr->arguments()[0], block_count);

    // Aggregate function will only have one output value.
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(MakeShared<DataType>(agg_expr->Type()));
    column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
    result->column_vectors_.emplace_back(column);
//    result->output_data_block_.Init({agg_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CaseExpression>& case_expr, SizeT block_count) {

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    Vector<CaseCheck>& case_checks = case_expr->CaseExpr();
    for(auto& case_check: case_checks) {
        result->AddChild(case_check.when_expr_, block_count);
        result->AddChild(case_check.then_expr_, block_count);
    }
    result->AddChild(case_expr->ElseExpr(), block_count);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(MakeShared<DataType>(case_expr->Type()));
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({case_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CastExpression>& cast_expr, SizeT block_count) {
    ExecutorAssert(cast_expr->arguments().size() == 1, "Cast function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(cast_expr->arguments()[0], block_count);

    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        if(result->Children()[0]->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
            result_column_vector_type[idx] = ColumnVectorType::kFlat;
        }
    }

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(MakeShared<DataType>(cast_expr->Type()));
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({cast_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ReferenceExpression>& column_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> column_data_type = MakeShared<DataType>(column_expr->Type());
    SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(column_data_type);

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(column_data_type);
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
//    result->output_data_block_.Init({column});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<FunctionExpression>& function_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> function_expr_data_type = MakeShared<DataType>(function_expr->Type());

    for(auto& arg : function_expr->arguments()) {
        result->AddChild(arg, block_count);
    }

    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);

    for(SizeT idx = 0; idx < block_count; ++ idx) {
        for(auto& child_state: result->Children()) {
            // Once a child column vector isn't kConstant, the result column vector will be kFlat;
            if(child_state->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
                result_column_vector_type[idx] = ColumnVectorType::kFlat;
            }
        }
    }

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(function_expr_data_type);
        column->Initialize(result_column_vector_type[idx], DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }

//    result->output_data_block_.Init({function_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ValueExpression>& value_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> value_data_type = MakeShared<DataType>(value_expr->Type());

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(value_data_type);
        column->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        value_expr->AppendToChunk(column);
        result->column_vectors_.emplace_back(column);
    }

    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<InExpression>& in_expr, SizeT block_count) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> in_expr_data_type = MakeShared<DataType>(in_expr->Type());

    result->AddChild(in_expr->left_operand(), block_count);

    for(auto& argument_expr: in_expr->arguments()) {
        result->AddChild(argument_expr, block_count);
    }

    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        if(result->Children()[0]->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
            result_column_vector_type[idx] = ColumnVectorType::kFlat;
        }
    }

    result->column_vectors_.reserve(block_count);
    for(SizeT idx = 0; idx < block_count; ++ idx) {
        SharedPtr<ColumnVector> column = MakeShared<ColumnVector>(in_expr_data_type);
        column->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
        result->column_vectors_.emplace_back(column);
    }
    return result;
}

void
ExpressionState::AddChild(const SharedPtr<BaseExpression> &expression, SizeT block_count) {
    children_.emplace_back(CreateState(expression, block_count));
}

}

