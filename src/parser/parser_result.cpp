//
// Created by jinhai on 23-2-22.
//

#include "parser_result.h"

#include <sstream>

namespace infinity {

ParserResult::~ParserResult() {
    if (statements_ptr_ == nullptr)
        return;
    for (auto &statement : *statements_ptr_) {
        delete statement;
    }
    delete statements_ptr_;
}

void ParserResult::SetErrorMessage(const char *msg, size_t error_line, size_t error_position) {
    this->error_message_ = std::string(msg);
    this->error_line_ = error_line;
    this->error_position_ = error_position;
}

std::string ParserResult::ToString() {
    std::stringstream ss;
    if (error_message_.empty()) {
        // Success
        for (const auto &statement : *statements_ptr_) {
            ss << statement->ToString() << std::endl;
        }
        return ss.str();
    } else {
        // Fail with error
        ss << "Error(" << error_line_ << ":" << error_position_ << "): " << error_message_;
        return ss.str();
    }
}

} // namespace infinity