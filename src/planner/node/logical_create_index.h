#pragma once

#include "parser/statement/extra/extra_ddl_info.h"
#include "planner/logical_node.h"

namespace infinity {

class IndexDef;

class LogicalCreateIndex : public LogicalNode {
public:
    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const override;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const override;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;
    String ToString(i64 &space) const final;

    inline String name() override { return "LogicalCreateIndex"; }

public:
    static inline SharedPtr<LogicalCreateIndex>
    Make(u64 node_id, SharedPtr<String> schema_name, SharedPtr<String> table_name, SharedPtr<IndexDef> index_def, ConflictType conflict_type) {
        return MakeShared<LogicalCreateIndex>(node_id, schema_name, table_name, index_def, conflict_type);
    }

    LogicalCreateIndex(u64 node_id,
                       SharedPtr<String> schema_name,
                       SharedPtr<String> table_name,
                       SharedPtr<IndexDef> index_def,
                       ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateIndex), schema_name_(schema_name), table_name_(table_name), index_definition_(index_def),
          conflict_type_(conflict_type) {}

public:
    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<IndexDef> index_definition_{};
    const ConflictType conflict_type_{ConflictType::kInvalid};
};
} // namespace infinity