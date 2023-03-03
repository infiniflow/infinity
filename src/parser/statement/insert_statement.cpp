//
// Created by jinhai on 23-2-22.
//

#include "insert_statement.h"

namespace infinity {

InsertStatement::~InsertStatement() {
    if(select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }

    if(columns_ != nullptr) {
        delete columns_;
        columns_ = nullptr;
    }

    if(values_ != nullptr) {
        for(auto& value_ptr: *values_) {
            delete value_ptr;
        }
        delete values_;
        values_ = nullptr;
    }
}

String
InsertStatement::ToString() const {
    return "insert statement";
}

}