//
// Created by JinHai on 2022/7/26.
//

module;

import logical_node;
import stl;

export module optimizer_rule;

namespace infinity {

class QueryContext;
export class OptimizerRule {
public:
    virtual void ApplyToPlan(QueryContext *query_context_ptr, const SharedPtr<LogicalNode> &logical_plan) = 0;

    virtual String name() const = 0;
};

} // namespace infinity
