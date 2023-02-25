//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "parser/base_statement.h"

namespace infinity {

enum class ShowStmtType {
    kColumns,
    kTables,
};

class ShowStatement : public BaseStatement {
public:
    explicit
    ShowStatement() : BaseStatement(StatementType::kShow) {}

    [[nodiscard]] String
    ToString() const final;

    ShowStmtType show_type_{ShowStmtType::kTables};
    String schema_name_{"Default"};
    String table_name_{};
};

}
