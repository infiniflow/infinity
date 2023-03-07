//
// Created by jinhai on 23-3-6.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class LogicalCreateView : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateView>
    Make(u64 node_id,
         const SharedPtr<String>& schema_name,
         const SharedPtr<String>& view_name,
         const Vector<SharedPtr<String>>& columns,
         ConflictType conflict_type) {
        return MakeShared<LogicalCreateView>(node_id,
                                             schema_name,
                                             view_name,
                                             columns,
                                             conflict_type);
    }

public:
    LogicalCreateView(u64 node_id,
                      SharedPtr<String> schema_name,
                      SharedPtr<String> view_name,
                      Vector<SharedPtr<String>> columns,
                      ConflictType conflict_type)
            : LogicalNode(node_id, LogicalNodeType::kCreateTable),
              schema_name_(std::move(schema_name)),
              view_name_(std::move(view_name)),
              columns_(std::move(columns)),
              conflict_type_(conflict_type)
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalCreateView";
    }

    [[nodiscard]] inline SharedPtr<String>
    view_name() const {
        return view_name_;
    }

    [[nodiscard]] inline SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> view_name_{};
    Vector<SharedPtr<String>> columns_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}
