//
// Created by JinHai on 2022/10/26.
//

#include "common/utility/infinity_assert.h"
#include "expression_state.h"

namespace infinity {

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<BaseExpression> &expression) {

    switch(expression->type()) {
        case ExpressionType::kAggregate:
            return CreateState(std::static_pointer_cast<AggregateExpression>(expression));
        case ExpressionType::kCast:
            return CreateState(std::static_pointer_cast<CastExpression>(expression));
        case ExpressionType::kCase:
            return CreateState(std::static_pointer_cast<CaseExpression>(expression));
        case ExpressionType::kFunction:
            return CreateState(std::static_pointer_cast<FunctionExpression>(expression));
        case ExpressionType::kValue:
            return CreateState(std::static_pointer_cast<ValueExpression>(expression));
        case ExpressionType::kReference:
            return CreateState(std::static_pointer_cast<ReferenceExpression>(expression));
        case ExpressionType::kKnn: {
            ExecutorError("Unexpected expression type: KNN");
        }
        default:
            ExecutorError("Unknown expression type: " + expression->Name());
    }

}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<AggregateExpression>& agg_expr) {
    ExecutorAssert(agg_expr->arguments().size() == 1, "Aggregate function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(agg_expr->arguments()[0]);

    // Aggregate function will only have one output value.
    result->column_vector_ = MakeShared<ColumnVector>(MakeShared<DataType>(agg_expr->Type()));
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);
//    result->output_data_block_.Init({agg_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CaseExpression>& case_expr) {

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();

    Vector<CaseCheck>& case_checks = case_expr->CaseExpr();
    for(auto& case_check: case_checks) {
        result->AddChild(case_check.when_expr_);
        result->AddChild(case_check.then_expr_);
    }
    result->AddChild(case_expr->ElseExpr());

    result->column_vector_ = MakeShared<ColumnVector>(MakeShared<DataType>(case_expr->Type()));
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<CastExpression>& cast_expr) {
    ExecutorAssert(cast_expr->arguments().size() == 1, "Cast function arguments error.");

    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    result->AddChild(cast_expr->arguments()[0]);

//    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);
//    for(SizeT idx = 0; idx < block_count; ++ idx) {
//        if(result->Children()[0]->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
//            result_column_vector_type[idx] = ColumnVectorType::kFlat;
//        }
//    }

    result->column_vector_ = MakeShared<ColumnVector>(MakeShared<DataType>(cast_expr->Type()));
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

//    result->output_data_block_.Init({cast_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ReferenceExpression>& column_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> column_data_type = MakeShared<DataType>(column_expr->Type());
    result->column_vector_ = MakeShared<ColumnVector>(column_data_type);
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

//    result->output_data_block_.Init({column});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<FunctionExpression>& function_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> function_expr_data_type = MakeShared<DataType>(function_expr->Type());

    for(auto& arg : function_expr->arguments()) {
        result->AddChild(arg);
    }

//    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);
//
//    for(SizeT idx = 0; idx < block_count; ++ idx) {
//        for(auto& child_state: result->Children()) {
//            // Once a child column vector isn't kConstant, the result column vector will be kFlat;
//            if(child_state->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
//                result_column_vector_type[idx] = ColumnVectorType::kFlat;
//            }
//        }
//    }

    result->column_vector_ = MakeShared<ColumnVector>(function_expr_data_type);
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

//    result->output_data_block_.Init({function_expr->Type()});
    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<ValueExpression>& value_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> value_data_type = MakeShared<DataType>(value_expr->Type());

    result->column_vector_ = MakeShared<ColumnVector>(value_data_type);
    result->column_vector_->Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    value_expr->AppendToChunk(result->column_vector_);

    return result;
}

SharedPtr<ExpressionState>
ExpressionState::CreateState(const SharedPtr<InExpression>& in_expr) {
    SharedPtr<ExpressionState> result = MakeShared<ExpressionState>();
    SharedPtr<DataType> in_expr_data_type = MakeShared<DataType>(in_expr->Type());

    result->AddChild(in_expr->left_operand());

    for(auto& argument_expr: in_expr->arguments()) {
        result->AddChild(argument_expr);
    }

//    Vector<ColumnVectorType> result_column_vector_type(block_count, ColumnVectorType::kConstant);
//    for(SizeT idx = 0; idx < block_count; ++ idx) {
//        if(result->Children()[0]->OutputColumnVectors()[idx]->vector_type() != ColumnVectorType::kConstant) {
//            result_column_vector_type[idx] = ColumnVectorType::kFlat;
//        }
//    }

    result->column_vector_ = MakeShared<ColumnVector>(in_expr_data_type);
    result->column_vector_->Initialize(ColumnVectorType::kFlat, DEFAULT_VECTOR_SIZE);

    return result;
}

void
ExpressionState::AddChild(const SharedPtr<BaseExpression> &expression) {
    children_.emplace_back(CreateState(expression));
}

}

