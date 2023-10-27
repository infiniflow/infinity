//
// Created by JinHai on 2022/10/26.
//

module;

import stl;

export module expression_state;

namespace infinity {

class BaseExpression;
class AggregateExpression;
class CaseExpression;
class CastExpression;
class ReferenceExpression;
class FunctionExpression;
class ValueExpression;
class InExpression;
class ColumnVector;

export class ExpressionState {
public:
    // Static functions
    static SharedPtr<ExpressionState> CreateState(const SharedPtr<BaseExpression> &expression);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<AggregateExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CaseExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<CastExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ReferenceExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<FunctionExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<ValueExpression> &agg_expr);

    static SharedPtr<ExpressionState> CreateState(const SharedPtr<InExpression> &in_expr);

public:
    void AddChild(const SharedPtr<BaseExpression> &expression);

    Vector<SharedPtr<ExpressionState>> &Children() { return children_; }

    SharedPtr<ColumnVector> &OutputColumnVector() { return column_vector_; }

private:
    Vector<SharedPtr<ExpressionState>> children_;
    String name_;

    // output blocks and each block have one output column;
    SharedPtr<ColumnVector> column_vector_;
};

} // namespace infinity
