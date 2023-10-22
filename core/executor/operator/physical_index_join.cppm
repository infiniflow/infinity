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

export module physical_index_join;

namespace infinity {

export class PhysicalIndexJoin : public PhysicalOperator {
public:
    explicit PhysicalIndexJoin(u64 id) : PhysicalOperator(PhysicalOperatorType::kJoinIndex, nullptr, nullptr, id) {}

    ~PhysicalIndexJoin() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;
};

} // namespace infinity
