//
// Created by JinHai on 2022/7/30.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

#include "storage/table.h"

namespace infinity {

enum class ShowType {
    kInvalid,
    kShowTables,
    kShowViews,
    kShowColumn,
    kIntermediate,
};

String ToString(ShowType type);

class LogicalShow : public LogicalNode {
public:
    explicit LogicalShow(u64 node_id, ShowType type, String schema_name, String object_name, u64 table_index)
        : LogicalNode(node_id, LogicalNodeType::kShow), scan_type_(type), schema_name_(std::move(schema_name)), object_name_(std::move(object_name)),
          table_index_(table_index) {}

    [[nodiscard]] inline Vector<ColumnBinding> GetColumnBindings() const final { return {}; }

    [[nodiscard]] inline SharedPtr<Vector<String>> GetOutputNames() const final { return MakeShared<Vector<String>>(); }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return MakeShared<Vector<SharedPtr<DataType>>>(); }

    String ToString(i64 &space) final;

    inline String name() final { return "LogicalShow"; }

    [[nodiscard]] ShowType scan_type() const { return scan_type_; }

    [[nodiscard]] inline u64 table_index() const { return table_index_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &object_name() const { return object_name_; }

private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_;
    String object_name_; // It could be table/collection/view name
    u64 table_index_{};
};

} // namespace infinity
