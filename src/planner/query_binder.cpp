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

#include <string>

module query_binder;

import stl;
import internal_types;
import infinity_exception;

import bind_context;
import join_table_ref;
import base_table_ref;
import query_context;
import binding;
import bound_select_statement;
import bound_delete_statement;
import bound_update_statement;
import bound_compact_statement;
import table_ref;
import bind_alias_proxy;
import base_expression;
import conjunction_expression;
import column_expression;
import expression_transformer;
import third_party;
import logger;
import where_binder;
import join_binder;
import group_binder;
import having_binder;
import order_binder;
import project_binder;
import limit_binder;
import subquery_table_ref;
import cross_product_table_ref;

import table_entry_type;
import block_index;
import cast_expression;
import search_expression;
import status;
import default_values;
import base_statement;
import select_statement;
import delete_statement;
import update_statement;
import compact_statement;
import parsed_expr;
import column_expr;
import knn_expr;
import column_def;
import base_table_reference;
import table_reference;
import subquery_reference;
import join_reference;
import cross_product_reference;
import data_type;
import logical_type;
import base_entry;
import view_entry;
import table_entry;
import txn;

namespace infinity {

UniquePtr<BoundSelectStatement> QueryBinder::BindSelect(const SelectStatement &statement) {

    UniquePtr<BoundSelectStatement> bound_select_statement = BoundSelectStatement::Make(bind_context_ptr_);

    if (statement.select_list_ == nullptr) {
        UnrecoverableError("SELECT list is needed");
    }
    if (statement.select_list_->empty()) {
        UnrecoverableError("SELECT list can't be empty");
    }

    // 1. WITH clause
    if (statement.with_exprs_ != nullptr) {

        // Prepare to store the with statement
        SizeT with_stmt_count = statement.with_exprs_->size();
        bind_context_ptr_->CTE_map_.reserve(with_stmt_count);

        // Hash set to restrict the with statement name visibility
        HashSet<String> masked_name_set;

        for (i64 i = with_stmt_count - 1; i >= 0; --i) {
            WithExpr *with_expr = (*statement.with_exprs_)[i];
            String name = with_expr->alias_;
            if (bind_context_ptr_->CTE_map_.contains(name)) {
                RecoverableError(Status::SyntaxError("WITH query table_name: " + name + " occurs more than once."));
            }

            if (with_expr->select_->type_ != StatementType::kSelect) {
                RecoverableError(Status::SyntaxError("Non-select statement in WITH clause."));
            }

            masked_name_set.insert(name);
            SharedPtr<CommonTableExpressionInfo> cte_info_ptr =
                MakeShared<CommonTableExpressionInfo>(name, (SelectStatement *)(with_expr->select_), masked_name_set);

            bind_context_ptr_->CTE_map_[name] = cte_info_ptr;
        }
    }

    // 2. FROM clause (BaseTable, Join and Subquery)
    // 3. ON
    // 4. JOIN
    if (statement.table_ref_ != nullptr) {
        // Build table reference
        bound_select_statement->table_ref_ptr_ = BuildFromClause(query_context_ptr_, statement.table_ref_);
    } else {
        // No table reference, just evaluate the expr of the select list.
        bound_select_statement->table_ref_ptr_ = BuildDummyTable(query_context_ptr_);
    }

    // 5. SELECT list (aliases)
    // Unfold the star expression in the select list.
    // Star expression will be unfolded and bound as column expressions.
    UnfoldStarExpression(query_context_ptr_, *statement.select_list_, bind_context_ptr_->select_expression_);

    SizeT select_column_count = bind_context_ptr_->select_expression_.size();
    for (SizeT column_index = 0; column_index < select_column_count; ++column_index) {
        const ParsedExpr *select_expr = bind_context_ptr_->select_expression_[column_index];

        // Check if select expression has alias.
        if (!select_expr->alias_.empty()) {
            if (bind_context_ptr_->select_alias2index_.contains(select_expr->alias_)) {
                i64 bound_column_index = bind_context_ptr_->select_alias2index_[select_expr->alias_];
                RecoverableError(Status::SyntaxError(bind_context_ptr_->select_expression_[bound_column_index]->ToString() + " and " +
                                                     select_expr->ToString() + " have same alias: " + select_expr->alias_));
            } else {
                // Store the alias to column index mapping, the mapping will be used in
                // - where clause binding
                // - group by clause binding
                // - having clause binding
                bind_context_ptr_->select_alias2index_[select_expr->alias_] = column_index;
            }
        } else {
            // KNN expression without alias, isn't allowed
            if (select_expr->type_ == ParsedExprType::kKnn) {
                RecoverableError(Status::SyntaxError("KNN expression in select list must have an alias."));
            }

            String select_expr_name = select_expr->ToString();
            if ((not bind_context_ptr_->select_expr_name2index_.contains(select_expr_name)) and
                bind_context_ptr_->binding_names_by_column_.contains(select_expr_name)) {
                select_expr_name = fmt::format("{}.{}", bind_context_ptr_->binding_names_by_column_[select_expr_name][0], select_expr_name);
            }
            if (bind_context_ptr_->select_expr_name2index_.contains(select_expr_name)) {
                LOG_TRACE(fmt::format("Same expression: {} had already been found in select list index: {}",
                                      select_expr_name,
                                      bind_context_ptr_->select_expr_name2index_[select_expr_name]));
                // TODO: create an map from secondary expression to the primary one.
            } else {
                bind_context_ptr_->select_expr_name2index_[select_expr_name] = column_index;
            }
        }
    }

    SharedPtr<BindAliasProxy> bind_alias_proxy = MakeShared<BindAliasProxy>();

    // 6.1 SEARCH
    if (statement.search_expr_ != nullptr) {
        bind_context_ptr_->BoundSearch(statement.search_expr_);

        auto where_binder = MakeShared<WhereBinder>(query_context_ptr_, bind_alias_proxy);
        SharedPtr<BaseExpression> search_expr = where_binder->Bind(*statement.search_expr_, this->bind_context_ptr_.get(), 0, true);
        bound_select_statement->search_expr_ = static_pointer_cast<SearchExpression>(search_expr);
    }

    // 6.2 WHERE
    if (statement.where_expr_) {
        auto where_binder = MakeShared<WhereBinder>(query_context_ptr_, bind_alias_proxy);
        SharedPtr<BaseExpression> where_expr = where_binder->Bind(*statement.where_expr_, this->bind_context_ptr_.get(), 0, true);
        if (where_expr->Type().type() != LogicalType::kBoolean) {
            RecoverableError(Status::InvalidFilterExpression(where_expr->Type().ToString()));
        }
        bound_select_statement->where_conditions_ = SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);
    }

