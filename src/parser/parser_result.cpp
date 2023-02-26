//
// Created by jinhai on 23-2-22.
//

#include "parser_result.h"

#include <sstream>

namespace infinity {

void
ParserResult::SetErrorMessage(const char *msg, SizeT error_line, SizeT error_position) {
    this->error_message_ = String(msg);
    this->error_line_ = error_line;
    this->error_position_ = error_position;
}

String
ParserResult::ToString() {
    std::stringstream ss;
    if (error_message_.empty()) {
        // Success
        for (const auto &statement: *statements_ptr_) {
            ss << statement->ToString() << std::endl;
        }
        return ss.str();
    } else {
        // Fail with error
        ss << "Error(" << error_line_ << ":" << error_position_ << "): " << error_message_;
        return ss.str();
    }
}

}