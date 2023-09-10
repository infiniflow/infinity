//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "expression/base_expression.h"
#include "executor/hash_table.h"

namespace infinity {

class PhysicalAggregate final : public PhysicalOperator {
public:
    explicit PhysicalAggregate(u64 id,
                               SharedPtr<PhysicalOperator> left,
                               Vector<SharedPtr<BaseExpression>> groups,
                               u64 groupby_index,
                               Vector<SharedPtr<BaseExpression>> aggregates,
                               u64 aggregate_index)
                               : PhysicalOperator(PhysicalOperatorType::kAggregate, std::move(left), nullptr, id),
                               groups_(std::move(groups)),
                               groupby_index_(groupby_index),
                               aggregates_(std::move(aggregates)),
                               aggregate_index_(aggregate_index)
                               {}

    ~PhysicalAggregate() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    void
    GroupByInputTable(const SharedPtr<Table>& input_table, SharedPtr<Table>& output_table);

    void
    GenerateGroupByResult(const SharedPtr<Table>& input_table, SharedPtr<Table>& output_table);

    Vector<SharedPtr<BaseExpression>> groups_{};
    Vector<SharedPtr<BaseExpression>> aggregates_{};
    HashTable hash_table_;

    void
    SimpleAggregate(SharedPtr<Table>& output_table);

    inline u64
    GroupTableIndex() const {
        return groupby_index_;
    }

    inline u64
    AggregateTableIndex() const {
        return aggregate_index_;
    }

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final;

    bool
    IsSink() const override {
        return true;
    }

private:
    SharedPtr<Table> input_table_{};
    u64 groupby_index_{};
    u64 aggregate_index_{};
};

}

