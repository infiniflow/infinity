//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "expression/base_expression.h"
#include "storage/table.h"
#include "planner/logical_node.h"

namespace infinity {

class LogicalInsert : public LogicalNode {
public:
    explicit LogicalInsert(SharedPtr<Table> table_ptr,
                           u64 table_index,
                           Vector<SharedPtr<BaseExpression>> value_list)
            : LogicalNode(LogicalNodeType::kInsert),
              table_ptr_(std::move(table_ptr)),
              table_index_(table_index),
              value_list_(std::move(value_list)) {};

    String 
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalInsert";
    }

    void 
    set_value_list(const Vector<SharedPtr<BaseExpression>>& value_list) {
        value_list_ = value_list;
    }

    SharedPtr<Table>& table_ptr() {
        return table_ptr_;
    }

    Vector<SharedPtr<BaseExpression>>&
    value_list() {
        return value_list_;
    }

    [[nodiscard]] inline u64
    table_index() const {
        return table_index_;
    }

private:
    SharedPtr<Table> table_ptr_{};
    Vector<SharedPtr<BaseExpression>> value_list_{};
    u64 table_index_{};
};

}
