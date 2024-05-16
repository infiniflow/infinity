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

#include "expression_parser.h"
#include "expression_parser_result.h"

#include "expression_lexer.h"

#include <memory>
#include <string>

namespace infinity {

class ExprParser {

public:
    ExprParser();

    ~ExprParser();

    void Parse(const std::string &expr_text, ExpressionParserResult *result);

    void *scanner_{};
    YY_BUFFER_STATE state_;
};

} // namespace infinity
