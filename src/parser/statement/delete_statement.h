//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"
#include "expr/expr.h"
#include <string>

namespace infinity {

class DeleteStatement final : public BaseStatement {
public:
    DeleteStatement() : BaseStatement(StatementType::kDelete) {}

    ~DeleteStatement() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{};
    std::string table_name_{};

    ParsedExpr *where_expr_{nullptr};
};

} // namespace infinity
