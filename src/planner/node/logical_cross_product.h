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
                        const SharedPtr<LogicalNode>& left,
                        const SharedPtr<LogicalNode>& right);

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final;

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalCrossProduct";
    }

    String alias_{};
};

}
