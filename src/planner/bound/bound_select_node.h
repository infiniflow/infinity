//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "planner/bound_node.h"

namespace infinity {

class TableRef;
class BindContext;

class BoundSelectNode : public BoundNode {
public:
    BoundSelectNode(std::shared_ptr<BindContext> bind_context_ptr)
        : BoundNode(BoundNodeType::kSelect), bind_context_ptr_(std::move(bind_context_ptr)) {}

    std::shared_ptr<BindContext> bind_context_ptr_;

    // From clause
    std::shared_ptr<TableRef> table_ref_ptr_;

    int64_t GetTableIndex() override { return 0; };

};

}
