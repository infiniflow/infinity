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
    kAddColumn,
    kDropColumn,
    kAlterColumn,
    kRenameColumn,
};

class AlterStatement : public BaseStatement {
public:
    AlterStatement(TableName *table_name, AlterStatementType type) : BaseStatement(StatementType::kAlter), type_(type) {
        if (table_name->schema_name_ptr_ != nullptr) {
            schema_name_ = table_name->schema_name_ptr_;
            free(table_name->schema_name_ptr_);
        }
        table_name_ = table_name->table_name_ptr_;
        free(table_name->table_name_ptr_);
        delete table_name;
    }

    ~AlterStatement() = default;

    std::string schema_name_;
    std::string table_name_;

    AlterStatementType type_{AlterStatementType::kInvalid};
};

class RenameTableStatement final : public AlterStatement {
public:
    RenameTableStatement(TableName *table_name) : AlterStatement(table_name, AlterStatementType::kRenameTable) {}

    ~RenameTableStatement() = default;

    std::string ToString() const final { return "RenameTableStatement"; }

    std::string new_table_name_;
};

class AddColumnStatement final : public AlterStatement {
public:
    AddColumnStatement(TableName *table_name) : AlterStatement(table_name, AlterStatementType::kAddColumn) {}

    ~AddColumnStatement() {
        if (column_def_ != nullptr) {
            delete column_def_;
            column_def_ = nullptr;
        }
    }

    std::string ToString() const final { return "AddColumnStatement"; }

    ColumnDef *column_def_{};
};

class DropColumnStatement final : public AlterStatement {
public:
    DropColumnStatement(TableName *table_name) : AlterStatement(table_name, AlterStatementType::kDropColumn) {}

    ~DropColumnStatement() = default;

    std::string ToString() const final { return "DropColumnStatement"; }

    std::string column_name_;
};

class AlterColumnStatement final : public AlterStatement {
public:
    AlterColumnStatement(TableName *table_name) : AlterStatement(table_name, AlterStatementType::kAlterColumn) {}

    ~AlterColumnStatement() {
        if (column_def_ != nullptr) {
            delete column_def_;
            column_def_ = nullptr;
        }
    }

    std::string ToString() const final { return "AlterColumnStatement"; }

    ColumnDef *column_def_{};
};

class RenameColumnStatement final : public AlterStatement {
public:
    RenameColumnStatement(TableName *table_name) : AlterStatement(table_name, AlterStatementType::kRenameColumn) {}

    ~RenameColumnStatement() {}

    std::string ToString() const final { return "RenameColumnStatement"; }

    std::string column_name_;
    std::string new_column_name_;
};

} // namespace infinity
