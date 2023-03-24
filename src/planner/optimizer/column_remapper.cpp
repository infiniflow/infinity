//
// Created by jinhai on 23-2-17.
//

#include "column_remapper.h"
#include "expression/reference_expression.h"

namespace infinity {

void
BindingRemapper::VisitNode(LogicalNode &op) {
    VisitNodeChildren(op);
    VisitNodeExpression(op);
    bindings_ = op.GetColumnBindings();
}

SharedPtr<BaseExpression>
BindingRemapper::VisitReplace(const SharedPtr<ColumnExpression>& expression) {
    SizeT binding_count= bindings_.size();
    for (SizeT idx = 0; idx < binding_count; ++ idx) {
        if (expression->binding() == bindings_[idx]) {
            return ReferenceExpression::Make(expression->Type(),
                                             expression->table_name(),
                                             expression->column_name(),
                                             expression->alias_,
                                             idx);
        }
    }
    LOG_ERROR("Can't bind column expression: {} [{}.{}]",
              expression->table_name(),
              expression->binding().table_idx,
              expression->binding().column_idx);
    return nullptr;
}

}


