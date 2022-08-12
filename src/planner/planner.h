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

#include "bind_context.h"

#include "storage/data_type.h"

#include "expression/base_expression.h"

namespace infinity {

class Planner {
public:
    static LogicalType TypeConversion(hsql::ColumnType type);

    std::shared_ptr<LogicalOperator> CreateLogicalOperator(const hsql::SQLStatement &statement);
private:
    // Create operator
    std::shared_ptr<LogicalOperator> BuildCreate(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildCreateTable(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildCreateTableFromTable(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildCreateView(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildCreateIndex(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Drop operator
    std::shared_ptr<LogicalOperator> BuildDrop(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildDropTable(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildDropSchema(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildDropIndex(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildDropView(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildDropPreparedStatement(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Insert operator
    std::shared_ptr<LogicalOperator> BuildInsert(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildInsertValue(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildInsertSelect(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Delete operator
    std::shared_ptr<LogicalOperator> BuildDelete(const hsql::DeleteStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Update operator
    std::shared_ptr<LogicalOperator> BuildUpdate(const hsql::UpdateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Select operator
    std::shared_ptr<LogicalOperator> BuildSelect(const hsql::SelectStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Show operator
    std::shared_ptr<LogicalOperator> BuildShow(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildShowColumns(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildShowTables(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Import operator
    std::shared_ptr<LogicalOperator> BuildImport(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildImportCsv(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildImportTbl(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildImportBinary(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildImportAuto(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Export operator
    std::shared_ptr<LogicalOperator> BuildExport(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildExportCsv(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildExportTbl(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildExportBinary(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildExportAuto(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Transaction operator
    std::shared_ptr<LogicalOperator> BuildTransaction(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildTransactionBegin(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildTransactionCommit(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildTransactionRollback(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Alter operator
    std::shared_ptr<LogicalOperator> BuildAlter(const hsql::AlterStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalOperator> BuildAlterDropColumn(const hsql::AlterStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Prepare operator
    std::shared_ptr<LogicalOperator> BuildPrepare(const hsql::PrepareStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Execute operator
    std::shared_ptr<LogicalOperator> BuildExecute(const hsql::ExecuteStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);


//    // Expression
//    std::shared_ptr<BaseExpression>
//    BuildExpression(const hsql::Expr& expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Build From clause
    std::shared_ptr<LogicalOperator>
    BuildFromClause(const hsql::TableRef* fromTable, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::vector<SelectListElement>
    BuildSelectList(const std::vector<hsql::Expr*>& select_list, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalOperator>
    BuildFilter(const hsql::Expr* where_clause, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalOperator>
    BuildGroupByHaving(
            const hsql::SelectStatement& select,
            const std::shared_ptr<BindContext>& bind_context_ptr,
            const std::shared_ptr<LogicalOperator>& root_operator);

    std::shared_ptr<LogicalOperator>
    BuildOrderBy(const std::vector<hsql::OrderDescription*>& order_by_clause, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalOperator>
    BuildLimit(const hsql::LimitDescription& limit_description, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalOperator>
    BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
             const hsql::LimitDescription& limit_description,
             const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalOperator>
    BuildTable(const hsql::TableRef* from_table, const std::shared_ptr<BindContext>& bind_context_ptr);

    uint64_t AppendOperator(std::shared_ptr<LogicalOperator> op, const std::shared_ptr<BindContext>& bind_context);
private:
    struct OperatorContext {
        std::shared_ptr<LogicalOperator> operator_{nullptr};
        uint64_t context_id_{0};
    };

    // All operators
    std::vector<OperatorContext> operator_array_;

    // Bind Contexts
    std::vector<std::shared_ptr<BindContext>> bind_contexts_;
    std::shared_ptr<BindContext> current_bind_context_ptr_;
};

}
