//
// Created by jinhai on 23-1-17.
//

#include "query_binder.h"
#include "main/infinity.h"
#include "planner/bound/base_table_ref.h"
#include "planner/bound/subquery_table_ref.h"
#include "planner/bound/cross_product_table_ref.h"
#include "planner/bound/join_table_ref.h"
#include "planner/bound/dummy_table_ref.h"
#include "common/singleton.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"
#include "planner/binder/join_binder.h"
#include "planner/binder/bind_alias_proxy.h"
#include "expression/expression_transformer.h"
#include "parser/expression/parsed_raw_expression.h"
#include "parser/expression/parsed_column_expression.h"
#include "planner/binder/where_binder.h"
#include "planner/binder/group_binder.h"
#include "planner/binder/having_binder.h"
#include "planner/binder/project_binder.h"
#include "planner/binder/order_binder.h"
#include "planner/binder/limit_binder.h"

namespace infinity {

SharedPtr<BoundSelectStatement>
QueryBinder::BindSelect(const hsql::SelectStatement& statement) {
    SharedPtr<BoundSelectStatement> bound_select_statement = BoundSelectStatement::Make();
    PlannerAssert(statement.selectList != nullptr, "SELECT list is needed");
    PlannerAssert(!statement.selectList->empty(), "SELECT list can't be empty");

    // 1. WITH clause
    if (statement.withDescriptions != nullptr) {

        // Prepare to store the with statement
        SizeT with_stmt_count = statement.withDescriptions->size();
        bind_context_ptr_->CTE_map_.reserve(with_stmt_count);

        // Hash set to restrict the with statement name visibility
        HashSet<String> masked_name_set;

        for(i64 i = with_stmt_count - 1; i >= 0; -- i) {
            hsql::WithDescription* with_desc = (*statement.withDescriptions)[i];
            String name = with_desc->alias;
            if(bind_context_ptr_->CTE_map_.contains(name)) {
                PlannerError("WITH query name: " + name + " occurs more than once.");
            }

            masked_name_set.insert(name);
            SharedPtr<CommonTableExpressionInfo> cte_info_ptr
                    = MakeShared<CommonTableExpressionInfo>(name, with_desc->select, masked_name_set);

            bind_context_ptr_->CTE_map_[name] = cte_info_ptr;
        }
    }

    // 2. FROM clause (BaseTable, Join and Subquery)
    // 3. ON
    // 4. JOIN
    if (statement.fromTable != nullptr) {
        // Build table reference
        bound_select_statement->table_ref_ptr_ = BuildFromClause(query_context_ptr_, statement.fromTable);
    } else {
        // No table reference, just evaluate the expr of the select list.
        bound_select_statement->table_ref_ptr_ = BuildDummyTable(query_context_ptr_);
    }

    // 5. SELECT list (aliases)
    // Unfold the star expression in the select list.
    // Star expression will be unfolded and bound as column expressions.
    bind_context_ptr_->select_expression_ = UnfoldStarExpression(query_context_ptr_, *statement.selectList);

    i64 select_column_count = bind_context_ptr_->select_expression_.size();
    bound_select_statement->output_names_.reserve(select_column_count);
    for(i64 column_index = 0; column_index < select_column_count; ++ column_index) {
        const SharedPtr<ParsedExpression>& select_expr = bind_context_ptr_->select_expression_[column_index];
        // Bound column expression is bound due to the star expression, which won't be referenced in other place.
        // Only consider the Raw Expression case.
        if(!select_expr->alias_.empty()) {
            if(bind_context_ptr_->select_alias2index_.contains(select_expr->alias_)) {
                i64 bound_column_index = bind_context_ptr_->select_alias2index_[select_expr->alias_];
                PlannerError(bind_context_ptr_->select_expression_[bound_column_index]->ToString() + " and "
                             + select_expr->ToString() + " have same alias: " + select_expr->alias_);
            } else {
                bind_context_ptr_->select_alias2index_[select_expr->alias_] = column_index;
            }
        } else {
            const String select_expr_name = select_expr->GetName();
            if(bind_context_ptr_->select_expr_name2index_.contains(select_expr_name)) {
                LOG_TRACE("Same expression: {} had already been found in select list index: {}",
                          select_expr_name,
                          bind_context_ptr_->select_expr_name2index_[select_expr_name]);
                // TODO: create an map from secondary expression to the primary one.
            } else {
                bind_context_ptr_->select_expr_name2index_[select_expr_name] = column_index;
            }
        }

        bound_select_statement->output_names_.emplace_back(select_expr->ToString());
    }

    SharedPtr<BindAliasProxy> bind_alias_proxy = MakeShared<BindAliasProxy>();

    // 6. WHERE
    if (statement.whereClause) {
        auto where_binder = MakeShared<WhereBinder>(query_context_ptr_, bind_alias_proxy);
        SharedPtr<BaseExpression> where_expr =
                where_binder->Bind(*statement.whereClause, this->bind_context_ptr_, 0, true);

        bound_select_statement->where_conditions_ =
                SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);
    }

