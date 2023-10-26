//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"
#include "statement/select_statement.h"

namespace infinity {

class InsertStatement final : public BaseStatement {
public:
    InsertStatement() : BaseStatement(StatementType::kInsert) {}

    ~InsertStatement() final;

    [[nodiscard]] std::string ToString() const final;

    std::string schema_name_{"default"};
    std::string table_name_{};

    std::vector<std::string> *columns_{nullptr};
    std::vector<std::vector<ParsedExpr *> *> *values_{nullptr};

    SelectStatement *select_{nullptr};
};

} // namespace infinity
