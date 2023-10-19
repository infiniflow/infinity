//
// Created by jinhai on 23-3-6.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_create_collection;

namespace infinity {

export class LogicalCreateCollection : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateCollection>
    Make(u64 node_id, const SharedPtr<String> &schema_name, const SharedPtr<String> &collection_name, u64 table_index, ConflictType conflict_type) {
        return MakeShared<LogicalCreateCollection>(node_id, schema_name, collection_name, table_index, conflict_type);
    }

public:
    LogicalCreateCollection(u64 node_id,
                            SharedPtr<String> schema_name,
                            SharedPtr<String> collection_name_,
                            u64 table_index,
                            ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateCollection), schema_name_(Move(schema_name)),
          collection_name_(Move(collection_name_)), table_index_(table_index), conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCreateCollection"; }

    [[nodiscard]] inline SharedPtr<String> collection_name() const { return collection_name_; }

    [[nodiscard]] inline SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