    // 7. GROUP BY
    BuildGroupBy(query_context_ptr_, statement, bind_alias_proxy, bound_select_statement);
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    BuildHaving(query_context_ptr_, statement, bind_alias_proxy, bound_select_statement);

    // 10. DISTINCT
    bound_select_statement->distinct_ = statement.selectDistinct;

    // Push order by expression to projection
    if(statement.order != nullptr) {
        PushOrderByToProject(query_context_ptr_, statement);
    }

    // 11. SELECT (not flatten subquery)
    BuildSelectList(query_context_ptr_, bound_select_statement);

    // 12. ORDER BY
    if(statement.order != nullptr) {
        BuildOrderBy(query_context_ptr_, statement, bound_select_statement);
    }

    // 13. LIMIT
    if(statement.limit !=nullptr) {
        BuildLimit(query_context_ptr_, statement, bound_select_statement);
    }

    // Trying to check if order by import new invisible column in project
    if(select_column_count < bound_select_statement->projection_expressions_.size()) {
        PruneOutput(query_context_ptr_, select_column_count, bound_select_statement);
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

    return bound_select_statement;
}

SharedPtr<TableRef>
QueryBinder::BuildFromClause(SharedPtr<QueryContext>& query_context,
                             const hsql::TableRef *from_table) {

    SharedPtr<TableRef> result = nullptr;
    switch(from_table->type) {
        case hsql::kTableName: {
            // Only one table: select * from t1;
            result = BuildTable(query_context, from_table);
            break;
        }
        case hsql::kTableSelect: {
            // select t1.a from (select * from t2 as t1);
            String table_alias = from_table->getName() != nullptr ? from_table->getName() : String();
            result = BuildSubquery(query_context, table_alias, *(from_table->select));
            break;
        }
        case hsql::kTableJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            result = BuildJoin(query_context, from_table);
            break;
        }
        case hsql::kTableCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            result = BuildCrossProduct(query_context, from_table);
            break;
        }

            // TODO: No case currently, since parser doesn't support it.
//        case hsql::kExpressionList: {
//            break;
//        }
    }

    return result;
}

SharedPtr<TableRef>
QueryBinder::BuildDummyTable(SharedPtr<QueryContext>& query_context) {
    return nullptr;
}

SharedPtr<TableRef>
QueryBinder::BuildTable(SharedPtr<QueryContext>& query_context,
                        const hsql::TableRef* from_table) {
    // There are five cases here:
    // CTE*, which is subquery (may include correlated expression).
    // Recursive CTE (not supported by parser.)
    // View*, which is also a subquery (may include correlated expression).
    // Table*
    // External Table (not supported by parser.)

    // In AST, name is the alias of CTE and name of table
    // In AST, alias is the alias of table

    String name = from_table->name;
    String schema_name{};

    // If schema is null, it may from CTE
    // else the table will be checked in catalog
    if(from_table->schema == nullptr) {
        // Before find the table meta from catalog, Attempt to get CTE info from bind context which saved CTE info into before.
        if(SharedPtr<TableRef> cte_ref = BuildCTE(query_context, name); cte_ref != nullptr) {
            return cte_ref;
        }

        schema_name = "Default";
    }

    // Base Table
    if(SharedPtr<TableRef> base_table_ref = BuildBaseTable(query_context, from_table, schema_name); base_table_ref != nullptr) {
        return base_table_ref;
    }

    // View
    if(SharedPtr<TableRef> view_ref = BuildView(query_context, from_table, schema_name); view_ref != nullptr) {
        return view_ref;
    }

    PlannerError("Table or View: " + name +" is not found in catalog.");
}

