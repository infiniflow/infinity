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

#include "column_expr.h"
#include "parser_assert.h"

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

ColumnExpr::~ColumnExpr() = default;

std::string ColumnExpr::ToString() const {
    std::stringstream ss;
    size_t name_count = names_.size();
    if (name_count == 0) {
        if (star_) {
            return "*";
        }
        ParserError("Empty column expression");
        return std::string();
    }
    for (size_t i = 0; i < name_count - 1; ++i) {
        ss << names_[i] << ".";
    }
    ss << names_[name_count - 1];
    if (star_) {
        ss << ".*";
    }

    return ss.str();
}

} // namespace infinity
