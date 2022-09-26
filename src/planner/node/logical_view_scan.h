//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/view.h"

namespace infinity {

class LogicalViewScan: public LogicalNode {

public:
    explicit LogicalViewScan(std::shared_ptr<View> view_ptr, std::shared_ptr<BindContext>& bind_context)
    : LogicalNode(LogicalNodeType::kViewScan, bind_context), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] std::shared_ptr<View> view_ptr() const { return view_ptr_; }
    std::string ToString(int64_t& space) final;
private:
    std::shared_ptr<View> view_ptr_{nullptr};

};

}
