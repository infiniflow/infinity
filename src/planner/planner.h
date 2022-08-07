//
// Created by JinHai on 2022/7/23.
//

#pragma once


#include "logical_operator.h"
#include "SQLParserResult.h"
#include "sql/CreateStatement.h"
#include "sql/DropStatement.h"
#include "sql/InsertStatement.h"
#include "sql/DeleteStatement.h"
#include "sql/UpdateStatement.h"
#include "sql/SelectStatement.h"
#include "sql/ShowStatement.h"
#include "sql/ImportStatement.h"
#include "sql/ExportStatement.h"
#include "sql/TransactionStatement.h"
#include "sql/AlterStatement.h"
#include "sql/PrepareStatement.h"
#include "sql/ExecuteStatement.h"

#include "storage/data_type.h"

#include "expression/base_expression.h"

namespace infinity {

class Planner {
public:
    static LogicalType TypeConversion(hsql::ColumnType type);

    std::shared_ptr<LogicalOperator> CreateLogicalOperator(const hsql::SQLStatement &statement);
private:
    // Create operator
    std::shared_ptr<LogicalOperator> BuildCreate(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateTable(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateTableFromTable(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateView(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateIndex(const hsql::CreateStatement& statement);

    // Drop operator
    std::shared_ptr<LogicalOperator> BuildDrop(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropTable(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropSchema(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropIndex(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropView(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropPreparedStatement(const hsql::DropStatement& statement);

    // Insert operator
    std::shared_ptr<LogicalOperator> BuildInsert(const hsql::InsertStatement& statement);
    std::shared_ptr<LogicalOperator> BuildInsertValue(const hsql::InsertStatement& statement);
    std::shared_ptr<LogicalOperator> BuildInsertSelect(const hsql::InsertStatement& statement);

    // Delete operator
    std::shared_ptr<LogicalOperator> BuildDelete(const hsql::DeleteStatement& statement);

    // Update operator
    std::shared_ptr<LogicalOperator> BuildUpdate(const hsql::UpdateStatement& statement);

    // Select operator
    std::shared_ptr<LogicalOperator> BuildSelect(const hsql::SelectStatement& statement);

    // Show operator
    std::shared_ptr<LogicalOperator> BuildShow(const hsql::ShowStatement& statement);
    std::shared_ptr<LogicalOperator> BuildShowColumns(const hsql::ShowStatement& statement);
    std::shared_ptr<LogicalOperator> BuildShowTables(const hsql::ShowStatement& statement);

    // Import operator
    std::shared_ptr<LogicalOperator> BuildImport(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildImportCsv(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildImportTbl(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildImportBinary(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildImportAuto(const hsql::ImportStatement& statement);

    // Export operator
    std::shared_ptr<LogicalOperator> BuildExport(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildExportCsv(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildExportTbl(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildExportBinary(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> BuildExportAuto(const hsql::ExportStatement& statement);

    // Transaction operator
    std::shared_ptr<LogicalOperator> BuildTransaction(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> BuildTransactionBegin(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> BuildTransactionCommit(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> BuildTransactionRollback(const hsql::TransactionStatement& statement);

    // Alter operator
    std::shared_ptr<LogicalOperator> BuildAlter(const hsql::AlterStatement& statement);
    std::shared_ptr<LogicalOperator> BuildAlterDropColumn(const hsql::AlterStatement& statement);

    // Prepare operator
    std::shared_ptr<LogicalOperator> BuildPrepare(const hsql::PrepareStatement& statement);

    // Execute operator
    std::shared_ptr<LogicalOperator> BuildExecute(const hsql::ExecuteStatement& statement);

    // Expression
    std::shared_ptr<BaseExpression> BuildExpression(const hsql::Expr& expr);
};

}
