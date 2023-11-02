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
import data_table;

export module physical_cross_product;

namespace infinity {

export class PhysicalCrossProduct final : public PhysicalOperator {
public:
    explicit PhysicalCrossProduct(u64 id, SharedPtr<PhysicalOperator> left, SharedPtr<PhysicalOperator> right)
        : PhysicalOperator(PhysicalOperatorType::kCrossProduct, Move(left), Move(right), id) {}

    ~PhysicalCrossProduct() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

private:
    SharedPtr<DataTable> left_table_{};
    SharedPtr<DataTable> right_table_{};
};

} // namespace infinity
