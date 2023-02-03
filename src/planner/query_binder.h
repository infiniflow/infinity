//
// Created by jinhai on 23-1-17.
//

#pragma once

#include <utility>

#include "parser/parsed_expression.h"
#include "planner/bind_context.h"
#include "planner/select_expression.h"
#include "bound_select_statement.h"
#include "sql/SelectStatement.h"
#include "storage/view.h"
#include "planner/binder/bind_alias_proxy.h"

namespace infinity {

class QueryBinder : public std::enable_shared_from_this<QueryBinder> {
public:
    explicit
    QueryBinder(SharedPtr<QueryContext> query_context, SharedPtr<BindContext> bind_context_ptr)
        : query_context_ptr_(std::move(query_context)), bind_context_ptr_(std::move(bind_context_ptr)) {}

    SharedPtr<BoundSelectStatement>
    BindSelect(const hsql::SelectStatement& statement);

    SharedPtr<QueryContext> query_context_ptr_;

    SharedPtr<BindContext> bind_context_ptr_;

private:
    SharedPtr<TableRef>
    BuildFromClause(SharedPtr<QueryContext>& query_context,
                    const hsql::TableRef *from_table);

    SharedPtr<TableRef>
    BuildDummyTable(SharedPtr<QueryContext>& query_context);

    SharedPtr<TableRef>
    BuildTable(SharedPtr<QueryContext>& query_context,
               const hsql::TableRef *from_table);

    SharedPtr<TableRef>
    BuildSubquery(SharedPtr<QueryContext>& query_context,
                  const String &table_alias,
                  const hsql::SelectStatement &select_stmt);

    SharedPtr<TableRef>
    BuildCTE(SharedPtr<QueryContext>& query_context,
             const String &name);

    SharedPtr<TableRef>
    BuildBaseTable(SharedPtr<QueryContext>& query_context,
                   const hsql::TableRef* from_table,
                   const String &schema_name);

    SharedPtr<TableRef>
    BuildView(SharedPtr<QueryContext>& query_context,
              const hsql::TableRef* from_table,
              const String &schema_name);

    SharedPtr<TableRef>
    BuildCrossProduct(SharedPtr<QueryContext>& query_context,
                      const hsql::TableRef *from_table);

    SharedPtr<TableRef>
    BuildJoin(SharedPtr<QueryContext>& query_context,
              const hsql::TableRef *from_table);

    Vector<SharedPtr<ParsedExpression>>
    UnfoldStarExpression(SharedPtr<QueryContext>& query_context,
                         const Vector<hsql::Expr *> &select_list);

    void
    BuildGroupByHaving(SharedPtr<QueryContext>& query_context,
                       const hsql::SelectStatement& select,
                       const SharedPtr<BindAliasProxy>& bind_alias_proxy,
                       SharedPtr<BoundSelectStatement>& select_statement);

    void
    BuildSelectList(SharedPtr<QueryContext>& query_context,
                    SharedPtr<BoundSelectStatement>& select_statement);

    void
    BuildOrderBy(SharedPtr<QueryContext>& query_context,
                 const hsql::SelectStatement& statement,
                 SharedPtr<BoundSelectStatement>& bound_statement);

    void
    BuildLimit(SharedPtr<QueryContext>& query_context,
               const hsql::SelectStatement& statement,
               SharedPtr<BoundSelectStatement>& bound_statement);
};

}
