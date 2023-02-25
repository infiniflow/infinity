//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"

namespace infinity {

class ParserResult {
public:

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
        statements_.clear();
        error_message_.clear();
        error_line_ = 0;
        error_position_ = 0;
    }

    Vector<SharedPtr<BaseStatement>> statements_;
    String error_message_;
    SizeT error_line_;
    SizeT error_position_;
};

}

