//
// Created by JinHai on 2022/9/24.
//

#include "subquery_flattener.h"

namespace infinity {

SubqueryFlattener::SubqueryFlattener(std::shared_ptr<BoundSelectNode>& bound_select_node,
                                     std::shared_ptr<BindContext> &bind_context_ptr)
                                     : bound_select_node_(bound_select_node), bind_context_ptr_(bind_context_ptr)
                                     {}

std::shared_ptr<BoundSelectNode>
SubqueryFlattener::GetResult() {
    FlattenProjectList();
    FlattenWhereClause();
    FlattenHavingList();
    return bound_select_node_;
}

void
SubqueryFlattener::FlattenProjectList() {

}

void
SubqueryFlattener::FlattenWhereClause() {

}

void
SubqueryFlattener::FlattenHavingList() {

}

}
