//
// Created by jinhai on 23-4-27.
//

#pragma once

#include "physical_operator.h"

namespace infinity {

class PhysicalOperatorVisitor {
public:
    virtual void VisitNode(PhysicalOperator &op) = 0;
};

} // namespace infinity
