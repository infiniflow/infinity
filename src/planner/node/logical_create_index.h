#pragma once

#include "common/types/alias/strings.h"
#include "planner/logical_node.h"
#include "storage/index_def/index_def.h"

namespace infinity {
class LogicalCreateIndex : public LogicalNode {
  public:
    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const override {
        return {};
    };

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const override {
        SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
        result->emplace_back("OK");
        return result;
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const override {
        SharedPtr<Vector<SharedPtr<DataType>>> result_type =
            MakeShared<Vector<SharedPtr<DataType>>>();
        result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
        return result_type;
    }

    String ToString(i64 &space) override; // TTT shenyushi: should be const

    inline String name() override { return "LogicalCreateIndex"; }

  public:
    static inline SharedPtr<LogicalCreateIndex>
    Make(u64 node_id, SharedPtr<String> schema_name,
         SharedPtr<String> table_name, SharedPtr<IndexDef> index_def,
         ConflictType conflict_type) {
        return MakeShared<LogicalCreateIndex>(node_id, schema_name, table_name,
                                              index_def, conflict_type);
    }

    LogicalCreateIndex(u64 node_id, SharedPtr<String> schema_name,
                       SharedPtr<String> table_name,
                       SharedPtr<IndexDef> index_def,
                       ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kCreateIndex),
          schema_name_(schema_name), table_name_(table_name),
          index_definition_(index_def), conflict_type_(conflict_type) {}

  public:
    [[nodiscard]] inline SharedPtr<String> schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline SharedPtr<String> table_name() const {
        return table_name_;
    }

    [[nodiscard]] inline SharedPtr<IndexDef> index_definition() const {
        return index_definition_;
    }

    [[nodiscard]] inline ConflictType conflict_type() const {
        return conflict_type_;
    }

  private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<IndexDef> index_definition_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};
} // namespace infinity