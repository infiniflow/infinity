//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "common/types.h"
#include "planner/logical_node.h"

namespace infinity {

class LogicalJoin: public LogicalNode {
public:
    explicit
    LogicalJoin(JoinType join_type, std::vector<std::shared_ptr<BaseExpression>> conditions,
                const std::shared_ptr<LogicalNode>& left, const std::shared_ptr<LogicalNode>& right,
                std::shared_ptr<BindContext>& bind_context);

    std::string ToString(uint64_t space) final;
private:

    JoinType join_type_{JoinType::kInvalid};
    std::vector<std::shared_ptr<BaseExpression>> conditions_;
};

}

