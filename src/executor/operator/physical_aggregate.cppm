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
import table;
import hash_table;
import base_expression;

export module physical_aggregate;

namespace infinity {

export struct HashRange {
    i64 start_{};
    i64 end_{};
};

export class PhysicalAggregate final : public PhysicalOperator {
public:
    explicit PhysicalAggregate(u64 id,
                               SharedPtr<PhysicalOperator> left,
                               Vector<SharedPtr<BaseExpression>> groups,
                               u64 groupby_index,
                               Vector<SharedPtr<BaseExpression>> aggregates,
                               u64 aggregate_index)
        : PhysicalOperator(PhysicalOperatorType::kAggregate, Move(left), nullptr, id), groups_(Move(groups)), groupby_index_(groupby_index),
          aggregates_(Move(aggregates)), aggregate_index_(aggregate_index) {}

    ~PhysicalAggregate() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    void GroupByInputTable(const SharedPtr<Table> &input_table, SharedPtr<Table> &output_table);

    void GenerateGroupByResult(const SharedPtr<Table> &input_table, SharedPtr<Table> &output_table);

    Vector<SharedPtr<BaseExpression>> groups_{};
    Vector<SharedPtr<BaseExpression>> aggregates_{};
    HashTable hash_table_;

    void SimpleAggregate(SharedPtr<Table> &output_table);

    inline u64 GroupTableIndex() const { return groupby_index_; }

    inline u64 AggregateTableIndex() const { return aggregate_index_; }

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    bool IsSink() const override { return true; }

    Vector<HashRange> GetHashRanges(i64 parallel_count) const;

private:
    SharedPtr<Table> input_table_{};
    u64 groupby_index_{};
    u64 aggregate_index_{};
};

} // namespace infinity
