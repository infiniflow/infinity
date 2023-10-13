//
// Created by jinhai on 23-1-17.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class DataType;
class QueryContext;

struct BoundStatement {
public:
    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<SharedPtr<DataType>>> types_ptr_{};

    virtual SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) = 0;
};

} // namespace infinity
