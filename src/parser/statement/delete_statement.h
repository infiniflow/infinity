//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/parsed_expr/parsed_expr.h"
#include "parser/base_statement.h"

namespace infinity {

class DeleteStatement final : public BaseStatement {
public:
    DeleteStatement() : BaseStatement(StatementType::kDelete) {}

    ~DeleteStatement() final;

    [[nodiscard]] String
    ToString() const final;

    String schema_name_{};
    String table_name_{};

    ParsedExpr* where_expr_{nullptr};

};

}
