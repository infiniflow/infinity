//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include <string>
#include "logical_node.h"

namespace infinity {

class OptimizerRule {
public:
    virtual void
    ApplyToPlan(QueryContext* query_context_ptr,
                const SharedPtr<LogicalNode>& logical_plan) = 0;

    virtual String
    name() const = 0;
};


}
