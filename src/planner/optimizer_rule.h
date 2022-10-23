//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include <string>
#include "logical_node.h"

namespace infinity {

class OptimizerRule {
public:
    virtual ~OptimizerRule() = 0;

    virtual void ApplyToPlan(std::shared_ptr<QueryContext>& query_context_ptr,
                             const std::shared_ptr<LogicalNode>& logical_plan) const = 0;
    std::string name() const { return name_; };
private:
    std::string name_;
};


}
