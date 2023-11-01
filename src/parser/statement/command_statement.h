//
// Created by jinhai on 23-10-31.
//

#pragma once

#include "base_statement.h"
#include <memory>
#include <utility>

namespace infinity {

enum class CommandType {
    kInvalid,
    kUse,
    kCheckTable,
};

class CommandInfo {
public:
    explicit
    CommandInfo(CommandType type): type_(type) {}

    virtual ~CommandInfo() = default;

    [[nodiscard]] virtual std::string ToString() const = 0;

    CommandType type() const { return type_; }

private:
    CommandType type_{CommandType::kInvalid};
};

class UseCmd final: public CommandInfo {
public:
    explicit
    UseCmd(const char* db_name): CommandInfo(CommandType::kUse), db_name_(db_name) {}

    ~UseCmd() final = default;

    [[nodiscard]] std::string ToString() const final;

    const std::string& db_name() const { return db_name_; }

private:
    std::string db_name_;
};

class CheckTable final: public CommandInfo {
public:
    explicit
    CheckTable(const char* table_name): CommandInfo(CommandType::kCheckTable), table_name_(table_name) {}

    ~CheckTable() final = default;

    [[nodiscard]] std::string ToString() const final;

    const std::string& table_name() const { return table_name_; }

private:
    std::string table_name_;
};

class CommandStatement final : public BaseStatement {
public:
    CommandStatement() : BaseStatement(StatementType::kCommand) {}

    ~CommandStatement() final = default;

    std::shared_ptr<CommandInfo> command_info_{};

    [[nodiscard]] std::string ToString() const final;
};

} // namespace infinity
