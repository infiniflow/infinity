//
// Created by JinHai on 2022/7/23.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;
import base_expression;

export module logical_insert;

namespace infinity {

class TableCollectionEntry;

export class LogicalInsert : public LogicalNode {
public:
    explicit inline
    LogicalInsert(u64 node_id, TableCollectionEntry *table_collection_ptr, u64 table_index, Vector<Vector<SharedPtr<BaseExpression>>> value_list)
        : LogicalNode(node_id, LogicalNodeType::kInsert), table_collection_entry_(table_collection_ptr), table_index_(table_index),
          value_list_(Move(value_list)){};

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalInsert"; }

    inline void set_value_list(const Vector<Vector<SharedPtr<BaseExpression>>> &value_list) { value_list_ = value_list; }

    inline TableCollectionEntry *table_collection_entry() { return table_collection_entry_; }

    [[nodiscard]] inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline Vector<Vector<SharedPtr<BaseExpression>>> &value_list() { return value_list_; }

    [[nodiscard]] inline const Vector<Vector<SharedPtr<BaseExpression>>> &value_list() const { return value_list_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

public:
    static bool NeedCastInInsert(const DataType &from, const DataType &to) {
        if (from.type() == to.type()) {
            switch (from.type()) {
                // Embedding
                // Bitmap
                // Decimal
                default:
                    break;
            }
        }
        return true;
    }

private:
    TableCollectionEntry *table_collection_entry_{};
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};
    u64 table_index_{};
};

} // namespace infinity
