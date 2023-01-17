//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include <string>
#include "logical_node.h"

namespace infinity {

class OptimizerRule {
public:
    virtual
    ~OptimizerRule() = 0;

    virtual void
    ApplyToPlan(SharedPtr<QueryContext>& query_context_ptr,
                const SharedPtr<LogicalNode>& logical_plan) const = 0;

    String
    name() const {
        return name_;
    };

private:
    String name_;
};


}
