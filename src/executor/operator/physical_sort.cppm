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
import table;

export module physical_sort;

namespace infinity {

export class PhysicalSort : public PhysicalOperator {
public:
    explicit PhysicalSort(u64 id, SharedPtr<PhysicalOperator> left, Vector<SharedPtr<BaseExpression>> expressions, Vector<OrderType> order_by_types)
        : PhysicalOperator(PhysicalOperatorType::kSort, Move(left), nullptr, id), expressions_(Move(expressions)),
          order_by_types_(Move(order_by_types)) {}

    ~PhysicalSort() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    void Sort(const SharedPtr<Table> &order_by_table, const Vector<OrderType> &order_by_types_);

    static SharedPtr<Table> GenerateOutput(const SharedPtr<Table> &input_table, const SharedPtr<Vector<RowID>> &rowid_vector);

    Vector<SharedPtr<BaseExpression>> expressions_;
    Vector<OrderType> order_by_types_{};

private:
    SharedPtr<Table> GetOrderTable() const;

private:
    SharedPtr<Table> input_table_{};
    u64 input_table_index_{};
};

} // namespace infinity
