//
// Created by jinhai on 23-2-22.
//

#pragma once

#include "base_statement.h"

namespace infinity {

enum class ShowStmtType {
    kColumns,
    kTables,
    kCollections,
    kViews,
    kIndexes,
};

class ShowStatement : public BaseStatement {
public:
    explicit ShowStatement() : BaseStatement(StatementType::kShow) {}

    [[nodiscard]] std::string ToString() const final;

    ShowStmtType show_type_{ShowStmtType::kTables};
    std::string schema_name_{"default"};
    std::string table_name_{};
};

} // namespace infinity
