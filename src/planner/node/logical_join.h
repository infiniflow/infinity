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
    LogicalJoin(JoinType join_type,
                Vector<SharedPtr<BaseExpression>> conditions,
                const SharedPtr<LogicalNode>& left,
                const SharedPtr<LogicalNode>& right);

    String
    ToString(i64& space) final;
private:

    JoinType join_type_{JoinType::kInvalid};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

}

