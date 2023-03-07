//
// Created by jinhai on 23-3-6.
//


#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropView final : public LogicalNode {
public:
    LogicalDropView(u64 node_id,
                          SharedPtr<String> schema_name,
                          SharedPtr<String> view_name)
            : LogicalNode(node_id, LogicalNodeType::kDropTable),
              schema_name_(std::move(schema_name)),
              view_name_(std::move(view_name))
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropView";
    }

    [[nodiscard]] SharedPtr<String>
    view_name() const {
        return view_name_;
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<String> view_name_{};
    SharedPtr<String> schema_name_{};
};

}
