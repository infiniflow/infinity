//
// Created by JinHai on 2022/9/7.
//

#pragma once

#include "SQLParser.h"
#include "SQLParserResult.h"

namespace infinity {

class Statement {
public:
    explicit Statement(hsql::SQLStatement* statement) : statement_(statement) {};
    std::string ToString(size_t intent = 0);

private:
    std::string ToString(hsql::SelectStatement* select_stmt, size_t intent);
    std::string ToString(hsql::ImportStatement* import_stmt, size_t intent);
    std::string ToString(hsql::InsertStatement* insert_stmt, size_t intent);
    std::string ToString(hsql::UpdateStatement* update_stmt, size_t intent);
    std::string ToString(hsql::DeleteStatement* delete_stmt, size_t intent);
    std::string ToString(hsql::CreateStatement* create_stmt, size_t intent);
    std::string ToString(hsql::DropStatement* drop_stmt, size_t intent);
    std::string ToString(hsql::PrepareStatement* prepare_stmt, size_t intent);
    std::string ToString(hsql::ExecuteStatement* execute_stmt, size_t intent);
    std::string ToString(hsql::ExportStatement* export_stmt, size_t intent);
    std::string ToString(hsql::AlterStatement* alter_stmt, size_t intent);
    std::string ToString(hsql::ShowStatement* show_stmt, size_t intent);
    std::string ToString(hsql::TransactionStatement* transaction_stmt, size_t intent);
    std::string ToString(hsql::Expr* expr, size_t intent);

private:
    hsql::SQLStatement* statement_{nullptr};
};

}