SharedPtr<TableRef>
QueryBinder::BuildSubquery(SharedPtr<QueryContext>& query_context,
                           const String &table_alias,
                           const hsql::SelectStatement& select_stmt) {
    // Create new bind context and add into context array;
    SharedPtr<BindContext> subquery_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, subquery_bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(select_stmt);

//    auto bound_select_node_ptr = PlanBuilder::BuildSelect(query_context, select_stmt, subquery_bind_context_ptr);
    this->bind_context_ptr_->AddSubQueryChild(subquery_bind_context_ptr);

    // Get the table index of the sub query output
    u64 subquery_table_index = bound_statement_ptr->GetTableIndex();

    String binding_name = table_alias.empty() ? "subquery" + std::to_string(subquery_table_index) : table_alias;
    // Add binding into bind context
    this->bind_context_ptr_->AddSubqueryBinding(binding_name,
                                                subquery_table_index,
                                                *bound_statement_ptr->types_ptr_,
                                                *bound_statement_ptr->names_ptr_);

    // Use binding name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_statement_ptr, binding_name);

    // TODO: Not care about the correlated expression

    return subquery_table_ref_ptr;
}

SharedPtr<TableRef>
QueryBinder::BuildCTE(SharedPtr<QueryContext>& query_context,
                      const String &name) {
    SharedPtr<CommonTableExpressionInfo> cte = this->bind_context_ptr_->GetCTE(name);
    if(cte == nullptr) {
        return nullptr;
    }

    // Table is from CTE
    if(this->bind_context_ptr_->IsCTEBound(cte)) {
        // The CTE is bound before.
        PlannerError("CTE can only be bound only once");
    }

    // Build CTE(subquery)
    // insert into CTE Bound in bind context
    this->bind_context_ptr_->BoundCTE(cte);

    // Create new bind context and add into context array;
    SharedPtr<BindContext> subquery_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, subquery_bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(*cte->select_statement_);

    this->bind_context_ptr_->AddSubQueryChild(subquery_bind_context_ptr);

    u64 cte_table_index = bound_statement_ptr->GetTableIndex();
    // Add binding into bind context
    this->bind_context_ptr_->AddCTEBinding(name,
                                           cte_table_index,
                                           *bound_statement_ptr->types_ptr_,
                                           *bound_statement_ptr->names_ptr_);

    // Use CTE name as the subquery table reference name
    auto cte_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_statement_ptr, name);

    // TODO: Not care about the correlated expression

    return cte_table_ref_ptr;
}

SharedPtr<TableRef>
QueryBinder::BuildBaseTable(SharedPtr<QueryContext>& query_context,
                            const hsql::TableRef* from_table,
                            const String &schema_name) {
    String table_name = from_table->name;
    auto table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, table_name);
    if(table_ptr == nullptr) {
        // Not found in catalog
        return nullptr;
    }

    // TODO: Handle table and column alias

    u64 table_index = this->bind_context_ptr_->GenerateTableIndex();
    String alias = from_table->getName();
    Vector<DataType> types;
    Vector<String> names;
    Vector<SizeT> columns;

    SizeT column_count = table_ptr->ColumnCount();
    types.reserve(column_count);
    names.reserve(column_count);
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        types.emplace_back(table_ptr->GetColumnTypeById(idx));
        names.emplace_back(table_ptr->GetColumnNameById(idx));
        columns.emplace_back(idx);
    }

    SharedPtr<TableScanFunction> scan_function = TableScanFunction::Make("seq_scan");
    SharedPtr<TableScanFunctionData> function_data = MakeShared<TableScanFunctionData>(table_ptr, columns);

    auto table_ref = MakeShared<BaseTableRef>(scan_function,
                                              function_data,
                                              alias,
                                              table_index,
                                              names,
                                              types);

    // Insert the table in the binding context
    this->bind_context_ptr_->AddTableBinding(alias, table_index, table_ptr, types, names);

    return table_ref;
}

