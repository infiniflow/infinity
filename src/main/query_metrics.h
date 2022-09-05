//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include <string>
#include <vector>

namespace infinity {

class QueryPhaseProfiler {
public:
private:
};

struct OptimizerRuleMetrics {
    std::string                 rule_name_;
    std::chrono::nanoseconds    rule_cost_;
};

struct SQLMetrics {
    std::chrono::nanoseconds parser_cost_;
    std::chrono::nanoseconds logical_plan_cost_;
    std::vector<OptimizerRuleMetrics> optimizer_costs_;
    std::chrono::nanoseconds physical_plan_cost_;
    std::chrono::nanoseconds pipeline_build_cost_;
    std::chrono::nanoseconds execution_cost_;
};

class QueryMetrics {
public:
private:
    std::shared_ptr<SQLMetrics> current_metric_;
    std::vector<std::shared_ptr<SQLMetrics>> statements_metric_;
};

}