    // 7. GROUP BY
    BuildGroupBy(query_context_ptr_, statement, bind_alias_proxy, bound_select_statement);
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    BuildHaving(query_context_ptr_, statement, bind_alias_proxy, bound_select_statement);

    // 10. DISTINCT
    bound_select_statement->distinct_ = statement.select_distinct_;

    // TODO: Add projection before sort, limit?
    //    // Push order by expression to projection
    //    if (statement.order_by_list != nullptr) {
    //        PushOrderByToProject(query_context_ptr_, statement);
    //    }

    // 11. SELECT (not flatten subquery)
    BuildSelectList(query_context_ptr_, bound_select_statement);
    bound_select_statement->aggregate_expressions_ = bind_context_ptr_->aggregate_exprs_;

    // 12. ORDER BY
    if (statement.order_by_list != nullptr) {
        BuildOrderBy(query_context_ptr_, statement, bound_select_statement);
    }

    // 13. LIMIT
    if (statement.limit_expr_ != nullptr) {
        BuildLimit(query_context_ptr_, statement, bound_select_statement);
    }

    // Trying to check if order by import new invisible column in project
    if (select_column_count < bound_select_statement->projection_expressions_.size()) {
        UnrecoverableError("Projection expressions more than expected!");
        //        bind_context_ptr_->result_index_ = bind_context_ptr_->GenerateTableIndex();
        //        PruneOutput(query_context_ptr_, select_column_count, bound_select_statement);
    } else {
        // Last table index is the project table index
        bind_context_ptr_->result_index_ = bind_context_ptr_->project_table_index_;
    }

    // 14. TOP
    // 15. UNION/INTERSECT/EXCEPT
    // 16. LIMIT
    // 17. ORDER BY
    // 18. TOP

    bound_select_statement->projection_index_ = bind_context_ptr_->project_table_index_;
    bound_select_statement->groupby_index_ = bind_context_ptr_->group_by_table_index_;
    bound_select_statement->aggregate_index_ = bind_context_ptr_->aggregate_table_index_;
    bound_select_statement->result_index_ = bind_context_ptr_->result_index_;
    bound_select_statement->knn_index_ = bind_context_ptr_->knn_table_index_;

    return bound_select_statement;
}

SharedPtr<TableRef> QueryBinder::BuildFromClause(QueryContext *query_context, const BaseTableReference *table_ref) {

    SharedPtr<TableRef> result = nullptr;
    switch (table_ref->type_) {
        case TableRefType::kTable: {
            // Only one table: select * from t1;
            result = BuildTable(query_context, (TableReference *)table_ref);
            break;
        }
        case TableRefType::kSubquery: {
            // select t1.a from (select * from t2 as t1);
            auto *subquery_ref = (SubqueryReference *)table_ref;
            result = BuildSubquery(query_context, subquery_ref);
            break;
        }
        case TableRefType::kJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            result = BuildJoin(query_context, (JoinReference *)table_ref);
            break;
        }
        case TableRefType::kCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            result = BuildCrossProduct(query_context, (CrossProductReference *)table_ref);
            break;
        }

        case TableRefType::kDummy: {
            UnrecoverableError("Unexpected table reference type.");
        }

            // TODO: No case currently, since parser doesn't support it.
            //        case hsql::kExpressionList: {
            //            break;
            //        }
    }

    return result;
}

SharedPtr<TableRef> QueryBinder::BuildDummyTable(QueryContext *) { return nullptr; }

SharedPtr<TableRef> QueryBinder::BuildTable(QueryContext *query_context, const TableReference *from_table) {
    // There are five cases here:
    // CTE*, which is subquery (may include correlated expression).
    // Recursive CTE (not supported by parser.)
    // View*, which is also a subquery (may include correlated expression).
    // Table*
    // External Table (not supported by parser.)

    // In AST, name is the alias of CTE and name of table
    // In AST, alias is the alias of table

    // If schema is null, it may from CTE
    // else the table will be checked in catalog

    String schema_name{};
    if (from_table->db_name_.empty()) {
        // Before find the table meta from catalog, Attempt to get CTE info from bind context which saved CTE info into before.
        if (SharedPtr<TableRef> cte_ref = BuildCTE(query_context, from_table->table_name_); cte_ref.get() != nullptr) {
            return cte_ref;
        }
    }

    // Base Table
    if (SharedPtr<TableRef> base_table_ref = BuildBaseTable(query_context, from_table); base_table_ref.get() != nullptr) {
        return base_table_ref;
    }

    // View
    if (SharedPtr<TableRef> view_ref = BuildView(query_context, from_table); view_ref.get() != nullptr) {
        return view_ref;
    }

    RecoverableError(Status::SyntaxError("Table or View: " + from_table->table_name_ + " is not found in catalog."));
    return nullptr;
}

