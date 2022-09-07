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
    std::string ToString();

private:
    std::string ToString(hsql::SelectStatement* select_stmt);
    std::string ToString(hsql::ImportStatement* import_stmt);
    std::string ToString(hsql::InsertStatement* insert_stmt);
    std::string ToString(hsql::UpdateStatement* update_stmt);
    std::string ToString(hsql::DeleteStatement* delete_stmt);
    std::string ToString(hsql::CreateStatement* create_stmt);
    std::string ToString(hsql::DropStatement* drop_stmt);
    std::string ToString(hsql::PrepareStatement* prepare_stmt);
    std::string ToString(hsql::ExecuteStatement* execute_stmt);
    std::string ToString(hsql::ExportStatement* export_stmt);
    std::string ToString(hsql::AlterStatement* alter_stmt);
    std::string ToString(hsql::ShowStatement* show_stmt);
    std::string ToString(hsql::TransactionStatement* transaction_stmt);

private:
    hsql::SQLStatement* statement_{nullptr};
};

}
