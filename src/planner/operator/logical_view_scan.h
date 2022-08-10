//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_operator.h"

#include "storage/view.h"

namespace infinity {

class LogicalViewScan: public LogicalOperator {

public:
    explicit LogicalViewScan(std::shared_ptr<View> view_ptr)
    : LogicalOperator(LogicalOperatorType::kViewScan), view_ptr_(std::move(view_ptr)) {}

    [[nodiscard]] std::shared_ptr<View> view_ptr() const { return view_ptr_; }
    std::string ToString(uint64_t space) final;
private:
    std::shared_ptr<View> view_ptr_{nullptr};

};

}
