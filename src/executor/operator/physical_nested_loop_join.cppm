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

export module physical_nested_loop_join;

namespace infinity {

export class PhysicalNestedLoopJoin : public PhysicalOperator {
public:
    explicit PhysicalNestedLoopJoin(u64 id,
                                    JoinType join_type,
                                    Vector<SharedPtr<BaseExpression>> conditions,
                                    SharedPtr<PhysicalOperator> left,
                                    SharedPtr<PhysicalOperator> right)
        : PhysicalOperator(PhysicalOperatorType::kJoinNestedLoop, Move(left), Move(right), id), join_type_(join_type),
          conditions_(Move(conditions)) {}

    ~PhysicalNestedLoopJoin() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    inline const Vector<SharedPtr<BaseExpression>> &conditions() const { return conditions_; }

private:
    SharedPtr<DataTable> left_table_{};
    SharedPtr<DataTable> right_table_{};
    JoinType join_type_{JoinType::kInner};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

} // namespace infinity