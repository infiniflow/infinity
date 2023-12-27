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

module;

import logical_node;
import stl;
import parser;
import query_context;
import bound_select_statement;
import bind_context;
import table_ref;
import binding;
import bind_alias_proxy;

export module query_binder;

namespace infinity {

class BoundDeleteStatement;
class BoundUpdateStatement;

export class QueryBinder {
public:
    explicit QueryBinder(QueryContext *query_context, SharedPtr<BindContext> bind_context_ptr)
        : query_context_ptr_(Move(query_context)), bind_context_ptr_(Move(bind_context_ptr)) {}

    UniquePtr<BoundSelectStatement> BindSelect(const SelectStatement &statement);

    UniquePtr<BoundDeleteStatement> BindDelete(const DeleteStatement &statement);

    UniquePtr<BoundUpdateStatement> BindUpdate(const UpdateStatement &statement);

    SharedPtr<TableRef> GetTableRef(const String &db_name, const String &table_name);

    QueryContext *query_context_ptr_;

    SharedPtr<BindContext> bind_context_ptr_;

private:
    SharedPtr<TableRef> BuildFromClause(QueryContext *query_context, const BaseTableReference *from_table);

    SharedPtr<TableRef> BuildDummyTable(QueryContext *query_context);

    SharedPtr<TableRef> BuildTable(QueryContext *query_context, const TableReference *table_reference);

    SharedPtr<TableRef> BuildSubquery(QueryContext *query_context, const SubqueryReference *subquery_reference);

    SharedPtr<TableRef> BuildCTE(QueryContext *query_context, const String &name);

    SharedPtr<TableRef> BuildBaseTable(QueryContext *query_context, const TableReference *table_reference);

    SharedPtr<TableRef> BuildView(QueryContext *query_context, const TableReference *from_table);

    SharedPtr<TableRef> BuildCrossProduct(QueryContext *query_context, const CrossProductReference *cross_product);

    SharedPtr<TableRef> BuildJoin(QueryContext *query_context, const JoinReference *join_reference);

    void UnfoldStarExpression(QueryContext *query_context, const Vector<ParsedExpr *> &input_select_list, Vector<ParsedExpr *> &output_select_list);

    void GenerateColumns(const SharedPtr<Binding> &binding, const String &table_name, Vector<ParsedExpr *> &output_select_list);

    void BuildGroupBy(QueryContext *query_context,
                      const SelectStatement &select,
                      const SharedPtr<BindAliasProxy> &bind_alias_proxy,
                      UniquePtr<BoundSelectStatement> &select_statement);

    void BuildHaving(QueryContext *query_context,
                     const SelectStatement &select,
                     const SharedPtr<BindAliasProxy> &bind_alias_proxy,
                     UniquePtr<BoundSelectStatement> &select_statement);

    void PushOrderByToProject(QueryContext *query_context, const SelectStatement &statement);

    void BuildSelectList(QueryContext *query_context, UniquePtr<BoundSelectStatement> &select_statement);

    void BuildOrderBy(QueryContext *query_context, const SelectStatement &statement, UniquePtr<BoundSelectStatement> &bound_statement) const;

    void BuildLimit(QueryContext *query_context, const SelectStatement &statement, UniquePtr<BoundSelectStatement> &bound_statement) const;

    void PruneOutput(QueryContext *query_context, i64 select_column_count, UniquePtr<BoundSelectStatement> &bound_statement);

    static void CheckKnnAndOrderBy(KnnDistanceType distance_type, OrderType order_type);
};

} // namespace infinity
