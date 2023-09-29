//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"
#include "parser/table_reference/join_reference.h"

namespace infinity {

class BindContext;

class JoinTableRef : public TableRef {
public:
    explicit JoinTableRef(String alias) :
            TableRef(TableRefType::kJoin, std::move(alias)) {}

    SharedPtr<BindContext> left_bind_context_;
    SharedPtr<BindContext> right_bind_context_;

    SharedPtr<TableRef> left_table_ref_;
    SharedPtr<TableRef> right_table_ref_;

    // Condition ???
    std::vector<SharedPtr<BaseExpression>> on_conditions_;

    // Join Type ???
    JoinType join_type_;
};

}