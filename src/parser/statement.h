//
// Created by JinHai on 2022/9/7.
//

#pragma once

#include "common/types/internal_types.h"
#include "SQLParser.h"
#include "SQLParserResult.h"

namespace infinity {

class Statement {
public:
    static String
    ExprAsColumnName(const hsql::Expr* expr);

private:
    static String
    ToString(hsql::ColumnType column_type);

public:
    explicit
    Statement(hsql::SQLStatement* statement) : statement_(statement) {};

    String
    ToString(size_t intent = 0);

private:
    String
    ToString(hsql::SelectStatement* select_stmt, size_t intent);

    String
    ToString(hsql::ImportStatement* import_stmt, size_t intent);

    String
    ToString(hsql::InsertStatement* insert_stmt, size_t intent);

    String
    ToString(hsql::UpdateStatement* update_stmt, size_t intent);

    String
    ToString(hsql::DeleteStatement* delete_stmt, size_t intent);

    String
    ToString(hsql::CreateStatement* create_stmt, size_t intent);

    String
    ToString(hsql::DropStatement* drop_stmt, size_t intent);

    String
    ToString(hsql::PrepareStatement* prepare_stmt, size_t intent);

    String
    ToString(hsql::ExecuteStatement* execute_stmt, size_t intent);

    String
    ToString(hsql::ExportStatement* export_stmt, size_t intent);

    String
    ToString(hsql::AlterStatement* alter_stmt, size_t intent);

    String
    ToString(hsql::ShowStatement* show_stmt, size_t intent);

    String
    ToString(hsql::TransactionStatement* transaction_stmt, size_t intent);

    String
    ToString(hsql::Expr* expr, size_t intent);

private:
    hsql::SQLStatement* statement_{nullptr};
};

}
