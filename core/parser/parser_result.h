//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"

#include <vector>

namespace infinity {

class ParserResult {
public:
    ~ParserResult();

    void SetErrorMessage(const char *msg, size_t error_line, size_t error_position);

    std::string ToString();

    [[nodiscard]] inline bool IsError() const { return !error_message_.empty(); }

    void Reset() {
        if (statements_ptr_ != nullptr) {
            for (auto *statement_ptr : *statements_ptr_) {
                delete statement_ptr;
            }
            delete statements_ptr_;
            statements_ptr_ = nullptr;
        }
        error_message_.clear();
        error_line_ = 0;
        error_position_ = 0;
    }

    std::vector<BaseStatement *> *statements_ptr_;
    std::string error_message_;
    size_t error_line_;
    size_t error_position_;
};

} // namespace infinity
