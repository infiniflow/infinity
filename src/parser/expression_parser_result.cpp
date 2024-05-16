// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#include "expression_parser_result.h"

#include <sstream>

namespace infinity {

ExpressionParserResult::~ExpressionParserResult() {
    if (exprs_ptr_ == nullptr)
        return;
    for (auto &expr : *exprs_ptr_) {
        delete expr;
    }
    delete exprs_ptr_;
}

void ExpressionParserResult::SetErrorMessage(const char *msg, size_t error_line, size_t error_position) {
    this->error_message_ = std::string(msg);
    this->error_line_ = error_line;
    this->error_position_ = error_position;
}

std::string ExpressionParserResult::ToString() {
    std::stringstream ss;
    if (error_message_.empty()) {
        // Success
        for (const auto &expr : *exprs_ptr_) {
            ss << expr->ToString() << std::endl;
        }
        return ss.str();
    } else {
        // Fail with error
        ss << "Error(" << error_line_ << ":" << error_position_ << "): " << error_message_;
        return ss.str();
    }
}

} // namespace infinity