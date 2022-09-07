//
// Created by JinHai on 2022/9/7.
//

#include "statement.h"

#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

std::string
Statement::ToString(size_t intent) {
    std::stringstream ss;
    switch(statement_->type()) {
        case hsql::kStmtError:
            ParserError("Unsupported error statement");
        case hsql::kStmtSelect: {
            hsql::SelectStatement* select_stmt = dynamic_cast<hsql::SelectStatement*>(statement_);
            if(select_stmt == nullptr) {
                ParserError("SelectStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtImport: {
            hsql::ImportStatement* import_stmt = dynamic_cast<hsql::ImportStatement*>(statement_);
            if(import_stmt == nullptr) {
                ParserError("ImportStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtInsert: {
            hsql::InsertStatement* insert_stmt = dynamic_cast<hsql::InsertStatement*>(statement_);
            if(insert_stmt == nullptr) {
                ParserError("InsertStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtUpdate: {
            hsql::UpdateStatement* update_stmt = dynamic_cast<hsql::UpdateStatement*>(statement_);
            if(update_stmt == nullptr) {
                ParserError("UpdateStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtDelete: {
            hsql::DeleteStatement* delete_stmt = dynamic_cast<hsql::DeleteStatement*>(statement_);
            if(delete_stmt == nullptr) {
                ParserError("DeleteStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtCreate: {
            hsql::CreateStatement* create_stmt = dynamic_cast<hsql::CreateStatement*>(statement_);
            if(create_stmt == nullptr) {
                ParserError("CreateStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtDrop: {
            hsql::DropStatement* drop_stmt = dynamic_cast<hsql::DropStatement*>(statement_);
            if(drop_stmt == nullptr) {
                ParserError("DropStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtPrepare: {
            hsql::PrepareStatement* prepare_stmt = dynamic_cast<hsql::PrepareStatement*>(statement_);
            if(prepare_stmt == nullptr) {
                ParserError("PrepareStatement: invalid statement type");
            }
            return ToString(prepare_stmt, intent);
        }
        case hsql::kStmtExecute: {
            hsql::ExecuteStatement* execute_stmt = dynamic_cast<hsql::ExecuteStatement*>(statement_);
            if(execute_stmt == nullptr) {
                ParserError("ExecuteStatement: invalid statement type");
            }
            return ToString(execute_stmt, intent);
        }
        case hsql::kStmtExport: {
            hsql::ExportStatement* export_stmt = dynamic_cast<hsql::ExportStatement*>(statement_);
            if(export_stmt == nullptr) {
                ParserError("ExportStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtRename:
            ParserError("Unsupported rename statement");
        case hsql::kStmtAlter: {
            hsql::AlterStatement* alter_stmt = dynamic_cast<hsql::AlterStatement*>(statement_);
            if(alter_stmt == nullptr) {
                ParserError("AlterStatement: invalid statement type");
                return std::string();
            }
            break;
        }
        case hsql::kStmtShow: {
            hsql::ShowStatement* show_stmt = dynamic_cast<hsql::ShowStatement*>(statement_);
            if(show_stmt == nullptr) {
                ParserError("ImportStatement: invalid statement type");
            }
            break;
        }
        case hsql::kStmtTransaction: {
            hsql::TransactionStatement* transaction_stmt = dynamic_cast<hsql::TransactionStatement*>(statement_);
            if(transaction_stmt == nullptr) {
                ParserError("TransactionStatement: invalid statement type");
            }
            break;
        }
    }
    return std::string();
}

std::string
Statement::ToString(hsql::SelectStatement* select_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::ImportStatement* import_stmt, size_t intent) {
    std::stringstream ss;
    ss << "Import statement: " << import_stmt->filePath
       << ", table: " << import_stmt->schema <<"." << import_stmt->tableName << std::endl;
    std::string space(intent, ' ');
    ss << space << "hints: " << std::endl;
    return std::string();
}

std::string
Statement::ToString(hsql::InsertStatement* insert_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::UpdateStatement* update_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::DeleteStatement* delete_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::CreateStatement* create_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::DropStatement* drop_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::PrepareStatement* prepare_stmt, size_t intent) {
    return std::string();
}
std::string
Statement::ToString(hsql::ExecuteStatement* execute_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::ExportStatement* export_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::AlterStatement* alter_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::ShowStatement* show_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::TransactionStatement* transaction_stmt, size_t intent) {
    return std::string();
}

std::string
Statement::ToString(hsql::Expr* expr, size_t intent) {
    return std::string();
}

}