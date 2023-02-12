//
// Created by JinHai on 2022/9/7.
//

#include "statement.h"

#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

String
Statement::ExprAsColumnName(const hsql::Expr* expr) {
    switch(expr->type) {
        case hsql::kExprLiteralFloat: {
            return std::to_string(expr->fval);
        }
        case hsql::kExprLiteralString: {
            return expr->name;
        }
        case hsql::kExprLiteralInt: {
            return std::to_string(expr->ival);
        }
        case hsql::kExprLiteralNull: {
            return "NULL";
        }
        case hsql::kExprLiteralDate: {
            NotImplementError("No implementation of date")
        }
        case hsql::kExprLiteralInterval: {
            NotImplementError("No implementation of interval")
        }
        case hsql::kExprStar: {
            if(expr->table == nullptr) {
                return "*";
            } else {
                std::stringstream ss;
                ss << expr->table << ".*";
                return ss.str();
            }
        }
        case hsql::kExprParameter: {
            NotImplementError("No implementation of parameter")
        }
        case hsql::kExprColumnRef: {
            return expr->getName();
        }
        case hsql::kExprFunctionRef: {
            std::stringstream ss;
            SizeT argument_count = expr->exprList->size();
            ss << expr->getName() << "(";
            for(SizeT idx = 0; idx < argument_count; ++ idx) {
                ss << Statement::ExprAsColumnName((*(expr->exprList))[idx]);
                if(idx == argument_count - 1) {
                    // the last one
                    ss << ")";
                } else {
                    ss << ", ";
                }
            }
            return ss.str();
        }
        case hsql::kExprOperator: {
            std::stringstream ss;
            switch(expr->opType) {
                case hsql::kOpNone: {
                    NotImplementError("None operator")
                }
                case hsql::kOpBetween: {
                    ss << Statement::ExprAsColumnName(expr->expr) << " BETWEEN "
                       << Statement::ExprAsColumnName((*expr->exprList)[0]) << " AND "
                       << Statement::ExprAsColumnName((*expr->exprList)[1]);
                    break;
                }
                case hsql::kOpCase: {
                    NotImplementError("Case operator")
                }
                case hsql::kOpCaseListElement: {
                    NotImplementError("Cast List operator")
                }
                case hsql::kOpPlus: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " + " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpMinus: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " - " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpAsterisk: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " * " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpSlash: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " / " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpPercentage: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " % " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpCaret: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " ^ " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpEquals: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " = " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpNotEquals: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " <> " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpLess: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " < " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpLessEq: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " <= " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpGreater: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " > " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpGreaterEq: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " >= " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpLike: {
                    NotImplementError("LIKE operator")
                }
                case hsql::kOpNotLike: {
                    NotImplementError("NOT LIKE operator")
                }
                case hsql::kOpILike: {
                    NotImplementError("ILIKE operator")
                }
                case hsql::kOpAnd: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " AND " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpOr: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " OR " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpIn: {
                    ss << Statement::ExprAsColumnName(expr->expr)
                       << " IN " << Statement::ExprAsColumnName(expr->expr);
                    break;
                }
                case hsql::kOpConcat: {
                    NotImplementError("Concat operator")
                }
                case hsql::kOpNot: {
                    ss << "NOT(" << Statement::ExprAsColumnName(expr->expr) << ")";
                    break;
                }
                case hsql::kOpUnaryMinus: {
                    ss << "-(" << Statement::ExprAsColumnName(expr->expr) << ")";
                    break;
                }
                case hsql::kOpIsNull: {
                    ss << "ISNULL(" << Statement::ExprAsColumnName(expr->expr) << ")";
                    break;
                }
                case hsql::kOpExists: {
                    ss << "EXISTS(" << Statement::ExprAsColumnName(expr->expr) << ")";
                    break;
                }
            }
            return ss.str();
        }
        case hsql::kExprSelect: {
            NotImplementError("No implementation of subquery")
        }
        case hsql::kExprHint: {
            NotImplementError("No implementation of hint")
        }
        case hsql::kExprArray:{
            NotImplementError("No implementation of array")
        }
        case hsql::kExprArrayIndex:{
            NotImplementError("No implementation of array index")
        }
        case hsql::kExprExtract:{
            NotImplementError("No implementation of extract")
        }
        case hsql::kExprCast:{
            std::stringstream ss;
            ss << "CAST(" << Statement::ExprAsColumnName(expr->expr) << " AS " << Statement::ToString(expr->columnType) << ")";
            return ss.str();
        }
    }
    NotImplementError("Unexpected here")
}

String
Statement::ToString(hsql::ColumnType column_type) {
    switch(column_type.data_type) {
        case hsql::DataType::UNKNOWN: {
            NotImplementError("Unknown type")
        }
        case hsql::DataType::BIGINT: {
            return "BIGINT";
        }
        case hsql::DataType::BOOLEAN: {
            return "BOOLEAN";
        }
        case hsql::DataType::CHAR: {
            return "CHAR(" + std::to_string(column_type.length) + ")";
        }
        case hsql::DataType::DATE: {
            NotImplementError("Date type")
        }
        case hsql::DataType::DATETIME: {
            NotImplementError("DateTime type")
        }
        case hsql::DataType::DECIMAL: {
            return "DECIMAL(" + std::to_string(column_type.scale) + ", " + std::to_string(column_type.precision) + ")";
        }
        case hsql::DataType::DOUBLE: {
            return "DOUBLE";
        }
        case hsql::DataType::FLOAT: {
            return "FLOAT";
        }
        case hsql::DataType::INT: {
            return "INTEGER";
        }
        case hsql::DataType::LONG: {
            return "BIGINT";
        }
        case hsql::DataType::REAL: {
            return "FLOAT";
        }
        case hsql::DataType::SMALLINT: {
            return "SMALLINT";
        }
        case hsql::DataType::TEXT: {
            return "TEXT(" + std::to_string(column_type.length) + ")";
        }
        case hsql::DataType::TIME: {
            NotImplementError("Time type")
        }
        case hsql::DataType::VARCHAR: {
            return "VARCHAR(" + std::to_string(column_type.length) + ")";
        }
    }
    NotImplementError("Unexpected here")
}

String
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
                return String();
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
    return String();
}

String
Statement::ToString(hsql::SelectStatement* select_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::ImportStatement* import_stmt, size_t intent) {
    std::stringstream ss;
    ss << "Import statement: " << import_stmt->filePath
       << ", table: " << import_stmt->schema <<"." << import_stmt->tableName << std::endl;
    String space(intent, ' ');
    ss << space << "hints: " << std::endl;
    return String();
}

String
Statement::ToString(hsql::InsertStatement* insert_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::UpdateStatement* update_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::DeleteStatement* delete_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::CreateStatement* create_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::DropStatement* drop_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::PrepareStatement* prepare_stmt, size_t intent) {
    return String();
}
String
Statement::ToString(hsql::ExecuteStatement* execute_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::ExportStatement* export_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::AlterStatement* alter_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::ShowStatement* show_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::TransactionStatement* transaction_stmt, size_t intent) {
    return String();
}

String
Statement::ToString(hsql::Expr* expr, size_t intent) {
    return String();
}

}