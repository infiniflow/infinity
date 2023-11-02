//
// Created by JinHai on 2022/7/23.
//

module;

import stl;
import parser;
import logical_node;
import logical_node_type;
import column_binding;
import table_collection_entry;

export module logical_delete;

namespace infinity {

export class LogicalDelete final : public LogicalNode {

public:
    LogicalDelete(u64 node_id, TableCollectionEntry *table_entry_ptr)
        : LogicalNode(node_id, LogicalNodeType::kDelete), table_entry_ptr_(table_entry_ptr) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDelete"; }

    TableCollectionEntry *table_entry_ptr_{};
};

} // namespace infinity
