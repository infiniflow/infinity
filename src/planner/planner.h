//
// Created by JinHai on 2022/7/23.
//

#pragma once


#include "logical_node.h"
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

    std::shared_ptr<LogicalNode> CreateLogicalOperator(const hsql::SQLStatement &statement);
private:
    // Create operator
    std::shared_ptr<LogicalNode> BuildCreate(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildCreateTable(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildCreateTableFromTable(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildCreateView(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildCreateIndex(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Drop operator
    std::shared_ptr<LogicalNode> BuildDrop(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildDropTable(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildDropSchema(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildDropIndex(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildDropView(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildDropPreparedStatement(const hsql::DropStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Insert operator
    std::shared_ptr<LogicalNode> BuildInsert(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildInsertValue(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildInsertSelect(const hsql::InsertStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Delete operator
    std::shared_ptr<LogicalNode> BuildDelete(const hsql::DeleteStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Update operator
    std::shared_ptr<LogicalNode> BuildUpdate(const hsql::UpdateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Select operator
    std::shared_ptr<LogicalNode> BuildSelect(const hsql::SelectStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Show operator
    std::shared_ptr<LogicalNode> BuildShow(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildShowColumns(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildShowTables(const hsql::ShowStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Import operator
    std::shared_ptr<LogicalNode> BuildImport(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildImportCsv(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildImportTbl(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildImportBinary(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildImportAuto(const hsql::ImportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Export operator
    std::shared_ptr<LogicalNode> BuildExport(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildExportCsv(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildExportTbl(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildExportBinary(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildExportAuto(const hsql::ExportStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Transaction operator
    std::shared_ptr<LogicalNode> BuildTransaction(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildTransactionBegin(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildTransactionCommit(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildTransactionRollback(const hsql::TransactionStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Alter operator
    std::shared_ptr<LogicalNode> BuildAlter(const hsql::AlterStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);
    std::shared_ptr<LogicalNode> BuildAlterDropColumn(const hsql::AlterStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Prepare operator
    std::shared_ptr<LogicalNode> BuildPrepare(const hsql::PrepareStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Execute operator
    std::shared_ptr<LogicalNode> BuildExecute(const hsql::ExecuteStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr);


//    // Expression
//    std::shared_ptr<BaseExpression>
//    BuildExpression(const hsql::Expr& expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Build From clause
    std::shared_ptr<LogicalNode>
    BuildFromClause(const hsql::TableRef* fromTable, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::vector<SelectListElement>
    BuildSelectList(const std::vector<hsql::Expr*>& select_list, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalNode>
    BuildFilter(const hsql::Expr* where_clause, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalNode>
    BuildGroupByHaving(
            const hsql::SelectStatement& select,
            const std::shared_ptr<BindContext>& bind_context_ptr,
            const std::shared_ptr<LogicalNode>& root_operator);

    std::shared_ptr<LogicalNode>
    BuildOrderBy(const std::vector<hsql::OrderDescription*>& order_by_clause, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalNode>
    BuildLimit(const hsql::LimitDescription& limit_description, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalNode>
    BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
             const hsql::LimitDescription& limit_description,
             const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<LogicalNode>
    BuildTable(const hsql::TableRef* from_table, const std::shared_ptr<BindContext>& bind_context_ptr);

    uint64_t AppendOperator(std::shared_ptr<LogicalNode> op, const std::shared_ptr<BindContext>& bind_context);
private:
    struct OperatorContext {
        std::shared_ptr<LogicalNode> operator_{nullptr};
        uint64_t context_id_{0};
    };

    // All operators
    std::vector<OperatorContext> operator_array_;

    // Bind Contexts
    std::vector<std::shared_ptr<BindContext>> bind_contexts_;
    std::shared_ptr<BindContext> current_bind_context_ptr_;
};

}
