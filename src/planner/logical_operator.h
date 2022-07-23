//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "logical_operator_type.h"

class LogicalOperator {
public:
    explicit LogicalOperator(LogicalOperatorType node_type);
    virtual ~LogicalOperator() = default;
private:
    LogicalOperatorType node_type_ = LogicalOperatorType::kInvalid;
};


