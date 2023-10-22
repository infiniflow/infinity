//
// Created by jinhai on 23-2-7.
//

module;

import stl;

export module expression_evaluator;

namespace infinity {

class BaseExpression;
class AggregateExpression;
class CaseExpression;
class CastExpression;
class ColumnExpression;
class ReferenceExpression;
class FunctionExpression;
class ValueExpression;
class InExpression;

class DataBlock;
class ColumnVector;

class ExpressionState;

export class ExpressionEvaluator {
public:
    void Init(const DataBlock *input_data_block);

    void Execute(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<AggregateExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CastExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<CaseExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ColumnExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<FunctionExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ValueExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<ReferenceExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

    void Execute(const SharedPtr<InExpression> &expr, SharedPtr<ExpressionState> &state, SharedPtr<ColumnVector> &output_column_vector);

private:
    const DataBlock *input_data_block_{};
    bool in_aggregate_{false};
};

} // namespace infinity
