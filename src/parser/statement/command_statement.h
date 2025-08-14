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
#include "statement_common.h"
// #include <memory>
// #include <string>
// #include <utility>
// #include <optional>

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

enum class CommandType {
    kInvalid,
    kUse,
    kSet,
    kExport,
    kCheckTable,
    kCleanup,
    kDumpIndex,
    kTestCommand,
    kSnapshot,
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

class CleanupCmd final : public CommandInfo {
public:
    explicit CleanupCmd() : CommandInfo(CommandType::kCleanup) {}

    ~CleanupCmd() final = default;

    [[nodiscard]] std::string ToString() const final;
};

class DumpIndexCmd final : public CommandInfo {
public:
    explicit DumpIndexCmd(const char *db_name, const char *table_name, const char *index_name)
        : CommandInfo(CommandType::kDumpIndex), db_name_(db_name == nullptr ? "" : db_name), table_name_(table_name), index_name_(index_name) {}

    ~DumpIndexCmd() final = default;

    [[nodiscard]] std::string ToString() const final;

    const std::string &db_name() const { return db_name_; }
    const std::string &table_name() const { return table_name_; }
    const std::string &index_name() const { return index_name_; }

    void SetDBName(const std::string &db_name) { db_name_ = db_name; }

private:
    std::string db_name_{};
    std::string table_name_{};
    std::string index_name_{};
};

class CommandStatement final : public BaseStatement {
public:
    CommandStatement() : BaseStatement(StatementType::kCommand) {}

    ~CommandStatement() final = default;

    std::shared_ptr<CommandInfo> command_info_{};

    [[nodiscard]] std::string ToString() const final;
};

class TestCmd final : public CommandInfo {
public:
    TestCmd(std::string command_content) : CommandInfo(CommandType::kTestCommand), command_content_(std::move(command_content)) {}

    [[nodiscard]] std::string ToString() const final;

    const std::string &command_content() { return command_content_; }

private:
    std::string command_content_{};
};

enum class SnapshotOp { kCreate, kDrop, kRestore, kInvalid };
enum class SnapshotScope : uint8_t { kTable = 0, kDatabase, kSystem, kIgnore, kInvalid };

class SnapshotCmd final : public CommandInfo {
public:
    SnapshotCmd(std::string name, SnapshotOp op, SnapshotScope scope, std::optional<std::string> object_name = std::nullopt)
        : CommandInfo(CommandType::kSnapshot), name_(std::move(name)), operation_(op), scope_(scope), object_name_(std::move(object_name)) {}

    [[nodiscard]] std::string ToString() const final;

    const std::string &name() { return name_; }
    const std::string &object_name() { return object_name_.value(); }
    SnapshotOp operation() { return operation_; }
    SnapshotScope &scope() { return scope_; }

private:
    std::string name_{};
    SnapshotOp operation_{SnapshotOp::kInvalid};
    SnapshotScope scope_{SnapshotScope::kInvalid};
    std::optional<std::string> object_name_{std::nullopt};
};

} // namespace infinity
