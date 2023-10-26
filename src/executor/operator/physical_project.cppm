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

export module physical_project;

namespace infinity {

export class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(u64 id, u64 table_index, SharedPtr<PhysicalOperator> left, Vector<SharedPtr<BaseExpression>> expressions)
        : PhysicalOperator(PhysicalOperatorType::kProjection, Move(left), nullptr, id), projection_table_index_(table_index),
          expressions_(Move(expressions)) {}

    ~PhysicalProject() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    Vector<SharedPtr<BaseExpression>> expressions_{};

    inline u64 TableIndex() const { return projection_table_index_; }

private:
    //    ExpressionExecutor executor;
    u64 projection_table_index_{};
};

} // namespace infinity
