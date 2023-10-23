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

export module physical_insert;

namespace infinity {

class TableCollectionEntry;

export class PhysicalInsert : public PhysicalOperator {
public:
    explicit
    PhysicalInsert(u64 id, TableCollectionEntry *table_collection_entry, u64 table_index, Vector<Vector<SharedPtr<BaseExpression>>> value_list)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id), table_collection_entry_(table_collection_entry),
          table_index_(table_index), value_list_(Move(value_list)) {}

    ~PhysicalInsert() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline const Vector<Vector<SharedPtr<BaseExpression>>> &value_list() const { return value_list_; }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    TableCollectionEntry *table_collection_entry_{};
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
