//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDummyScan : public LogicalNode {
public:
    explicit
    LogicalDummyScan(u64 node_id,
                     String table_alias,
                     u64 table_index)
        : LogicalNode(node_id, LogicalNodeType::kDummyScan),
          table_alias_(std::move(table_alias)),
          table_index_(table_index)
          {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>>();
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return MakeShared<Vector<SharedPtr<DataType>>>();
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDummyScan";
    }

    String table_alias_;
    u64 table_index_;
};

}
