//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"

namespace infinity {
class SubqueryTableRef: public TableRef {
public:
    explicit SubqueryTableRef(std::shared_ptr<BoundNode> subquery_node, std::string alias)
        : TableRef(TableRefType::kSubquery, std::move(alias)), subquery_node_(std::move(subquery_node)) {}

    std::shared_ptr<BoundNode> subquery_node_{nullptr};
};

}