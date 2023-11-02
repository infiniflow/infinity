//
// Created by JinHai on 2022/7/28.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import data_table;
import expression_evaluator;
import expression_selector;

export module physical_filter;

namespace infinity {

export class PhysicalFilter : public PhysicalOperator {
public:
    explicit PhysicalFilter(u64 id, SharedPtr<PhysicalOperator> left, SharedPtr<BaseExpression> condition)
        : PhysicalOperator(PhysicalOperatorType::kFilter, Move(left), nullptr, id), condition_(Move(condition)) {}

    ~PhysicalFilter() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    inline const SharedPtr<BaseExpression> &condition() const { return condition_; }

private:
    SharedPtr<BaseExpression> condition_;

    ExpressionEvaluator executor_;
    ExpressionSelector selector_;

    SharedPtr<DataTable> input_table_{};
};

} // namespace infinity
