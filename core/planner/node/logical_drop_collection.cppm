//
// Created by jinhai on 23-3-6.
//

module;

import stl;
import parser;
import logical_node;
import logical_node_type;
import column_binding;

export module logical_drop_collection;

namespace infinity {

export class LogicalDropCollection final : public LogicalNode {
public:
    LogicalDropCollection(u64 node_id, SharedPtr<String> schema_name, SharedPtr<String> collection_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kDropCollection), schema_name_(Move(schema_name)), collection_name_(Move(collection_name)),
          conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDropCollection"; }

    [[nodiscard]] SharedPtr<String> collection_name() const { return collection_name_; }

    [[nodiscard]] SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> collection_name_{};
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
