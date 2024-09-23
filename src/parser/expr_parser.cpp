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

#include "expr_parser.h"

#include "lexer.h"

#include <iostream>

namespace infinity {

ExprParser::ExprParser() {
    scanner_ = nullptr;
    state_ = nullptr;
    if (sqllex_init(&scanner_)) {
        std::cerr << "Init lexer error" << std::endl;
    }
}

ExprParser::~ExprParser() { expressionlex_destroy(scanner_); }

void ExprParser::Parse(const std::string &expr_text, ExpressionParserResult *result) {

    state_ = expression_scan_string(expr_text.c_str(), scanner_);

    // WARNNING: shall reset result to avoid polluting later Parse!
    result->Reset();
    if (expressionparse(scanner_, result)) {
        std::cerr << "Parse expression error: " << expr_text << std::endl;
    }

    expression_delete_buffer(state_, scanner_);
}

} // namespace infinity
