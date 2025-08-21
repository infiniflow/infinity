// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module infinity_core:query_binder;

import :logical_node;
import :query_context;
import :bound_select_statement;
import :bind_context;
import :table_ref;
import :base_table_ref;
import :binding;
import :bind_alias_proxy;
import :bound_insert_statement;
import :bound_delete_statement;
import :bound_update_statement;
import :bound_compact_statement;

import select_statement;
import delete_statement;
import update_statement;
import compact_statement;
import insert_statement;
import parsed_expr;
import knn_expr;
import table_reference;
import base_table_reference;
import subquery_reference;
import join_reference;
import cross_product_reference;

namespace infinity {

export class QueryBinder {
public:
    explicit QueryBinder(QueryContext *query_context, std::shared_ptr<BindContext> bind_context_ptr)
        : query_context_ptr_(std::move(query_context)), bind_context_ptr_(std::move(bind_context_ptr)) {}

    std::unique_ptr<BoundSelectStatement> BindSelect(const SelectStatement &statement);

    std::unique_ptr<BoundInsertStatement> BindInsert(const InsertStatement &statement);

    std::unique_ptr<BoundDeleteStatement> BindDelete(const DeleteStatement &statement);

    std::unique_ptr<BoundUpdateStatement> BindUpdate(const UpdateStatement &statement);

    std::unique_ptr<BoundCompactStatement> BindCompact(const CompactStatement &statement);

    std::shared_ptr<BaseTableRef> GetTableRef(const std::string &db_name, const std::string &table_name, bool update = false);

    QueryContext *query_context_ptr_;

    std::shared_ptr<BindContext> bind_context_ptr_;

private:
    std::shared_ptr<TableRef> BuildFromClause(QueryContext *query_context, const BaseTableReference *from_table);

    std::shared_ptr<TableRef> BuildDummyTable(QueryContext *query_context);

    std::shared_ptr<TableRef> BuildTable(QueryContext *query_context, const TableReference *table_reference);

    std::shared_ptr<TableRef> BuildSubquery(QueryContext *query_context, const SubqueryReference *subquery_reference);

    std::shared_ptr<TableRef> BuildCTE(QueryContext *query_context, const std::string &name);

    std::shared_ptr<BaseTableRef> BuildBaseTable(QueryContext *query_context, const TableReference *table_reference, bool update = false);

    std::shared_ptr<TableRef> BuildView(QueryContext *query_context, const TableReference *from_table);

    std::shared_ptr<TableRef> BuildCrossProduct(QueryContext *query_context, const CrossProductReference *cross_product);

    std::shared_ptr<TableRef> BuildJoin(QueryContext *query_context, const JoinReference *join_reference);

    void UnfoldStarExpression(QueryContext *query_context,
                              const std::vector<ParsedExpr *> &input_select_list,
                              std::vector<ParsedExpr *> &output_select_list);

    void GenerateColumns(const std::shared_ptr<Binding> &binding, const std::string &table_name, std::vector<ParsedExpr *> &output_select_list);

    void BuildGroupBy(QueryContext *query_context,
                      const SelectStatement &select,
                      const std::shared_ptr<BindAliasProxy> &bind_alias_proxy,
                      std::unique_ptr<BoundSelectStatement> &select_statement);

    void BuildHaving(QueryContext *query_context,
                     const SelectStatement &select,
                     const std::shared_ptr<BindAliasProxy> &bind_alias_proxy,
                     std::unique_ptr<BoundSelectStatement> &select_statement);

    void PushOrderByToProject(QueryContext *query_context, const SelectStatement &statement);

    void BuildSelectList(QueryContext *query_context, std::unique_ptr<BoundSelectStatement> &select_statement);

    void BuildOrderBy(QueryContext *query_context, const SelectStatement &statement, std::unique_ptr<BoundSelectStatement> &bound_statement) const;

    void BuildLimit(QueryContext *query_context, const SelectStatement &statement, std::unique_ptr<BoundSelectStatement> &bound_statement) const;

    void PruneOutput(QueryContext *query_context, i64 select_column_count, std::unique_ptr<BoundSelectStatement> &bound_statement);

    static void CheckKnnAndOrderBy(KnnDistanceType distance_type, OrderType order_type);
};

} // namespace infinity