SharedPtr<TableRef> QueryBinder::BuildSubquery(QueryContext *, const SubqueryReference *subquery_ref) {
    // Create new bind context and add into context array;
    SharedPtr<BindContext> subquery_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, subquery_bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(*subquery_ref->select_statement_);

    // Get the subquery result table index as the new from table index
    u64 subquery_table_index = bound_statement_ptr->result_index_;

    String binding_name;
    if (subquery_ref->alias_ == nullptr) {
        binding_name = "subquery" + std::to_string(subquery_table_index);
    } else {
        binding_name = subquery_ref->alias_->alias_;
        if (subquery_ref->alias_->column_alias_array_ != nullptr) {
            // Column alias
            SizeT column_count = subquery_ref->alias_->column_alias_array_->size();
            for (SizeT idx = 0; idx < column_count; ++idx) {
                bound_statement_ptr->names_ptr_->at(idx) = subquery_ref->alias_->column_alias_array_->at(idx);
            }
        }
    }

    // Add binding into bind context
    this->bind_context_ptr_->AddSubqueryBinding(binding_name, subquery_table_index, bound_statement_ptr->types_ptr_, bound_statement_ptr->names_ptr_);

    // Use binding name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(std::move(bound_statement_ptr), subquery_table_index, binding_name);

    // TODO: Not care about the correlated expression

    return subquery_table_ref_ptr;
}

SharedPtr<TableRef> QueryBinder::BuildCTE(QueryContext *, const String &name) {
    SharedPtr<CommonTableExpressionInfo> cte = this->bind_context_ptr_->GetCTE(name);
    if (cte.get() == nullptr) {
        return nullptr;
    }

    // Table is from CTE
    if (this->bind_context_ptr_->IsCTEBound(cte)) {
        // The CTE is bound before.
        RecoverableError(Status::SyntaxError("CTE can only be bound only once"));
    }

    // Build CTE(subquery)
    // insert into CTE Bound in bind context
    this->bind_context_ptr_->BoundCTE(cte);

    // Create new bind context and add into context array;
    SharedPtr<BindContext> subquery_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, subquery_bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(*cte->select_statement_);

    u64 cte_table_index = bound_statement_ptr->result_index_;
    // Add binding into bind context
    this->bind_context_ptr_->AddCTEBinding(name, cte_table_index, bound_statement_ptr->types_ptr_, bound_statement_ptr->names_ptr_);

    // Use CTE name as the subquery table reference name
    auto cte_table_ref_ptr = MakeShared<SubqueryTableRef>(std::move(bound_statement_ptr), cte_table_index, name);

    // TODO: Not care about the correlated expression

    return cte_table_ref_ptr;
}

