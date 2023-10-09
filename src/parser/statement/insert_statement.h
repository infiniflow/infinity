//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"
#include "parser/statement/select_statement.h"

namespace infinity {

class InsertStatement final : public BaseStatement {
public:
    InsertStatement() : BaseStatement(StatementType::kInsert) {}

    ~InsertStatement() final;

    [[nodiscard]] String ToString() const final;

    String schema_name_{"default"};
    String table_name_{};

    Vector<String> *columns_{nullptr};
    Vector<ParsedExpr *> *values_{nullptr};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
