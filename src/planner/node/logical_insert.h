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
                           Vector<SharedPtr<BaseExpression>> value_list,
                           SharedPtr<BindContext>& bind_context)
            : LogicalNode(LogicalNodeType::kInsert, bind_context),
              table_ptr_(std::move(table_ptr)), value_list_(std::move(value_list)) {};

    String 
    ToString(int64_t& space) final;
    
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

private:
    SharedPtr<Table> table_ptr_;
    Vector<SharedPtr<BaseExpression>> value_list_;
};

}