SharedPtr<BaseTableRef> QueryBinder::BuildBaseTable(QueryContext *query_context, const TableReference *from_table) {
    String schema_name;
    if (from_table->db_name_.empty()) {
        schema_name = query_context->schema_name();
    } else {
        schema_name = from_table->db_name_;
    }

    auto [table_entry, status] = query_context->GetTxn()->GetTableByName(schema_name, from_table->table_name_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    if (table_entry->EntryType() == TableEntryType::kCollectionEntry) {
        RecoverableError(Status::SyntaxError("Currently, collection isn't supported."));
    }

    String alias = from_table->GetTableName();
    SharedPtr<Vector<SharedPtr<DataType>>> types_ptr = MakeShared<Vector<SharedPtr<DataType>>>();
    SharedPtr<Vector<String>> names_ptr = MakeShared<Vector<String>>();
    Vector<SizeT> columns;

    SizeT column_count = table_entry->ColumnCount();
    types_ptr->reserve(column_count);
    names_ptr->reserve(column_count);
    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {
        const ColumnDef *column_def = table_entry->GetColumnDefByID(idx);
        types_ptr->emplace_back(column_def->column_type_);
        names_ptr->emplace_back(column_def->name_);
        columns.emplace_back(idx);
    }

    Txn *txn = query_context->GetTxn();

    SharedPtr<BlockIndex> block_index = table_entry->GetBlockIndex(txn);

    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    auto table_ref = MakeShared<BaseTableRef>(table_entry, std::move(columns), block_index, alias, table_index, names_ptr, types_ptr);

    // Insert the table in the binding context
    this->bind_context_ptr_->AddTableBinding(alias, table_index, table_entry, std::move(types_ptr), std::move(names_ptr), std::move(block_index));

    return table_ref;
}

SharedPtr<TableRef> QueryBinder::BuildView(QueryContext *query_context, const TableReference *from_table) {
    BaseEntry *base_view_entry{nullptr};
    Status status = query_context->GetTxn()->GetViewByName(from_table->db_name_, from_table->table_name_, base_view_entry);
    if (!status.ok()) {
        RecoverableError(status);
    }

    ViewEntry *view_entry = static_cast<ViewEntry *>(base_view_entry);

    // Build view scan operator
    if (this->bind_context_ptr_->IsViewBound(from_table->table_name_)) {
        RecoverableError(Status::SyntaxError("View: " + from_table->table_name_ + " is bound before!"));
    }
    this->bind_context_ptr_->BoundView(from_table->table_name_);

    const SelectStatement *select_stmt_ptr = view_entry->GetSQLStatement();

    // Create new bind context and add into context array;
    SharedPtr<BindContext> view_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, view_bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(*select_stmt_ptr);

    // View table index is the output index of view.
    u64 view_index = bound_statement_ptr->result_index_;

    // Add binding into bind context
    this->bind_context_ptr_->AddViewBinding(from_table->table_name_, view_index, view_entry->column_types(), view_entry->column_names());

    // Use view name as the subquery table reference name
    auto subquery_table_ref_ptr =
        MakeShared<SubqueryTableRef>(std::move(bound_statement_ptr), bind_context_ptr_->GenerateTableIndex(), from_table->table_name_);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

SharedPtr<TableRef> QueryBinder::BuildCrossProduct(QueryContext *query_context, const CrossProductReference *from_table) {
    const Vector<BaseTableReference *> &tables = from_table->tables_;

    Vector<SharedPtr<BindContext>> bind_contexts;
    {
        SharedPtr<BindContext> current_bind_context = this->bind_context_ptr_;
        SizeT table_count = tables.size();
        bind_contexts.reserve(table_count * 2);
        for (SizeT i = 0; i < table_count - 1; ++i) {
            SharedPtr<BindContext> right_bind_context = BindContext::Make(current_bind_context);
            SharedPtr<BindContext> left_bind_context = BindContext::Make(current_bind_context);
            bind_contexts.emplace_back(right_bind_context);
            bind_contexts.emplace_back(left_bind_context);

            current_bind_context = left_bind_context;
        }
    }

    SizeT bind_context_idx = bind_contexts.size() - 1;

    SharedPtr<CrossProductTableRef> cross_product_table_ref{};

    SharedPtr<BindContext> left_bind_context = bind_contexts[bind_context_idx--];
    UniquePtr<QueryBinder> left_query_binder = MakeUnique<QueryBinder>(query_context, left_bind_context);
    SharedPtr<TableRef> left_table_ref = left_query_binder->BuildFromClause(query_context, tables[0]);

    SharedPtr<BindContext> right_bind_context{};
    UniquePtr<QueryBinder> right_query_binder{};
    SharedPtr<TableRef> right_table_ref{};

    SizeT table_count = tables.size();
    for (SizeT i = 1; i < table_count - 1; ++i) {
        right_bind_context = bind_contexts[bind_context_idx--];
        right_query_binder = MakeUnique<QueryBinder>(query_context, right_bind_context);
        right_table_ref = right_query_binder->BuildFromClause(query_context, tables[i]);

        SharedPtr<BindContext> cross_product_bind_context = bind_contexts[bind_context_idx--];
        cross_product_bind_context->AddLeftChild(left_bind_context);
        cross_product_bind_context->AddRightChild(right_bind_context);

        UniquePtr<QueryBinder> cross_product_query_binder = MakeUnique<QueryBinder>(query_context, cross_product_bind_context);

        cross_product_table_ref = MakeShared<CrossProductTableRef>("cross product");
        cross_product_table_ref->left_table_ref_ = left_table_ref;
        cross_product_table_ref->right_table_ref_ = right_table_ref;

        left_bind_context = cross_product_bind_context;
        left_query_binder = std::move(cross_product_query_binder);
        left_table_ref = cross_product_table_ref;
    }

    right_bind_context = bind_contexts[bind_context_idx];
    if (bind_context_idx != 0) {
        RecoverableError(Status::SyntaxError("Mismatched bind context count."));
    }
    right_query_binder = MakeUnique<QueryBinder>(query_context, right_bind_context);
    right_table_ref = right_query_binder->BuildFromClause(query_context, tables[table_count - 1]);

    this->bind_context_ptr_->AddLeftChild(left_bind_context);
    this->bind_context_ptr_->AddRightChild(right_bind_context);

    cross_product_table_ref = MakeShared<CrossProductTableRef>("cross product");
    cross_product_table_ref->left_table_ref_ = left_table_ref;
    cross_product_table_ref->right_table_ref_ = right_table_ref;

    SharedPtr<TableRef> result = cross_product_table_ref;
    return result;
}

SharedPtr<TableRef> QueryBinder::BuildJoin(QueryContext *query_context, const JoinReference *from_table) {
    String alias{};

    if (from_table->alias_ != nullptr && from_table->alias_->alias_ != nullptr) {
        alias = from_table->alias_->alias_;
    }

    auto result = MakeShared<JoinTableRef>(alias);

    result->join_type_ = from_table->join_type_;

    // Build left child
    auto left_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);
    UniquePtr<QueryBinder> left_query_binder = MakeUnique<QueryBinder>(query_context, left_bind_context_ptr);
    auto left_bound_table_ref = left_query_binder->BuildFromClause(query_context, from_table->left_);
    this->bind_context_ptr_->AddLeftChild(left_query_binder->bind_context_ptr_);

    // Build right child
    auto right_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);
    UniquePtr<QueryBinder> right_query_binder = MakeUnique<QueryBinder>(query_context, right_bind_context_ptr);
    auto right_bound_table_ref = right_query_binder->BuildFromClause(query_context, from_table->right_);
    this->bind_context_ptr_->AddRightChild(right_query_binder->bind_context_ptr_);

    result->left_table_ref_ = left_bound_table_ref;
    result->left_bind_context_ = left_query_binder->bind_context_ptr_;

    result->right_table_ref_ = right_bound_table_ref;
    result->right_bind_context_ = right_query_binder->bind_context_ptr_;

    // Current parser doesn't support On using column syntax, so only consider the case of natural join.
    if (result->join_type_ == JoinType::kNatural) {
        HashSet<String> left_binding_column_names;
        // TODO: Is there any way to get all_column_names size ? Collect all left binding columns numbers at very beginning?
        for (auto &left_binding_pair : result->left_bind_context_->binding_by_name_) {
            for (auto &left_column_name : *left_binding_pair.second->column_names_) {
                left_binding_column_names.emplace(left_column_name);
            }
        }

        Vector<String> using_column_names;
        // TODO: column count of left binding tables and right binding tables is using_column_names size
        for (auto &right_binding_pair : result->right_bind_context_->binding_by_name_) {
            for (auto &right_column_name : *right_binding_pair.second->column_names_) {
                if (left_binding_column_names.contains(right_column_name)) {
                    using_column_names.emplace_back(right_column_name);
                }
            }
        }

        if (using_column_names.empty()) {
            // It is cross product, but not a natural join with dummy name
            String cross_product_table_name = "cross_product";
            auto cross_product_table_ref = MakeShared<CrossProductTableRef>(cross_product_table_name);
            //            cross_product_table_ref->left_bind_context_ = result->left_bind_context_;
            cross_product_table_ref->left_table_ref_ = left_bound_table_ref;

            //            cross_product_table_ref->right_bind_context_ = result->right_bind_context_;
            cross_product_table_ref->right_table_ref_ = right_bound_table_ref;
            return cross_product_table_ref;
        } else {
            // It is a natural join, we only consider the inner natural join case.
            result->join_type_ = JoinType::kInner;

            // Reserve the on condition space for the expression.
            result->on_conditions_.reserve(using_column_names.size());

            // TODO: Construct join condition: left_column_expr = right_column_expr AND left_column_expr = right_column_expr;
            for (auto &column_name : using_column_names) {
                // Create left bound column expression
                if (!result->left_bind_context_->binding_names_by_column_.contains(column_name)) {
                    RecoverableError(Status::SyntaxError("Column: " + column_name + " doesn't exist in left table"));
                }

                auto &left_column_binding_names = result->left_bind_context_->binding_names_by_column_[column_name];

                if (left_column_binding_names.size() != 1) {
                    RecoverableError(Status::SyntaxError("Ambiguous column table_name: " + column_name + " in left table"));
                }

                auto &left_binding_name = left_column_binding_names[0];
                auto &left_binding_ptr = result->left_bind_context_->binding_by_name_[left_binding_name];
                auto left_column_index = left_binding_ptr->name2index_[column_name];
                auto left_column_type = left_binding_ptr->column_types_->at(left_column_index);

                SharedPtr<ColumnExpression> left_column_expression_ptr = MakeShared<ColumnExpression>(*left_column_type,
                                                                                                      left_binding_ptr->table_name_,
                                                                                                      left_binding_ptr->table_index_,
                                                                                                      column_name,
                                                                                                      left_column_index,
                                                                                                      0);

                if (!result->right_bind_context_->binding_names_by_column_.contains(column_name)) {
                    RecoverableError(Status::SyntaxError("Column: " + column_name + " doesn't exist in right table"));
                }

                auto &right_column_binding_names = result->right_bind_context_->binding_names_by_column_[column_name];

                if (right_column_binding_names.size() != 1) {
                    RecoverableError(Status::SyntaxError("Ambiguous column table_name: " + column_name + " in right table"));
                }

                auto &right_binding_name = right_column_binding_names[0];
                auto &right_binding_ptr = result->right_bind_context_->binding_by_name_[right_binding_name];
                if (right_binding_ptr.get() == nullptr) {
                    RecoverableError(Status::SyntaxError("Column: " + column_name + " doesn't exist in right table"));
                }
                auto right_column_index = right_binding_ptr->name2index_[column_name];
                auto right_column_type = right_binding_ptr->column_types_->at(right_column_index);

                SharedPtr<ColumnExpression> right_column_expression_ptr = MakeShared<ColumnExpression>(*right_column_type,
                                                                                                       right_binding_ptr->table_name_,
                                                                                                       right_binding_ptr->table_index_,
                                                                                                       column_name,
                                                                                                       right_column_index,
                                                                                                       0);

                auto condition = MakeShared<ConjunctionExpression>(ConjunctionType::kAnd, left_column_expression_ptr, right_column_expression_ptr);
                result->on_conditions_.emplace_back(condition);

                // For natural join, we can return now.
                return result;
            }
        }
    }

    // SharedPtr<JoinBinder> join_binder
    auto join_binder = MakeShared<JoinBinder>(query_context);

    // SharedPtr<BaseExpression> on_condition_ptr
    auto on_condition_ptr = join_binder->BuildExpression(*from_table->condition_, this->bind_context_ptr_.get(), 0, true);

    result->on_conditions_ = SplitExpressionByDelimiter(on_condition_ptr, ConjunctionType::kAnd);

    return result;
}

