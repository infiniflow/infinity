//
// Created by JinHai on 2022/7/26.
//

module;

import stl;

export module optimizer;

namespace infinity {

class OptimizerRule;
class LogicalNode;
class QueryContext;

export class Optimizer {
public:
    explicit Optimizer(QueryContext *query_context_ptr);

    void AddRule(SharedPtr<OptimizerRule> rule);

    SharedPtr<LogicalNode> optimize(const SharedPtr<LogicalNode> &unoptimized_plan);

public:
    QueryContext *query_context_ptr_{};

    Vector<SharedPtr<OptimizerRule>> rules_{};
};

} // namespace infinity
