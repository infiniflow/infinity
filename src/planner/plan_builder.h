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

#include "storage/view.h"

namespace infinity {

class QueryContext;

class PlanBuilder : public std::enable_shared_from_this<PlanBuilder> {
public:
//    explicit PlanBuilder(std::shared_ptr<QueryContext> query_context)
//            : query_context_(std::move(query_context)) {}

    static PlanBuildingContext
    BuildPlan(std::shared_ptr<QueryContext>& query_context,
              const hsql::SQLStatement &statement);

    // Select operator
    static std::shared_ptr<BoundSelectNode>
    BuildSelect(std::shared_ptr<QueryContext>& query_context,
                const hsql::SelectStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

//    std::vector<std::shared_ptr<BindContext>> &BindContextArray();

//    void AddBindContextArray(std::shared_ptr<BindContext> &bind_context_ptr);

//    std::shared_ptr<PlanBuilder> parent_ptr_{nullptr};

//    std::shared_ptr<QueryContext> query_context_;

//    // Only the parent plan array will be used.
//    std::vector<std::shared_ptr<BindContext>> bind_context_array_;

private:

    static PlanBuildingContext
    BuildCreate(std::shared_ptr<QueryContext>& query_context,
                const hsql::CreateStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateTable(std::shared_ptr<QueryContext>& query_context,
                     const hsql::CreateStatement &statement,
                     std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateTableFromTable(std::shared_ptr<QueryContext>& query_context,
                              const hsql::CreateStatement &statement,
                              std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateView(std::shared_ptr<QueryContext>& query_context,
                    const hsql::CreateStatement &statement,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildCreateIndex(std::shared_ptr<QueryContext>& query_context,
                     const hsql::CreateStatement &statement,
                     std::shared_ptr<BindContext> &bind_context_ptr);

    // Drop operator
    static PlanBuildingContext
    BuildDrop(std::shared_ptr<QueryContext>& query_context,
              const hsql::DropStatement &statement,
              std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropTable(std::shared_ptr<QueryContext>& query_context,
                   const hsql::DropStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropSchema(std::shared_ptr<QueryContext>& query_context,
                    const hsql::DropStatement &statement,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropIndex(std::shared_ptr<QueryContext>& query_context,
                   const hsql::DropStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropView(std::shared_ptr<QueryContext>& query_context,
                  const hsql::DropStatement &statement,
                  std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildDropPreparedStatement(std::shared_ptr<QueryContext>& query_context,
                               const hsql::DropStatement &statement,
                               std::shared_ptr<BindContext> &bind_context_ptr);

    // Insert operator
    static PlanBuildingContext
    BuildInsert(std::shared_ptr<QueryContext>& query_context,
                const hsql::InsertStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildInsertValue(std::shared_ptr<QueryContext>& query_context,
                     const hsql::InsertStatement &statement,
                     std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildInsertSelect(std::shared_ptr<QueryContext>& query_context,
                      const hsql::InsertStatement &statement,
                      std::shared_ptr<BindContext> &bind_context_ptr);

    // Delete operator
    static PlanBuildingContext
    BuildDelete(std::shared_ptr<QueryContext>& query_context,
                const hsql::DeleteStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    // Update operator
    static PlanBuildingContext
    BuildUpdate(std::shared_ptr<QueryContext>& query_context,
                const hsql::UpdateStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    // Show operator
    static PlanBuildingContext
    BuildShow(std::shared_ptr<QueryContext>& query_context,
              const hsql::ShowStatement &statement,
              std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildShowColumns(std::shared_ptr<QueryContext>& query_context,
                     const hsql::ShowStatement &statement,
                     std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildShowTables(std::shared_ptr<QueryContext>& query_context,
                    const hsql::ShowStatement &statement,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    // Import operator
    static PlanBuildingContext
    BuildImport(std::shared_ptr<QueryContext>& query_context,
                const hsql::ImportStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportCsv(std::shared_ptr<QueryContext>& query_context,
                   const hsql::ImportStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportTbl(std::shared_ptr<QueryContext>& query_context,
                   const hsql::ImportStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportBinary(std::shared_ptr<QueryContext>& query_context,
                      const hsql::ImportStatement &statement,
                      std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildImportAuto(std::shared_ptr<QueryContext>& query_context,
                    const hsql::ImportStatement &statement,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    // Export operator
    static PlanBuildingContext
    BuildExport(std::shared_ptr<QueryContext>& query_context,
                const hsql::ExportStatement &statement,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportCsv(std::shared_ptr<QueryContext>& query_context,
                   const hsql::ExportStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportTbl(std::shared_ptr<QueryContext>& query_context,
                   const hsql::ExportStatement &statement,
                   std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportBinary(std::shared_ptr<QueryContext>& query_context,
                      const hsql::ExportStatement &statement,
                      std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildExportAuto(std::shared_ptr<QueryContext>& query_context,
                    const hsql::ExportStatement &statement,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    // Transaction operator
    static PlanBuildingContext
    BuildTransaction(std::shared_ptr<QueryContext>& query_context,
                     const hsql::TransactionStatement &statement,
                     std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionBegin(std::shared_ptr<QueryContext>& query_context,
                          const hsql::TransactionStatement &statement,
                          std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionCommit(std::shared_ptr<QueryContext>& query_context,
                           const hsql::TransactionStatement &statement,
                           std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTransactionRollback(std::shared_ptr<QueryContext>& query_context,
                             const hsql::TransactionStatement &statement,
                             std::shared_ptr<BindContext> &bind_context_ptr);

    // Alter operator
    static PlanBuildingContext
    BuildAlter(std::shared_ptr<QueryContext>& query_context,
               const hsql::AlterStatement &statement,
               std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildAlterDropColumn(std::shared_ptr<QueryContext>& query_context,
                         const hsql::AlterStatement &statement,
                         std::shared_ptr<BindContext> &bind_context_ptr);

    // Prepare operator
    static PlanBuildingContext
    BuildPrepare(std::shared_ptr<QueryContext>& query_context,
                 const hsql::PrepareStatement &statement,
                 std::shared_ptr<BindContext> &bind_context_ptr);

    // Execute operator
    static PlanBuildingContext
    BuildExecute(std::shared_ptr<QueryContext>& query_context,
                 const hsql::ExecuteStatement &statement,
                 std::shared_ptr<BindContext> &bind_context_ptr);


//    // Expression
//    std::shared_ptr<BaseExpression>
//    BuildExpression(const hsql::Expr& expr, std::shared_ptr<BindContext>& bind_context_ptr);

    // Build From clause
    static std::shared_ptr<TableRef>
    BuildFromClause(std::shared_ptr<QueryContext>& query_context,
                    const hsql::TableRef *fromTable,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    static std::vector<SelectItem>
    BuildSelectList(std::shared_ptr<QueryContext>& query_context,
                    const std::vector<hsql::Expr *> &select_list,
                    std::shared_ptr<BindContext> &bind_context_ptr);


    static std::shared_ptr<LogicalFilter>
    BuildFilter(std::shared_ptr<QueryContext>& query_context,
                const hsql::Expr *where_clause,
                const std::shared_ptr<BindAliasProxy>& bind_alias_proxy,
                std::shared_ptr<BindContext> &bind_context_ptr);

    static void
    BuildGroupByHaving(std::shared_ptr<QueryContext>& query_context,
                       const hsql::SelectStatement& select,
                       const std::shared_ptr<BindAliasProxy>& bind_alias_proxy,
                       std::shared_ptr<BindContext>& bind_context_ptr,
                       std::shared_ptr<BoundSelectNode>& root_operator);

    static PlanBuildingContext
    BuildOrderBy(std::shared_ptr<QueryContext>& query_context,
                 const std::vector<hsql::OrderDescription *> &order_by_clause,
                 std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildLimit(std::shared_ptr<QueryContext>& query_context,
               const hsql::LimitDescription &limit_description,
               std::shared_ptr<BindContext> &bind_context_ptr);

    static PlanBuildingContext
    BuildTop(std::shared_ptr<QueryContext>& query_context,
             const std::vector<hsql::OrderDescription *> &order_by_clause,
             const hsql::LimitDescription &limit_description,
             std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildDummyTable(std::shared_ptr<QueryContext>& query_context,
                    std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildTable(std::shared_ptr<QueryContext>& query_context,
               const hsql::TableRef *from_table,
               std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildSubquery(std::shared_ptr<QueryContext>& query_context,
                  const std::string &name,
                  const hsql::SelectStatement &select_stmt,
                  std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildCTE(std::shared_ptr<QueryContext>& query_context,
             const std::string &name,
             const std::shared_ptr<CommonTableExpressionInfo> &cte,
             std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildView(std::shared_ptr<QueryContext>& query_context,
              const std::string &name,
              const std::shared_ptr<View> &view,
              std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildCrossProduct(std::shared_ptr<QueryContext>& query_context,
                      const hsql::TableRef *from_table,
                      std::shared_ptr<BindContext> &bind_context_ptr);

    static std::shared_ptr<TableRef>
    BuildJoin(std::shared_ptr<QueryContext>& query_context,
              const hsql::TableRef *from_table,
              std::shared_ptr<BindContext> &bind_context_ptr);
};

}




