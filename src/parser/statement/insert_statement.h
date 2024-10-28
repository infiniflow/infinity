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
#include "expr/insert_row_expr.h"
#include "statement/select_statement.h"

namespace infinity {

class InsertStatement final : public BaseStatement {
public:
    InsertStatement() : BaseStatement(StatementType::kInsert) {}

    ~InsertStatement() override = default;

    [[nodiscard]] std::string ToString() const override { return "insert statement"; }

    std::string schema_name_{};
    std::string table_name_{};

    std::vector<std::unique_ptr<InsertRowExpr>> insert_rows_{};

    std::vector<std::string> columns_for_select_{};
    std::unique_ptr<SelectStatement> select_{};
};

} // namespace infinity
