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

#include "sql_parser.h"

#include "lexer.h"

#include <iostream>

namespace infinity {

SQLParser::SQLParser() {
    scanner_ = nullptr;
    state_ = nullptr;
    if (sqllex_init(&scanner_)) {
        std::cerr << "Init lexer error" << std::endl;
    }
}

SQLParser::~SQLParser() { sqllex_destroy(scanner_); }

void SQLParser::Parse(const std::string &sql_text, std::shared_ptr<ParserResult> &result) {

    state_ = sql_scan_string(sql_text.c_str(), scanner_);

    if (sqlparse(scanner_, result.get())) {
        std::cerr << "Parse error: " << sql_text << std::endl;
    }

    sql_delete_buffer(state_, scanner_);
}

} // namespace infinity
