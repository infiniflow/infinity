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
#include "definition/column_def.h"
#include "statement/statement_common.h"

namespace infinity {

enum class AlterStatementType : uint8_t {
    kInvalid,
    kRenameTable,
    kAddColumns,
    kDropColumns,
};

class AlterStatement : public BaseStatement {
public:
    AlterStatement(const char *schema_name, const char *table_name, AlterStatementType type) : BaseStatement(StatementType::kAlter), type_(type) {
        if (schema_name != nullptr) {
            schema_name_ = schema_name;
        }
        table_name_ = table_name;
    }

    ~AlterStatement() = default;

    std::string schema_name_;
    std::string table_name_;

    AlterStatementType type_{AlterStatementType::kInvalid};
};

class RenameTableStatement final : public AlterStatement {
public:
    RenameTableStatement(const char *schema_name, const char *table_name)
        : AlterStatement(schema_name, table_name, AlterStatementType::kRenameTable) {}

    ~RenameTableStatement() = default;

    std::string ToString() const final { return "RenameTableStatement"; }

    std::string new_table_name_;
};

class AddColumnsStatement final : public AlterStatement {
public:
    AddColumnsStatement(const char *schema_name, const char *table_name) : AlterStatement(schema_name, table_name, AlterStatementType::kAddColumns) {}

    ~AddColumnsStatement() = default;

    std::string ToString() const final { return "AddColumnsStatement"; }

    std::vector<std::shared_ptr<ColumnDef>> column_defs_{};
};

class DropColumnsStatement final : public AlterStatement {
public:
    DropColumnsStatement(const char *schema_name, const char *table_name)
        : AlterStatement(schema_name, table_name, AlterStatementType::kDropColumns) {}

    ~DropColumnsStatement() = default;

    std::string ToString() const final { return "DropColumnsStatement"; }

    std::vector<std::string> column_names_;
};

} // namespace infinity
