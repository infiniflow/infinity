//
// Created by jinhai on 23-2-22.
//

#include "delete_statement.h"

namespace infinity {

DeleteStatement::~DeleteStatement() {
    if (where_expr_ != nullptr) {
        delete where_expr_;
        where_expr_ = nullptr;
    }
}

String DeleteStatement::ToString() const { return "Delete statement"; }

} // namespace infinity
