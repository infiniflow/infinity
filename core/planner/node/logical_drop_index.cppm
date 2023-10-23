//
// Created by jinhai on 23-3-6.
//

module;

import stl;
import parser;
import logical_node;
import logical_node_type;
import column_binding;

export module logical_drop_index;

namespace infinity {

export class LogicalDropIndex final : public LogicalNode {
public:
    LogicalDropIndex(u64 node_id, SharedPtr<String> schema_name, SharedPtr<String> index_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kDropSchema), schema_name_(Move(schema_name)), index_name_(Move(index_name)),
          conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDropIndex"; }

    [[nodiscard]] SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] SharedPtr<String> index_name() const { return index_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> index_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
