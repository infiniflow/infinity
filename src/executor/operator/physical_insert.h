//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "expression/base_expression.h"

namespace infinity {

class PhysicalInsert : public PhysicalOperator {
public:
    explicit PhysicalInsert(u64 id, TableCollectionEntry *table_collection_entry, u64 table_index, Vector<SharedPtr<BaseExpression>> value_list)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id), table_collection_entry_(table_collection_entry),
          table_index_(table_index), value_list_(std::move(value_list)) {}

    ~PhysicalInsert() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline const Vector<SharedPtr<BaseExpression>> &value_list() const { return value_list_; }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    TableCollectionEntry *table_collection_entry_{};
    Vector<SharedPtr<BaseExpression>> value_list_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
