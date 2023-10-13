//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "planner/logical_node.h"
#include "parser/statement/extra/extra_ddl_info.h"

namespace infinity {

class LogicalDropTable : public LogicalNode {
public:
    LogicalDropTable(u64 node_id, SharedPtr<String> schema_name, SharedPtr<String> table_name, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kDropTable), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)),
          conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDropTable"; }

    [[nodiscard]] inline SharedPtr<String> table_name() const { return table_name_; }

    [[nodiscard]] inline SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> table_name_{};
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
