//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/view.h"

namespace infinity {

class LogicalViewScan: public LogicalNode {

public:
    explicit LogicalViewScan(int64_t node_id, std::shared_ptr<View> view_ptr)
    : LogicalNode(node_id, LogicalNodeType::kViewScan), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] std::shared_ptr<View> view_ptr() const { return view_ptr_; }
    std::string ToString(uint64_t space) final;
private:
    std::shared_ptr<View> view_ptr_{nullptr};

};

}
