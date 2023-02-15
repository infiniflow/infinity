//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/view.h"

namespace infinity {

class LogicalViewScan: public LogicalNode {

public:
    explicit LogicalViewScan(SharedPtr<View> view_ptr)
    : LogicalNode(LogicalNodeType::kViewScan), view_ptr_(std::move(view_ptr)) {}

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
