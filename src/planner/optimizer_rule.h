//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include <string>
#include "logical_operator.h"

namespace infinity {

class OptimizerRule {
public:
    virtual ~OptimizerRule() = 0;

    virtual void ApplyToPlan(const std::shared_ptr<LogicalOperator>& logical_plan) const = 0;
    std::string name() const { return name_; };
private:
    std::string name_;
};


}
