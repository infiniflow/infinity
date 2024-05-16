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
#include <memory>
#include <string>
#include <utility>

namespace infinity {

enum class CommandType {
    kInvalid,
    kUse,
    kSet,
    kExport,
    kCheckTable,
    kCompactTable,
};

class CommandInfo {
public:
    explicit CommandInfo(CommandType type) : type_(type) {}

    virtual ~CommandInfo() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    CommandType type() const { return type_; }

private:
    CommandType type_{CommandType::kInvalid};
};

class UseCmd final : public CommandInfo {
public:
    explicit UseCmd(const char *db_name) : CommandInfo(CommandType::kUse), db_name_(db_name) {}

    ~UseCmd() final = default;

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] const std::string &db_name() const { return db_name_; }

private:
    std::string db_name_{};
};

enum class SetScope {
    kSession,
    kGlobal,
    kConfig,
    kInvalid,
};

enum class SetVarType { kBool, kInteger, kDouble, kString, kInvalid };

class SetCmd final : public CommandInfo {
public:
    SetCmd(SetScope scope, SetVarType value_type, const char *var_name, bool value_bool)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(var_name), value_type_(value_type), value_bool_(value_bool) {}

    SetCmd(SetScope scope, SetVarType value_type, const char *var_name, int64_t value_int)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(var_name), value_type_(value_type), value_int_(value_int) {}

    SetCmd(SetScope scope, SetVarType value_type, const char *var_name, double value_double)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(var_name), value_type_(value_type), value_double_(value_double) {}

    SetCmd(SetScope scope, SetVarType value_type, const char *var_name, const char *value_str)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(var_name), value_type_(value_type), value_str_(value_str) {}

    SetCmd(SetScope scope, SetVarType value_type, std::string var_name, bool value_bool)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(std::move(var_name)), value_type_(value_type), value_bool_(value_bool) {}

    SetCmd(SetScope scope, SetVarType value_type, std::string var_name, int64_t value_int)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(std::move(var_name)), value_type_(value_type), value_int_(value_int) {}

    SetCmd(SetScope scope, SetVarType value_type, std::string var_name, double value_double)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(std::move(var_name)), value_type_(value_type), value_double_(value_double) {}

    SetCmd(SetScope scope, SetVarType value_type, std::string var_name, std::string value_str)
        : CommandInfo(CommandType::kSet), scope_(scope), var_name_(std::move(var_name)), value_type_(value_type), value_str_(std::move(value_str)) {}

    ~SetCmd() final = default;

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] inline SetScope scope() const { return scope_; }
    [[nodiscard]] inline std::string var_name() const { return var_name_; }
    [[nodiscard]] inline SetVarType value_type() const { return value_type_; }
    [[nodiscard]] inline std::string value_type_str() const {
        switch (value_type_) {
            case SetVarType::kBool: {
                return "Boolean";
            }
            case SetVarType::kInteger: {
                return "Integer";
            }
            case SetVarType::kDouble: {
                return "Double";
            }
            case SetVarType::kString: {
                return "String";
            }
            case SetVarType::kInvalid: {
                return "Invalid";
            }
        };
    }

    [[nodiscard]] inline std::string value_str() const { return value_str_; }
    [[nodiscard]] inline bool value_bool() const { return value_bool_; }
    [[nodiscard]] inline int64_t value_int() const { return value_int_; }
    [[nodiscard]] inline double value_double() const { return value_double_; }

private:
    SetScope scope_{SetScope::kSession};
    std::string var_name_{};
    SetVarType value_type_{SetVarType::kInteger};
    bool value_bool_{};
    int64_t value_int_{};
    double value_double_{};
    std::string value_str_{};
};

enum class ExportType {
    kProfileRecord,
};

class ExportCmd final : public CommandInfo {
public:
    explicit ExportCmd(const char *file_name, ExportType export_type, int64_t file_no)
        : CommandInfo(CommandType::kExport), file_name_(file_name), export_type_(export_type), file_no_(file_no) {}

    ~ExportCmd() final = default;

    [[nodiscard]] std::string ToString() const final;

    [[nodiscard]] const std::string &file_name() const { return file_name_; }
    [[nodiscard]] ExportType export_type() const { return export_type_; }
    [[nodiscard]] int64_t file_no() const { return file_no_; }

private:
    std::string file_name_{};
    ExportType export_type_{ExportType::kProfileRecord};
    int64_t file_no_{-1};
};

class CheckTable final : public CommandInfo {
public:
    explicit CheckTable(const char *table_name) : CommandInfo(CommandType::kCheckTable), table_name_(table_name) {}

    ~CheckTable() final = default;

    [[nodiscard]] std::string ToString() const final;

    const std::string &table_name() const { return table_name_; }

private:
    std::string table_name_;
};

class CompactTable final : public CommandInfo {
public:
    explicit CompactTable(std::string &&schema_name, std::string &&table_name)
        : CommandInfo(CommandType::kCompactTable), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {}

    explicit CompactTable(std::string &&table_name) : CommandInfo(CommandType::kCompactTable), table_name_(std::move(table_name)) {}

    std::string ToString() const final;

public:
    std::string schema_name_{};

    std::string table_name_;
};

class CommandStatement final : public BaseStatement {
public:
    CommandStatement() : BaseStatement(StatementType::kCommand) {}

    ~CommandStatement() final = default;

    std::unique_ptr<CommandInfo> command_info_{};

    [[nodiscard]] std::string ToString() const final;
};

} // namespace infinity
