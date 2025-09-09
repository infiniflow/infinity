// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "base_statement.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

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
