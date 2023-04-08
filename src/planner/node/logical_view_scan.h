//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/view.h"

namespace infinity {

class LogicalViewScan: public LogicalNode {

public:
    explicit LogicalViewScan(u64 node_id,
                             SharedPtr<View> view_ptr)
    : LogicalNode(node_id, LogicalNodeType::kViewScan), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return left_node_->GetColumnBindings();
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return left_node_->GetOutputNames();
    }

    [[nodiscard]] SharedPtr<View>
    view_ptr() const {
        return view_ptr_;
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalViewScan";
    }

private:
    SharedPtr<View> view_ptr_{nullptr};

};

}