void QueryBinder::UnfoldStarExpression(QueryContext *, const Vector<ParsedExpr *> &input_select_list, Vector<ParsedExpr *> &output_select_list) {
    output_select_list.reserve(input_select_list.size());
    for (auto *select_expr : input_select_list) {
        if (select_expr->type_ == ParsedExprType::kColumn) {
            auto *column_expr = (ColumnExpr *)select_expr;
            if (column_expr->star_) {
                if (column_expr->names_.empty()) {
                    // select * from t1;
                    if (this->bind_context_ptr_->table_names_.empty()) {
                        RecoverableError(Status::SyntaxError("No table was bound."));
                    }

                    // select * from t1, t2; means select t1.*, t2.* from t1, t2;
                    for (const auto &table_name : this->bind_context_ptr_->table_names_) {
                        SharedPtr<Binding> binding = this->bind_context_ptr_->binding_by_name_[table_name];
                        if (binding.get() == nullptr) {
                            RecoverableError(Status::SyntaxError("Table: " + table_name + " wasn't bound before."));
                        }
                        GenerateColumns(binding, table_name, output_select_list);
                    }
                } else {
                    String table_name = column_expr->names_[0];
                    SharedPtr<Binding> binding = this->bind_context_ptr_->binding_by_name_[table_name];
                    if (binding.get() == nullptr) {
                        RecoverableError(Status::SyntaxError("Table: " + table_name + " wasn't bound before."));
                    }
                    GenerateColumns(binding, table_name, output_select_list);
                }

                continue;
            }
        }
        output_select_list.emplace_back(select_expr);
    }
}

