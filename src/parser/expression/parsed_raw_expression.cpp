//
// Created by jinhai on 23-1-22.
//

#include "parsed_raw_expression.h"
#include "parser/statement.h"

namespace infinity {

String
ParsedRawExpression::ToString() const {
//    String expr_name = raw_expr_->getName() == nullptr ? Statement::ExprAsColumnName(raw_expr_) : raw_expr_->getName();
    String expr_name = Statement::ExprAsColumnName(raw_expr_);
    return expr_name;
}

}
