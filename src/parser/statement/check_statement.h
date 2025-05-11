#pragma once

#include "base_statement.h"
#include "statement/statement_common.h"
#include <optional>

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
    std::string schema_name_;
    std::string table_name_;
};

} // namespace infinity
