//
// Created by jinhai on 23-2-22.
//

#include "select_statement.h"

namespace infinity {

SelectStatement::~SelectStatement() {
    if(table_ref_ != nullptr) {
        delete table_ref_;
        table_ref_ = nullptr;
    }

    if(select_list_ != nullptr) {
        for(auto expr_ptr: *select_list_) {
            delete expr_ptr;
        }
        delete select_list_;
    }

    if(where_expr_ != nullptr) {
        delete where_expr_;
        where_expr_ = nullptr;
    }
}

String
SelectStatement::ToString() const {
    return "Select Statement";
}

}
