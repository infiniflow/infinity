#pragma once

#include "base_statement.h"
#include "statement/statement_common.h"
// #include <optional>

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

namespace infinity {

enum class CheckStmtType : uint8_t {
    kInvalid,
    kSystem,
    kTable,
};

class CheckStatement : public BaseStatement {
public:
    CheckStatement() : BaseStatement(StatementType::kCheck) {}

    ~CheckStatement() = default;

    [[nodiscard]] std::string ToString() const final;

    CheckStmtType check_type_{CheckStmtType::kInvalid};
    std::optional<std::string> schema_name_;
    std::optional<std::string> table_name_;
};

} // namespace infinity
