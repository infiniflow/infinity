//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"

namespace infinity {

class ParserResult {
public:

    ~ParserResult();

    void
    SetErrorMessage(const char *msg, SizeT error_line, SizeT error_position);

    String
    ToString();

    [[nodiscard]] inline bool
    IsError() const {
        return !error_message_.empty();
    }

    void
    Reset() {
        if(statements_ptr_ != nullptr) {
            for(auto* statement_ptr: *statements_ptr_) {
                delete statement_ptr;
            }
            delete statements_ptr_;
            statements_ptr_ = nullptr;
        }
        error_message_.clear();
        error_line_ = 0;
        error_position_ = 0;
    }

    Vector<BaseStatement*> *statements_ptr_;
    String error_message_;
    SizeT error_line_;
    SizeT error_position_;
};

}

