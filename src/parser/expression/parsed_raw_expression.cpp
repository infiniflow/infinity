//
// Created by jinhai on 23-1-22.
//

#include "parsed_raw_expression.h"

namespace infinity {

String
ParsedRawExpression::ToString() const {
    return raw_expr_->getName();
}

}
