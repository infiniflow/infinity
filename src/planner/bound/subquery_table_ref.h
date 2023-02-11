//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"
#include "planner/bound_statement.h"

namespace infinity {
class SubqueryTableRef: public TableRef {
public:
    explicit SubqueryTableRef(SharedPtr<BoundStatement> subquery_node, u64 table_index, String alias)
        : TableRef(TableRefType::kSubquery, table_index, std::move(alias)), subquery_node_(std::move(subquery_node)) {}

    SharedPtr<BoundStatement> subquery_node_{nullptr};
};

}