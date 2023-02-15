//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDummyScan : public LogicalNode {
public:
    explicit
    LogicalDummyScan(String table_alias,
                     u64 table_index)
        : LogicalNode(LogicalNodeType::kDummyScan),
          table_alias_(std::move(table_alias)),
          table_index_(table_index)
          {}

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDummyScan";
    }

    String table_alias_;
    u64 table_index_;
};

}
