//
// Created by jinhai on 23-2-17.
//

#pragma once

#include "planner/logical_node_visitor.h"

namespace infinity {

class ColumnRemapper: public LogicalNodeVisitor {
public:
    void
    VisitNode(LogicalNode& op) final;

};

}

