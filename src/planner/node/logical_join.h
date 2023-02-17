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
                String alias,
                u64 join_index,
                Vector<SharedPtr<BaseExpression>> conditions,
                const SharedPtr<LogicalNode>& left,
                const SharedPtr<LogicalNode>& right);

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalJoin";
    }

    String alias_{};
    u64 table_index_{};

public:
    JoinType join_type_{JoinType::kInvalid};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

}

