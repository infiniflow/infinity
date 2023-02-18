//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalCrossProduct: public LogicalNode {
public:
    explicit
    LogicalCrossProduct(u64 node_id,
                        String alias,
                        u64 join_index,
                        const SharedPtr<LogicalNode>& left,
                        const SharedPtr<LogicalNode>& right);

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalCrossProduct";
    }

    String alias_{};
    u64 table_index_{};
};

}