void QueryBinder::GenerateColumns(const SharedPtr<Binding> &binding, const String &table_name, Vector<ParsedExpr *> &output_select_list) {
    switch (binding->binding_type_) {

        case BindingType::kInvalid: {
            UnrecoverableError("Invalid binding type.");
            break;
        }
        case BindingType::kTable: {
            SizeT column_count = binding->table_collection_entry_ptr_->ColumnCount();

            // Reserve more data in select list
            output_select_list.reserve(output_select_list.size() + column_count);

            // Build select list
            for (SizeT idx = 0; idx < column_count; ++idx) {
                String column_name = binding->table_collection_entry_ptr_->GetColumnDefByID(idx)->name_;
                auto *column_expr = new ColumnExpr();
                column_expr->names_.emplace_back(table_name);
                column_expr->names_.emplace_back(column_name);
                column_expr->generated_ = true;
                output_select_list.emplace_back(column_expr);
            }
            break;
        }
        case BindingType::kSubquery:
        case BindingType::kCTE: {
            SizeT column_count = binding->column_names_->size();

            // Reserve more data in select list
            output_select_list.reserve(output_select_list.size() + column_count);

            // Build select list
            for (SizeT idx = 0; idx < column_count; ++idx) {
                String column_name = binding->column_names_->at(idx);
                auto *column_expr = new ColumnExpr();
                column_expr->names_.emplace_back(table_name);
                column_expr->names_.emplace_back(column_name);
                output_select_list.emplace_back(column_expr);
                column_expr->generated_ = true;
            }
            break;
        }
        case BindingType::kView: {
            RecoverableError(Status::SyntaxError("Not implemented"));
            break;
        }
    }
}

void QueryBinder::BuildGroupBy(QueryContext *query_context,
                               const SelectStatement &select,
                               const SharedPtr<BindAliasProxy> &bind_alias_proxy,
                               UniquePtr<BoundSelectStatement> &select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->group_by_table_index_ = table_index;
    bind_context_ptr_->group_by_table_name_ = "groupby" + std::to_string(table_index);

    if (select.group_by_list_ != nullptr) {
        // Start to bind GROUP BY clause
        // Set group binder
        auto group_binder = MakeShared<GroupBinder>(query_context, bind_alias_proxy);

        // Reserve the group names used in GroupBinder::BuildExpression
        SizeT group_count = select.group_by_list_->size();
        bind_context_ptr_->group_exprs_.reserve(group_count);
        for (SizeT idx = 0; idx < group_count; ++idx) {
            // set group-by expression index
            group_binder->group_by_expr_index = idx;
            const ParsedExpr &expr = *(*select.group_by_list_)[idx];

            // Call GroupBinder BuildExpression
            SharedPtr<BaseExpression> group_by_expr = group_binder->Bind(expr, this->bind_context_ptr_.get(), 0, true);
        }

        select_statement->group_by_expressions_ = bind_context_ptr_->group_exprs_;
    }
}

void QueryBinder::BuildHaving(QueryContext *query_context,
                              const SelectStatement &select,
                              const SharedPtr<BindAliasProxy> &bind_alias_proxy,
                              UniquePtr<BoundSelectStatement> &select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->aggregate_table_index_ = table_index;
    bind_context_ptr_->aggregate_table_name_ = "aggregate" + std::to_string(table_index);

    // All having expr must appear in group by list or aggregate function list.
    if (select.group_by_list_ != nullptr && select.having_expr_ != nullptr) {
        // Start to bind Having clause
        // Set having binder
        auto having_binder = MakeShared<HavingBinder>(query_context, bind_alias_proxy);
        SharedPtr<BaseExpression> having_expr = having_binder->Bind(*(select.having_expr_), bind_context_ptr_.get(), 0, true);
        select_statement->having_expressions_ = SplitExpressionByDelimiter(having_expr, ConjunctionType::kAnd);
    }
}

void QueryBinder::PushOrderByToProject(QueryContext *, const SelectStatement &statement) {
    for (const OrderByExpr *order_by_expr : *statement.order_by_list) {
        if (order_by_expr->expr_->type_ == ParsedExprType::kKnn) {
            continue;
        } else {
            OrderBinder::PushExtraExprToSelectList(order_by_expr->expr_, bind_context_ptr_);
        }
    }
}

