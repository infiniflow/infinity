//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropTable : public LogicalNode {
public:
    LogicalDropTable(SharedPtr<String> schema_name,
                     SharedPtr<String> table_name,
                     u64 table_index)
        : LogicalNode(LogicalNodeType::kDropTable),
          schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)),
          table_index_(table_index)
          {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropTable";
    }

    [[nodiscard]] SharedPtr<String>
    table_name() const {
        return table_name_;
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline u64
    table_index() const {
        return table_index_;
    }

private:
    SharedPtr<String> table_name_{};
    SharedPtr<String> schema_name_{};
    u64 table_index_{};
};

}
