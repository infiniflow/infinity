//
// Created by jinhai on 23-3-6.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_create_schema;


namespace infinity {

export class LogicalCreateSchema : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateSchema> Make(u64 node_id, const SharedPtr<String> &schema_name, ConflictType conflict_type) {
        return MakeShared<LogicalCreateSchema>(node_id, schema_name, conflict_type);
    }

public:
    LogicalCreateSchema(u64 node_id, SharedPtr<String> schema_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateSchema), schema_name_(Move(schema_name)), conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCreateSchema"; }

    [[nodiscard]] inline SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
