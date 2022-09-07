//
// Created by JinHai on 2022/9/7.
//

#include "statement.h"

#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

std::string
Statement::ToString() {
    std::stringstream ss;
    switch(statement_->type()) {
        case hsql::kStmtError:
            ParserError("Unsupported error statement");
            break;
        case hsql::kStmtSelect:

            break;
        case hsql::kStmtImport:
            break;
        case hsql::kStmtInsert:
            break;
        case hsql::kStmtUpdate:
            break;
        case hsql::kStmtDelete:
            break;
        case hsql::kStmtCreate:
            break;
        case hsql::kStmtDrop:
            break;
        case hsql::kStmtPrepare:
            break;
        case hsql::kStmtExecute:
            break;
        case hsql::kStmtExport:
            break;
        case hsql::kStmtRename:
            ParserError("Unsupported rename statement");
            break;
        case hsql::kStmtAlter: {
            hsql::AlterStatement* alter_stmt = dynamic_cast<hsql::AlterStatement*>(statement_);
            if(alter_stmt == nullptr) {
                ParserError("AlterStatement: invalid statement type");
                return std::string();
            }
            break;
        }
        case hsql::kStmtShow:
            break;
        case hsql::kStmtTransaction:
            break;
    }
    return std::string();
}

std::string
Statement::ToString(hsql::SelectStatement* select_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::ImportStatement* import_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::InsertStatement* insert_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::UpdateStatement* update_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::DeleteStatement* delete_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::CreateStatement* create_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::DropStatement* drop_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::PrepareStatement* prepare_stmt) {
    return std::string();
}
std::string
Statement::ToString(hsql::ExecuteStatement* execute_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::ExportStatement* export_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::AlterStatement* alter_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::ShowStatement* show_stmt) {
    return std::string();
}

std::string
Statement::ToString(hsql::TransactionStatement* transaction_stmt) {
    return std::string();
}

}