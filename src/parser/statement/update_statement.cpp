//
// Created by jinhai on 23-2-22.
//

#include "update_statement.h"

namespace infinity {

UpdateStatement::~UpdateStatement() {
    if (where_expr_ != nullptr) {
        delete where_expr_;
        where_expr_ = nullptr;
    }

    if (update_expr_array_ != nullptr) {
        for (auto update_expr : *update_expr_array_) {
            delete update_expr;
        }
        delete update_expr_array_;
    }
}

std::string UpdateStatement::ToString() const { return "Update statement"; }

} // namespace infinity