void QueryBinder::BuildSelectList(QueryContext *, UniquePtr<BoundSelectStatement> &bound_select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->project_table_index_ = table_index;
    bind_context_ptr_->project_table_name_ = "project" + std::to_string(table_index);

    auto project_binder = MakeShared<ProjectBinder>(query_context_ptr_);

    SizeT column_count = bind_context_ptr_->select_expression_.size();
    bound_select_statement->names_ptr_ = MakeShared<Vector<String>>();
    bound_select_statement->names_ptr_->reserve(column_count);

    bound_select_statement->types_ptr_ = MakeShared<Vector<SharedPtr<DataType>>>();
    bound_select_statement->types_ptr_->reserve(column_count);
    bind_context_ptr_->project_exprs_.reserve(column_count);
    bound_select_statement->projection_expressions_.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        const ParsedExpr *select_expr = bind_context_ptr_->select_expression_[column_id];
        SharedPtr<BaseExpression> bound_expr = project_binder->Bind(*select_expr, this->bind_context_ptr_.get(), 0, true);
        String expr_name = bound_expr->Name();
        if (!(bind_context_ptr_->project_index_by_name_.contains(expr_name))) {
            bind_context_ptr_->project_index_by_name_[expr_name] = column_id;
        }
        bind_context_ptr_->project_exprs_.emplace_back(bound_expr);

        // Insert the bound expression into projection expressions of select node.
        bound_select_statement->projection_expressions_.emplace_back(bound_expr);

        bound_select_statement->names_ptr_->emplace_back(expr_name);
        bound_select_statement->types_ptr_->emplace_back(MakeShared<DataType>(bound_expr->Type()));
    }

    if (!bound_select_statement->having_expressions_.empty() || !bound_select_statement->group_by_expressions_.empty() ||
        !bind_context_ptr_->aggregate_exprs_.empty()) {
        if (!project_binder->BoundColumn().empty()) {
            RecoverableError(Status::SyntaxError("Column: " + project_binder->BoundColumn() +
                                                 " must appear in the GROUP BY clause or be used in an aggregate function"));
        }
    }
}

void QueryBinder::BuildOrderBy(QueryContext *query_context,
                               const SelectStatement &statement,
                               UniquePtr<BoundSelectStatement> &bound_statement) const {
    auto order_binder = MakeShared<OrderBinder>(query_context);
    SizeT order_by_count = statement.order_by_list->size();
    bound_statement->order_by_expressions_.reserve(order_by_count);
    bound_statement->order_by_types_.reserve(order_by_count);
    for (const OrderByExpr *order_expr : *statement.order_by_list) {
        auto bound_order_expr = order_binder->Bind(*order_expr->expr_, this->bind_context_ptr_.get(), 0, true);
        bound_statement->order_by_types_.emplace_back(order_expr->type_);
        bound_statement->order_by_expressions_.emplace_back(bound_order_expr);
    }
}

void QueryBinder::BuildLimit(QueryContext *query_context, const SelectStatement &statement, UniquePtr<BoundSelectStatement> &bound_statement) const {
    auto limit_binder = MakeShared<LimitBinder>(query_context);
    bound_statement->limit_expression_ = limit_binder->Bind(*statement.limit_expr_, this->bind_context_ptr_.get(), 0, true);

    if (statement.offset_expr_ != nullptr) {
        bound_statement->offset_expression_ = limit_binder->Bind(*statement.offset_expr_, this->bind_context_ptr_.get(), 0, true);
    }
}

void QueryBinder::PruneOutput(QueryContext *, i64 select_column_count, UniquePtr<BoundSelectStatement> &bound_statement) {
    Vector<SharedPtr<BaseExpression>> &pruned_expressions = bound_statement->pruned_expression_;
    Vector<SharedPtr<BaseExpression>> &projection_expressions = bound_statement->projection_expressions_;
    Vector<String> &output_names = *bound_statement->names_ptr_;

    pruned_expressions.reserve(select_column_count);

    for (i64 column_id = 0; column_id < select_column_count; ++column_id) {
        const SharedPtr<BaseExpression> &expr = projection_expressions[column_id];
        SharedPtr<ColumnExpression> result = ColumnExpression::Make(expr->Type(),
                                                                    bind_context_ptr_->project_table_name_,
                                                                    bind_context_ptr_->project_table_index_,
                                                                    output_names[column_id],
                                                                    column_id,
                                                                    0);
        result->source_position_ = SourcePosition(bind_context_ptr_->binding_context_id_, ExprSourceType::kProjection);
        pruned_expressions.emplace_back(result);
    }
}

void QueryBinder::CheckKnnAndOrderBy(KnnDistanceType distance_type, OrderType order_type) {
    switch (distance_type) {
        case KnnDistanceType::kL2:
        case KnnDistanceType::kHamming: {
            if (order_type != OrderType::kAsc) {
                RecoverableError(Status::SyntaxError("L2 and Hamming distance need ascending order"));
            }
            break;
        }
        case KnnDistanceType::kInnerProduct:
        case KnnDistanceType::kCosine: {
            if (order_type != OrderType::kDesc) {
                RecoverableError(Status::SyntaxError("Inner product and cosine distance need descending order"));
            }
            break;
        }
        default: {
            UnrecoverableError("Invalid KNN distance type");
        }
    }
}

