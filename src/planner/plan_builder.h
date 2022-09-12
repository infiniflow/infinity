//
// Created by JinHai on 2022/9/11.
//

#pragma ocne

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

#include "bound/bound_select_node.h"
#include "bound/table_ref.h"
#include "bound/base_table_ref.h"
#include "bound/subquery_table_ref.h"
#include "bound/cross_product_table_ref.h"
#include "bound/join_table_ref.h"

//#include <memory>

namespace infinity {

class QueryContext;

struct PlanContext {
    std::shared_ptr<LogicalNode> plan;

    // Output of the last plan node.
    std::vector<LogicalType> types;
    std::vector<std::string> output_names;
};

class PlanBuilder : public std::enable_shared_from_this<PlanBuilder> {
public:
    explicit PlanBuilder(std::shared_ptr<QueryContext> query_context)
        : query_context_(std::move(query_context)) {}

    PlanContext BuildPlan(const hsql::SQLStatement &statement);

private:
    // Only return the root query builder bind context array;
    std::vector<std::shared_ptr<BindContext>>& BindContextArray();

    void AddBindContextArray(std::shared_ptr<BindContext>& bind_context_ptr);

private:
//    std::shared_ptr<PlanBuilder> parent_ptr_{nullptr};

    std::shared_ptr<QueryContext> query_context_;

    // Only the parent plan array will be used.
    std::vector<std::shared_ptr<BindContext>> bind_context_array_;

private:
    PlanContext BuildCreate(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildCreateTable(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildCreateTableFromTable(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildCreateView(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildCreateIndex(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Drop operator
    PlanContext BuildDrop(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildDropTable(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildDropSchema(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildDropIndex(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildDropView(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildDropPreparedStatement(const hsql::DropStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Insert operator
    PlanContext BuildInsert(const hsql::InsertStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildInsertValue(const hsql::InsertStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildInsertSelect(const hsql::InsertStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Delete operator
    PlanContext BuildDelete(const hsql::DeleteStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Update operator
    PlanContext BuildUpdate(const hsql::UpdateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Select operator
    std::shared_ptr<BoundSelectNode>
    BuildSelect(const hsql::SelectStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Show operator
    PlanContext BuildShow(const hsql::ShowStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildShowColumns(const hsql::ShowStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildShowTables(const hsql::ShowStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Import operator
    PlanContext BuildImport(const hsql::ImportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildImportCsv(const hsql::ImportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildImportTbl(const hsql::ImportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildImportBinary(const hsql::ImportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildImportAuto(const hsql::ImportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Export operator
    PlanContext BuildExport(const hsql::ExportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildExportCsv(const hsql::ExportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildExportTbl(const hsql::ExportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildExportBinary(const hsql::ExportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildExportAuto(const hsql::ExportStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Transaction operator
    PlanContext BuildTransaction(const hsql::TransactionStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildTransactionBegin(const hsql::TransactionStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildTransactionCommit(const hsql::TransactionStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildTransactionRollback(const hsql::TransactionStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Alter operator
    PlanContext BuildAlter(const hsql::AlterStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);
    PlanContext BuildAlterDropColumn(const hsql::AlterStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Prepare operator
    PlanContext BuildPrepare(const hsql::PrepareStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);

    // Execute operator
    PlanContext BuildExecute(const hsql::ExecuteStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr);


//    // Expression
//    std::shared_ptr<BaseExpression>
//    BuildExpression(const hsql::Expr& expr, std::shared_ptr<BindContext>& bind_context_ptr);

        // Build From clause
    std::shared_ptr<TableRef>
    BuildFromClause(const hsql::TableRef* fromTable, std::shared_ptr<BindContext>& bind_context_ptr);

    std::vector<SelectListElement>
    BuildSelectList(const std::vector<hsql::Expr*>& select_list, std::shared_ptr<BindContext>& bind_context_ptr);

    PlanContext
    BuildFilter(const hsql::Expr* where_clause, std::shared_ptr<BindContext>& bind_context_ptr);

    PlanContext
    BuildGroupByHaving(
            const hsql::SelectStatement& select,
            std::shared_ptr<BindContext>& bind_context_ptr,
            const std::shared_ptr<LogicalNode>& root_operator);

    PlanContext
    BuildOrderBy(const std::vector<hsql::OrderDescription*>& order_by_clause, std::shared_ptr<BindContext>& bind_context_ptr);

    PlanContext
    BuildLimit(const hsql::LimitDescription& limit_description, std::shared_ptr<BindContext>& bind_context_ptr);

    PlanContext
    BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
             const hsql::LimitDescription& limit_description,
             std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<TableRef>
    BuildTable(const hsql::TableRef* from_table, std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<TableRef>
    BuildSubquery(const hsql::SelectStatement select_stmt, std::shared_ptr<BindContext>& bind_context_ptr);

};

}




