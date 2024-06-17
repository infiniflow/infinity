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

#include "parser_assert.h"
#include <string>

namespace infinity {

enum class StatementType {
    kInvalidStmt, // unused
    kSelect,
    kCopy,
    kInsert,
    kUpdate,
    kDelete,
    kCreate,
    kDrop,
    kPrepare,
    kExecute,
    kAlter,
    kShow,
    kExplain,
    kFlush,
    kOptimize,
    kCommand,
    kCompact,
};

extern std::string StatementType2Str(StatementType statement_type);

class BaseStatement {
public:
    explicit BaseStatement(StatementType type) : type_(type) {}

    virtual ~BaseStatement() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    [[nodiscard]] inline StatementType Type() const { return type_; }

    StatementType type_{StatementType::kInvalidStmt};
    size_t stmt_location_{0};
    size_t stmt_length_ = {0};
    std::string text_{};
};

} // namespace infinity