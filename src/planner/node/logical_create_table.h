//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class LogicalCreateTable : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateTable>
    Make(u64 node_id, const SharedPtr<String> &schema_name, const SharedPtr<TableDef> &table_def_ptr, u64 table_index, ConflictType conflict_type) {
        return MakeShared<LogicalCreateTable>(node_id, schema_name, table_def_ptr, table_index, conflict_type);
    }

public:
    LogicalCreateTable(u64 node_id, SharedPtr<String> schema_name, SharedPtr<TableDef> table_def_ptr, u64 table_index, ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateTable), schema_name_(std::move(schema_name)), table_definition_(std::move(table_def_ptr)),
          table_index_(table_index), conflict_type_(conflict_type) {}

    [[nodiscard]] inline Vector<ColumnBinding> GetColumnBindings() const final { return {}; }

    [[nodiscard]] inline SharedPtr<Vector<String>> GetOutputNames() const final {
        SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
        result->emplace_back("OK");
        return result;
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final {
        SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
        result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
        return result_type;
    }

    String ToString(i64 &space) final;

    inline String name() final { return "LogicalCreateTable"; }

    [[nodiscard]] SharedPtr<TableDef> table_definitions() const { return table_definition_; }

    [[nodiscard]] SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<TableDef> table_definition_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
