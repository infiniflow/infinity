//
// Created by JinHai on 2022/9/11.
//

#pragma ocne

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
#include "logical_node.h"
#include "plan_building_context.h"
#include "planner/node/logical_filter.h"

#include "binder/bind_alias_proxy.h"

#include "bound/bound_select_node.h"
#include "bound/table_ref.h"
#include "bound/base_table_ref.h"
#include "bound/subquery_table_ref.h"
#include "bound/cross_product_table_ref.h"
#include "bound/join_table_ref.h"

#include "storage/view.h"

//#include <memory>

namespace infinity {

class QueryContext;

class PlanBuilder : public std::enable_shared_from_this<PlanBuilder> {
public:
//    explicit PlanBuilder(std::shared_ptr<QueryContext> query_context)
//            : query_context_(std::move(query_context)) {}

    static PlanBuildingContext
    BuildPlan(const hsql::SQLStatement &statement);

    // Select operator
    static std::shared_ptr<BoundSelectNode>
    BuildSelect(const hsql::SelectStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

//    std::vector<std::shared_ptr<BindContext>> &BindContextArray();

//    void AddBindContextArray(std::shared_ptr<BindContext> &bind_context_ptr);

//    std::shared_ptr<PlanBuilder> parent_ptr_{nullptr};

//    std::shared_ptr<QueryContext> query_context_;

//    // Only the parent plan array will be used.
//    std::vector<std::shared_ptr<BindContext>> bind_context_array_;

private:

    static PlanBuildingContext
    BuildCreate(const hsql::CreateStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateTable(const hsql::CreateStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateTableFromTable(const hsql::CreateStatement &statement,
                                                  std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateView(const hsql::CreateStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateIndex(const hsql::CreateStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Drop operator
    static PlanBuildingContext
    BuildDrop(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropTable(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropSchema(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropIndex(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropView(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropPreparedStatement(const hsql::DropStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Insert operator
    static PlanBuildingContext
    BuildInsert(const hsql::InsertStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildInsertValue(const hsql::InsertStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildInsertSelect(const hsql::InsertStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Delete operator
    static PlanBuildingContext
    BuildDelete(const hsql::DeleteStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Update operator
    static PlanBuildingContext
    BuildUpdate(const hsql::UpdateStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Show operator
    static PlanBuildingContext
    BuildShow(const hsql::ShowStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildShowColumns(const hsql::ShowStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildShowTables(const hsql::ShowStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Import operator
    static PlanBuildingContext
    BuildImport(const hsql::ImportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportCsv(const hsql::ImportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportTbl(const hsql::ImportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportBinary(const hsql::ImportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportAuto(const hsql::ImportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Export operator
    static PlanBuildingContext
    BuildExport(const hsql::ExportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportCsv(const hsql::ExportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportTbl(const hsql::ExportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportBinary(const hsql::ExportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportAuto(const hsql::ExportStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Transaction operator
    static PlanBuildingContext
    BuildTransaction(const hsql::TransactionStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionBegin(const hsql::TransactionStatement &statement,
                                              std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionCommit(const hsql::TransactionStatement &statement,
                                               std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionRollback(const hsql::TransactionStatement &statement,
                                                 std::shared_ptr<BindContext> &bind_context_ptr);

    // Alter operator
    static PlanBuildingContext
    BuildAlter(const hsql::AlterStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildAlterDropColumn(const hsql::AlterStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Prepare operator
    static PlanBuildingContext
    BuildPrepare(const hsql::PrepareStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Execute operator
    static PlanBuildingContext
    BuildExecute(const hsql::ExecuteStatement &statement, std::shared_ptr<BindContext> &bind_context_ptr);


//    // Expression
//    std::shared_ptr<BaseExpression>
//    BuildExpression(const hsql::Expr& expr, std::shared_ptr<BindContext>& bind_context_ptr);

    // Build From clause
    static std::shared_ptr<TableRef>
    BuildFromClause(const hsql::TableRef *fromTable, std::shared_ptr<BindContext> &bind_context_ptr);

    static std::vector<SelectItem>
    BuildSelectList(const std::vector<hsql::Expr *> &select_list, std::shared_ptr<BindContext> &bind_context_ptr);


    static std::shared_ptr<LogicalFilter>
    BuildFilter(const hsql::Expr *where_clause,
                const std::shared_ptr<BindAliasProxy>& bind_alias_proxy,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static void
    BuildGroupByHaving(const hsql::SelectStatement& select,
                       const std::shared_ptr<BindAliasProxy>& bind_alias_proxy,
                       std::shared_ptr<BindContext>& bind_context_ptr,
                       std::shared_ptr<BoundSelectNode>& root_operator);

    static PlanBuildingContext
    BuildOrderBy(const std::vector<hsql::OrderDescription *> &order_by_clause,
                 std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildLimit(const hsql::LimitDescription &limit_description, std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTop(const std::vector<hsql::OrderDescription *> &order_by_clause,
             const hsql::LimitDescription &limit_description,
             std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildTable(const hsql::TableRef *from_table, std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildSubquery(const std::string &name, const hsql::SelectStatement &select_stmt,
                  std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildCTE(const std::string &name, const std::shared_ptr<CommonTableExpressionInfo> &cte,
             std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildView(const std::string &name, const std::shared_ptr<View> &view,
              std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildCrossProduct(const hsql::TableRef *from_table, std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildJoin(const hsql::TableRef *from_table, std::shared_ptr<BindContext> &bind_context_ptr);
};

}