SharedPtr<BaseTableRef> QueryBinder::GetTableRef(const String &db_name, const String &table_name) {
    TableReference from_table;
    from_table.db_name_ = db_name;
    from_table.table_name_ = table_name;
    return BuildBaseTable(this->query_context_ptr_, &from_table);
}

UniquePtr<BoundDeleteStatement> QueryBinder::BindDelete(const DeleteStatement &statement) {
    // refers to QueryBinder::BindSelect
    UniquePtr<BoundDeleteStatement> bound_delete_statement = BoundDeleteStatement::Make(bind_context_ptr_);
    SharedPtr<BaseTableRef> base_table_ref = GetTableRef(statement.schema_name_, statement.table_name_);

    bound_delete_statement->table_ref_ptr_ = base_table_ref;
    if (base_table_ref.get() == nullptr) {
        RecoverableError(Status::SyntaxError(fmt::format("Cannot bind {}.{} to a table", statement.schema_name_, statement.table_name_)));
    }

    SharedPtr<BindAliasProxy> bind_alias_proxy = MakeShared<BindAliasProxy>();
    auto where_binder = MakeShared<WhereBinder>(this->query_context_ptr_, bind_alias_proxy);
    if (statement.where_expr_ != nullptr) {
        SharedPtr<BaseExpression> where_expr = where_binder->Bind(*statement.where_expr_, this->bind_context_ptr_.get(), 0, true);
        if(where_expr->Type().type() != LogicalType::kBoolean) {
            RecoverableError(Status::InvalidFilterExpression(where_expr->Type().ToString()));
        }
        bound_delete_statement->where_conditions_ = SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);
    }
    return bound_delete_statement;
}

UniquePtr<BoundUpdateStatement> QueryBinder::BindUpdate(const UpdateStatement &statement) {
    // refers to QueryBinder::BindSelect
    UniquePtr<BoundUpdateStatement> bound_update_statement = BoundUpdateStatement::Make(bind_context_ptr_);
    SharedPtr<BaseTableRef> base_table_ref = GetTableRef(statement.schema_name_, statement.table_name_);

    bound_update_statement->table_ref_ptr_ = base_table_ref;
    if (base_table_ref.get() == nullptr) {
        RecoverableError(Status::SyntaxError(fmt::format("Cannot bind {}.{} to a table", statement.schema_name_, statement.table_name_)));
    }

    SharedPtr<BindAliasProxy> bind_alias_proxy = MakeShared<BindAliasProxy>();
    auto where_binder = MakeShared<WhereBinder>(this->query_context_ptr_, bind_alias_proxy);
    if (statement.where_expr_ != nullptr) {
        SharedPtr<BaseExpression> where_expr = where_binder->Bind(*statement.where_expr_, this->bind_context_ptr_.get(), 0, true);
        if(where_expr->Type().type() != LogicalType::kBoolean) {
            RecoverableError(Status::InvalidFilterExpression(where_expr->Type().ToString()));
        }
        bound_update_statement->where_conditions_ = SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);
    }
    if (statement.update_expr_array_ == nullptr) {
        RecoverableError(Status::SyntaxError(fmt::format("Update expr array is empty")));
    }

    const Vector<String> &column_names = *base_table_ref->column_names_;
    const Vector<SharedPtr<DataType>> &column_types = *base_table_ref->column_types_;
    //    const Vector<String> &column_names = *base_table_ref->column_names_;
    auto project_binder = MakeShared<ProjectBinder>(query_context_ptr_);
    for (UpdateExpr *upd_expr : *statement.update_expr_array_) {
        std::string &column_name = upd_expr->column_name;
        ParsedExpr *expr = upd_expr->value;
        auto it = std::find(column_names.begin(), column_names.end(), column_name);
        if (it == column_names.end()) {
            RecoverableError(Status::SyntaxError(
                fmt::format("Column {} doesn't exist in table {}.{}", column_name, statement.schema_name_, statement.table_name_)));
        }
        SizeT column_id = std::distance(column_names.begin(), it);
        SharedPtr<BaseExpression> update_expr = project_binder->Bind(*expr, this->bind_context_ptr_.get(), 0, true);
        update_expr = CastExpression::AddCastToType(update_expr, *column_types[column_id]);
        bound_update_statement->update_columns_.emplace_back(column_id, update_expr);
    }
    std::sort(bound_update_statement->update_columns_.begin(), bound_update_statement->update_columns_.end());
    return bound_update_statement;
}

UniquePtr<BoundCompactStatement> QueryBinder::BindCompact(const CompactStatement &statement) {
    Txn *txn = query_context_ptr_->GetTxn();
    SharedPtr<BaseTableRef> base_table_ref = nullptr;
    if (statement.compact_type_ == CompactStatementType::kManual) {
        const auto &compact_statement = static_cast<const ManualCompactStatement &>(statement);
        base_table_ref = GetTableRef(compact_statement.schema_name_, compact_statement.table_name_);
    } else {
        const auto &compact_statement = static_cast<const AutoCompactStatement &>(statement);
        auto block_index = MakeShared<BlockIndex>();
        for (auto *compactible_segment : compact_statement.compactible_segments_) {
            block_index->Insert(compactible_segment, txn);
        }
        base_table_ref = MakeShared<BaseTableRef>(compact_statement.table_entry_, std::move(block_index));
    }
    TableEntry *table_entry = base_table_ref->table_entry_ptr_;
    base_table_ref->index_index_ = table_entry->GetIndexIndex(txn);

    return MakeUnique<BoundCompactStatement>(bind_context_ptr_, base_table_ref, statement.compact_type_);
}

} // namespace infinity
