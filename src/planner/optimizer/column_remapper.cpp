//
// Created by jinhai on 23-2-17.
//

#include "column_remapper.h"

namespace infinity {

void
ColumnRemapper::VisitNode(LogicalNode &op) {
    VisitNodeChildren(op);
    VisitNodeExpression(op);
//    bindings = op.GetColumnBindings();
}

}