SharedPtr<TableRef>
QueryBinder::BuildView(SharedPtr<QueryContext>& query_context,
                       const hsql::TableRef* from_table,
                       const String &schema_name) {
    String view_name = from_table->name;
    SharedPtr<View> view_ptr = Infinity::instance().catalog()->GetViewByName(schema_name, view_name);
    if(view_ptr == nullptr) {
        // Not found in catalog
        return nullptr;
    }

    // Build view scan operator
    PlannerAssert(!(this->bind_context_ptr_->IsViewBound(view_name)), "View: " + view_name + " is bound before!");
    this->bind_context_ptr_->BoundView(view_name);

    const hsql::SQLStatement* sql_statement = view_ptr->GetSQLStatement();
    PlannerAssert(sql_statement->type() != hsql::StatementType::kStmtSelect, "View related statement isn't a select statement.");

    auto* select_stmt_ptr = static_cast<const hsql::SelectStatement*>(sql_statement);

    // Create new bind context and add into context array;
    SharedPtr<BindContext> view_bind_context_ptr = BindContext::Make(this->bind_context_ptr_);

    // Create bound select node and subquery table reference
    QueryBinder subquery_binder(this->query_context_ptr_, view_bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = subquery_binder.BindSelect(*select_stmt_ptr);

    this->bind_context_ptr_->AddSubQueryChild(view_bind_context_ptr);

    u64 view_index = this->bind_context_ptr_->GenerateTableIndex();
    // Add binding into bind context
    this->bind_context_ptr_->AddViewBinding(view_name,
                                            view_index,
                                            *bound_statement_ptr->types_ptr_,
                                            *bound_statement_ptr->names_ptr_);

    // Use view name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_statement_ptr, view_name);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

SharedPtr<TableRef>
QueryBinder::BuildCrossProduct(SharedPtr<QueryContext>& query_context,
                               const hsql::TableRef* from_table) {
    // Create left and right bind context
    // Build left and right table with new bind context
    // Merge two bind context into their parent context

    Vector<SharedPtr<QueryBinder>> binder_stack;
    SharedPtr<QueryBinder> left_query_binder = shared_from_this();
    binder_stack.push_back(left_query_binder);

    SharedPtr<QueryBinder> cross_product_query_binder = nullptr;
    const Vector<hsql::TableRef*>& tables = *from_table->list;
    auto table_count = tables.size();
    for(auto i = table_count - 1; i > 0 ; -- i) {

        SharedPtr<QueryBinder> right_query_binder = MakeShared<QueryBinder>(query_context, left_query_binder->bind_context_ptr_);
        binder_stack.emplace_back(right_query_binder);

        cross_product_query_binder = MakeShared<QueryBinder>(query_context, left_query_binder->bind_context_ptr_);
        left_query_binder = cross_product_query_binder;
        binder_stack.emplace_back(left_query_binder);
    }

    left_query_binder = binder_stack.back();
    binder_stack.pop_back();

    hsql::TableRef* left_table_ptr = tables[0];
    SharedPtr<TableRef> left_bound_table_ref = left_query_binder->BuildFromClause(query_context, left_table_ptr);

    for(auto i = 1; i < table_count; ++ i) {
        // Right node
        auto right_query_binder = binder_stack.back();
        binder_stack.pop_back();
        hsql::TableRef* right_table_ptr = tables[i];
        SharedPtr<TableRef> right_bound_table_ref = right_query_binder->BuildFromClause(query_context, right_table_ptr);

        // Cross product node with dummy name
        auto cross_product_table_ref = MakeShared<CrossProductTableRef>(String());
        cross_product_table_ref->left_bind_context_ = left_query_binder->bind_context_ptr_;
        cross_product_table_ref->left_table_ref_ = left_bound_table_ref;
        cross_product_table_ref->right_bind_context_ = right_query_binder->bind_context_ptr_;
        cross_product_table_ref->right_table_ref_ = right_bound_table_ref;

        cross_product_query_binder = binder_stack.back();
        binder_stack.pop_back();
        cross_product_query_binder->bind_context_ptr_->AddLeftChild(left_query_binder->bind_context_ptr_);
        cross_product_query_binder->bind_context_ptr_->AddRightChild(right_query_binder->bind_context_ptr_);

        left_query_binder = cross_product_query_binder;
        left_bound_table_ref = cross_product_table_ref;
    }

    this->bind_context_ptr_ = left_query_binder->bind_context_ptr_;
    return left_bound_table_ref;
}

SharedPtr<TableRef>
QueryBinder::BuildJoin(SharedPtr<QueryContext>& query_context,
                       const hsql::TableRef *from_table) {

    String alias = from_table->getName();
    auto result = MakeShared<JoinTableRef>(alias);

    switch (from_table->join->type) {
        case hsql::JoinType::kJoinCross: result->join_type_ = JoinType::kCross; break;
        case hsql::JoinType::kJoinFull: result->join_type_ = JoinType::kFull; break;
        case hsql::JoinType::kJoinInner: result->join_type_ = JoinType::kInner; break;
        case hsql::JoinType::kJoinNatural: result->join_type_ = JoinType::kNatural; break;
        case hsql::JoinType::kJoinLeft: result->join_type_ = JoinType::kLeft; break;
        case hsql::JoinType::kJoinRight: result->join_type_ = JoinType::kRight; break;
        default:
            PlannerError("Unsupported join type.")
    }

    // Build left child
    SharedPtr<QueryBinder> left_query_binder = MakeShared<QueryBinder>(query_context, this->bind_context_ptr_);
    auto left_bound_table_ref = left_query_binder->BuildFromClause(query_context, from_table->join->left);
    this->bind_context_ptr_->AddLeftChild(left_query_binder->bind_context_ptr_);

    // Build right child
    SharedPtr<QueryBinder> right_query_binder = MakeShared<QueryBinder>(query_context, this->bind_context_ptr_);
    auto right_bound_table_ref = right_query_binder->BuildFromClause(query_context, from_table->join->right);
    this->bind_context_ptr_->AddRightChild(right_query_binder->bind_context_ptr_);

    result->left_table_ref_ = left_bound_table_ref;
    result->left_bind_context_ = left_query_binder->bind_context_ptr_;

    result->right_table_ref_ = right_bound_table_ref;
    result->right_bind_context_ = right_query_binder->bind_context_ptr_;

    // Current parser doesn't support On using column syntax, so only consider the case of natural join.
    if(result->join_type_ == JoinType::kNatural) {
        HashSet<String> left_binding_column_names;
        // TODO: Is there any way to get all_column_names size ? Collect all left binding columns numbers at very beginning?
        for(auto& left_binding_pair: result->left_bind_context_->binding_by_name_) {
            for(auto& left_column_name: left_binding_pair.second->column_names_) {
                left_binding_column_names.emplace(left_column_name);
            }
        }

        Vector<String> using_column_names;
        // TODO: column count of left binding tables and right binding tables is using_column_names size
        for(auto& right_binding_pair: result->right_bind_context_->binding_by_name_) {
            for(auto& right_column_name: right_binding_pair.second->column_names_) {
                if(left_binding_column_names.contains(right_column_name)) {
                    using_column_names.emplace_back(right_column_name);
                }
            }
        }

        if(using_column_names.empty()) {
            // It is cross product, but not a natural join with dummy name
            auto cross_product_table_ref = MakeShared<CrossProductTableRef>(String());
            cross_product_table_ref->left_bind_context_ = result->left_bind_context_;
            cross_product_table_ref->left_table_ref_ = left_bound_table_ref;

            cross_product_table_ref->right_bind_context_ = result->right_bind_context_;
            cross_product_table_ref->right_table_ref_ = right_bound_table_ref;
            return cross_product_table_ref;
        } else {
            // It is a natural join, we only consider the inner natural join case.
            result->join_type_ = JoinType::kInner;

            // Reserve the on condition space for the expression.
            result->on_conditions_.reserve(using_column_names.size());

            // TODO: Construct join condition: left_column_expr = right_column_expr AND left_column_expr = right_column_expr;
            for(auto& column_name: using_column_names) {
                // Create left bound column expression
                PlannerAssert(result->left_bind_context_->binding_names_by_column_.contains(column_name), "Column: " + column_name + " doesn't exist in left table");

                auto& left_column_binding_names = result->left_bind_context_->binding_names_by_column_[column_name];

                PlannerAssert(left_column_binding_names.size() == 1, "Ambiguous column name: " + column_name + " in left table");

                auto& left_binding_name = left_column_binding_names[0];
                auto& left_binding_ptr = result->left_bind_context_->binding_by_name_[left_binding_name];
                auto left_column_index = left_binding_ptr->name2index_[column_name];
                auto left_column_type = left_binding_ptr->column_types_[left_column_index];

                SharedPtr<ColumnExpression> left_column_expression_ptr =
                        MakeShared<ColumnExpression>(left_column_type, left_binding_ptr->table_name_, column_name, left_column_index, 0);

                PlannerAssert(result->right_bind_context_->binding_names_by_column_.contains(column_name), "Column: " + column_name + " doesn't exist in right table");

                auto& right_column_binding_names = result->right_bind_context_->binding_names_by_column_[column_name];

                PlannerAssert(right_column_binding_names.size() == 1, "Ambiguous column name: " + column_name + " in right table");


                auto& right_binding_name = right_column_binding_names[0];
                auto& right_binding_ptr = result->right_bind_context_->binding_by_name_[right_binding_name];
                PlannerAssert(right_binding_ptr != nullptr, "Column: " + column_name + " doesn't exist in right table");
                auto right_column_index = right_binding_ptr->name2index_[column_name];
                auto right_column_type = right_binding_ptr->column_types_[right_column_index];

                SharedPtr<ColumnExpression> right_column_expression_ptr =
                        MakeShared<ColumnExpression>(right_column_type, right_binding_ptr->table_name_, column_name, right_column_index, 0);

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
    auto on_condition_ptr = join_binder->BuildExpression(*from_table->join->condition,
                                                         this->bind_context_ptr_,
                                                         0,
                                                         true);

    result->on_conditions_ = SplitExpressionByDelimiter(on_condition_ptr, ConjunctionType::kAnd);

    return result;
}

Vector<SharedPtr<ParsedExpression>>
QueryBinder::UnfoldStarExpression(SharedPtr<QueryContext>& query_context,
                                  const Vector<hsql::Expr *> &input_select_list) {
    Vector<SharedPtr<ParsedExpression>> output_select_list;
    output_select_list.reserve(input_select_list.size());
    for(const hsql::Expr* select_expr: input_select_list) {
        switch (select_expr->type) {
            case hsql::kExprStar: {
                SharedPtr<Binding> binding;
                String table_name;
                if (select_expr->table == nullptr) {
                    // select * from t1;
                    PlannerAssert(!this->bind_context_ptr_->table_names_.empty(), "No table is bound.");

                    // Use first table as the default table: select * from t1, t2; means select t1.* from t1, t2;
                    table_name = this->bind_context_ptr_->table_names_[0];
                    binding = this->bind_context_ptr_->binding_by_name_[table_name];
                } else {
                    // select t1.* from t1;
                    table_name = select_expr->table;
                    if (this->bind_context_ptr_->binding_by_name_.contains(table_name)) {
                        binding = this->bind_context_ptr_->binding_by_name_[table_name];
                    } else {
                        PlannerAssert(!this->bind_context_ptr_->table_names_.empty(),
                                      "Table: '" + table_name + "' not found in select list.");
                    }
                }

                SizeT column_count = binding->table_ptr_->ColumnCount();

                // Reserve more data in select list
                output_select_list.reserve(output_select_list.size() + column_count);

                // Build select list
                SharedPtr<String> table_name_ptr = MakeShared<String>(table_name);
                for(SizeT idx = 0; idx < column_count; ++ idx) {
                    String column_name = binding->table_ptr_->GetColumnNameById(idx);
                    SharedPtr<ParsedColumnExpression> bound_column_expr
                            = MakeShared<ParsedColumnExpression>(String(), String(), table_name, column_name);
                    output_select_list.emplace_back(bound_column_expr);
                }

                break;
            }
            default: {
                SharedPtr<ParsedRawExpression> raw_expr = MakeShared<ParsedRawExpression>(select_expr);
                output_select_list.emplace_back(raw_expr);
            }
        }
    }
    return output_select_list;
}

void
QueryBinder::BuildGroupBy(SharedPtr<QueryContext>& query_context,
                          const hsql::SelectStatement& select,
                          const SharedPtr<BindAliasProxy>& bind_alias_proxy,
                          SharedPtr<BoundSelectStatement>& select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->group_by_table_index_ = table_index;
    bind_context_ptr_->group_by_table_name_ = "groupby" + std::to_string(table_index);

    if(select.groupBy != nullptr) {
        // Start to bind GROUP BY clause
        // Set group binder
        auto group_binder = MakeShared<GroupBinder>(query_context, bind_alias_proxy);

        // Reserve the group names used in GroupBinder::BuildExpression
        SizeT group_count = select.groupBy->columns->size();
        bind_context_ptr_->group_exprs_.reserve(group_count);
        for(i64 idx = 0; idx < group_count; ++ idx) {
            // set group-by expression index
            group_binder->group_by_expr_index = idx;
            const hsql::Expr& expr = *(*select.groupBy->columns)[0];

            // Call GroupBinder BuildExpression
            SharedPtr<BaseExpression> group_by_expr = group_binder->Bind(expr, this->bind_context_ptr_, 0, true);
            select_statement->group_by_expressions_.emplace_back(group_by_expr);
        }
    }
}

void
QueryBinder::BuildHaving(SharedPtr<QueryContext>& query_context,
                         const hsql::SelectStatement& select,
                         const SharedPtr<BindAliasProxy>& bind_alias_proxy,
                         SharedPtr<BoundSelectStatement>& select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->aggregate_table_index_ = table_index;
    bind_context_ptr_->aggregate_table_name_ = "aggregate" + std::to_string(table_index);

    // All having expr must appear in group by list or aggregate function list.
    if(select.groupBy != nullptr && select.groupBy->having != nullptr) {
        // Start to bind Having clause
        // Set having binder
        auto having_binder = MakeShared<HavingBinder>(query_context, bind_alias_proxy);
        for (const hsql::Expr* expr: *select.groupBy->having->exprList) {

            // Call HavingBinder BuildExpression
            SharedPtr<BaseExpression> having_expr = having_binder->Bind(*expr, this->bind_context_ptr_, 0, true);
            select_statement->having_expressions_.emplace_back(having_expr);

        }
    }
}

void
QueryBinder::PushOrderByToProject(SharedPtr<QueryContext>& query_context,
                                  const hsql::SelectStatement& statement) {
    for(const hsql::OrderDescription* order_desc: *statement.order) {
        OrderBinder::PushExtraExprToSelectList(order_desc->expr, bind_context_ptr_);
    }
}

void
QueryBinder::BuildSelectList(SharedPtr<QueryContext>& query_context,
                             SharedPtr<BoundSelectStatement>& bound_select_statement) {
    u64 table_index = bind_context_ptr_->GenerateTableIndex();
    bind_context_ptr_->project_table_index_ = table_index;
    bind_context_ptr_->project_table_name_ = "project" + std::to_string(table_index);

    auto project_binder = MakeShared<ProjectBinder>(query_context_ptr_);

    SizeT column_count = bind_context_ptr_->select_expression_.size();
    bind_context_ptr_->project_exprs_.reserve(column_count);
    bound_select_statement->projection_expressions_.reserve(column_count);
    for(SizeT column_id = 0; column_id < column_count; ++ column_id) {
        const SharedPtr<ParsedExpression>& select_expr = bind_context_ptr_->select_expression_[column_id];
        SharedPtr<BaseExpression> bound_expr = nullptr;
        String expr_name;
        switch(select_expr->type_) {
            case ExpressionType::kColumn: {
                // This part is from unfold star expression, no alias
                SharedPtr<ParsedColumnExpression> parsed_col_expr
                        = std::static_pointer_cast<ParsedColumnExpression>(select_expr);
                bound_expr = project_binder->ExpressionBinder::BuildColExpr(parsed_col_expr,
                                                                            this->bind_context_ptr_,
                                                                            0,
                                                                            true);
                expr_name = parsed_col_expr->ToString();
                break;
            }
            case ExpressionType::kRaw: {
                SharedPtr<ParsedRawExpression> parsed_raw_expr
                        = std::static_pointer_cast<ParsedRawExpression>(select_expr);

                bound_expr = project_binder->Bind(*parsed_raw_expr->raw_expr_,
                                                  this->bind_context_ptr_,
                                                  0,
                                                  true);
                expr_name = parsed_raw_expr->ToString();
                break;
            }
            default: {
                PlannerError("Invalid type in select list.")
            }
        }
        if(!(bind_context_ptr_->project_index_by_name_.contains(expr_name))) {
            bind_context_ptr_->project_index_by_name_[expr_name] = column_id;
        }
        bind_context_ptr_->project_exprs_.emplace_back(bound_expr);

        // Insert the bound expression into projection expressions of select node.
        bound_select_statement->projection_expressions_.emplace_back(bound_expr);
    }

    if(!bound_select_statement->having_expressions_.empty() || !bound_select_statement->group_by_expressions_.empty()) {
        if(!project_binder->BoundColumn().empty()) {
            PlannerError("Column: " + project_binder->BoundColumn()
                         + " must appear in the GROUP BY clause or be used in an aggregate function");
        }
    }
}

void
QueryBinder::BuildOrderBy(SharedPtr<QueryContext>& query_context,
                          const hsql::SelectStatement& statement,
                          SharedPtr<BoundSelectStatement>& bound_statement) const {
    auto order_binder = MakeShared<OrderBinder>(query_context);
    SizeT order_by_count = statement.order->size();
    bound_statement->order_by_expressions_.reserve(order_by_count);
    bound_statement->order_by_types_.reserve(order_by_count);
    for(const hsql::OrderDescription* order_desc: *statement.order) {
        switch (order_desc->type) {
            case hsql::kOrderAsc:
                bound_statement->order_by_types_.emplace_back(OrderByType::kAscending);
                break;
            case hsql::kOrderDesc:
                bound_statement->order_by_types_.emplace_back(OrderByType::kDescending);
                break;
        }
        auto bound_order_expr = order_binder->Bind(*order_desc->expr,
                                                   this->bind_context_ptr_,
                                                   0,
                                                   true);
        bound_statement->order_by_expressions_.emplace_back(bound_order_expr);
    }
}

void
QueryBinder::BuildLimit(SharedPtr<QueryContext>& query_context,
                        const hsql::SelectStatement& statement,
                        SharedPtr<BoundSelectStatement>& bound_statement) const {
    auto limit_binder = MakeShared<LimitBinder>(query_context);
    bound_statement->limit_expression_
            = limit_binder->Bind(*statement.limit->limit, this->bind_context_ptr_, 0, true);

    if(statement.limit->offset != nullptr) {
        bound_statement->offset_expression_
                = limit_binder->Bind(*statement.limit->offset, this->bind_context_ptr_, 0, true);
    }
}

void
QueryBinder::PruneOutput(SharedPtr<QueryContext>& query_context,
                         i64 select_column_count,
                         SharedPtr<BoundSelectStatement>& bound_statement) {
    Vector<SharedPtr<BaseExpression>>& pruned_expressions = bound_statement->pruned_expression_;
    Vector<SharedPtr<BaseExpression>>& projection_expressions = bound_statement->projection_expressions_;
    Vector<String>& output_names = bound_statement->output_names_;

    pruned_expressions.reserve(select_column_count);

    for(i64 column_id = 0; column_id < select_column_count; ++ column_id) {
        const SharedPtr<BaseExpression>& expr = projection_expressions[column_id];
        SharedPtr<ColumnExpression> result = ColumnExpression::Make(expr->Type(),
                                                                    bind_context_ptr_->project_table_name_,
                                                                    output_names[column_id],
                                                                    column_id,
                                                                    0);
        result->source_position_ = SourcePosition(bind_context_ptr_->binding_context_id_, ExprSourceType::kProjection);
        pruned_expressions.emplace_back(result);
    }
}

}
