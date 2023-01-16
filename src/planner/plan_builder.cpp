//
// Created by JinHai on 2022/9/11.
//

#include "plan_builder.h"

#include "main/infinity.h"

#include "node/logical_create_table.h"
#include "node/logical_drop_table.h"
#include "node/logical_chunk_scan.h"
#include "node/logical_table_scan.h"
#include "node/logical_view_scan.h"
#include "node/logical_insert.h"
#include "node/logical_filter.h"

#include "bound/subquery_flattener.h"

#include "bound/base_table_ref.h"
#include "bound/subquery_table_ref.h"
#include "bound/cross_product_table_ref.h"
#include "bound/join_table_ref.h"
#include "bound/dummy_table_ref.h"

#include "binder/aggregate_binder.h"
#include "binder/group_binder.h"
#include "binder/having_binder.h"
#include "binder/insert_binder.h"
#include "binder/project_binder.h"
#include "binder/where_binder.h"
#include "binder/join_binder.h"
#include "binder/order_binder.h"
#include "binder/limit_binder.h"
#include "binder/bind_alias_proxy.h"

#include "storage/table_definition.h"
#include "storage/column_definition.h"
#include "storage/table_with_fix_row.h"

#include "common/utility/infinity_assert.h"

#include "expression/value_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"

#include "expression/expression_transformer.h"


namespace infinity {

PlanBuildingContext
PlanBuilder::BuildPlan(SharedPtr<QueryContext>& query_context, const hsql::SQLStatement &statement) {

    // Create the bind context and insert it into the root bind context array and assign an id to it.
    // TODO: consider to move the bind context construct into BuildXXXX method.
    SharedPtr<BindContext> bind_context_ptr = MakeShared<BindContext>(nullptr);
//    this->AddBindContextArray(bind_context_ptr);

    PlanBuildingContext plan_context;
    switch (statement.type()) {
        case hsql::kStmtSelect: {
            auto bound_select_node = BuildSelect(query_context,
                                                 static_cast<const hsql::SelectStatement &>(statement),
                                                 bind_context_ptr);

            plan_context.plan = bound_select_node->BuildPlan();
//            SubqueryFlattener flattner(bound_select_node,  bind_context_ptr);
//            bound_select_node = flattner.GetResult();
            // Need to create plan and construct the plan context;
            break;
        }
        case hsql::kStmtInsert:
            plan_context = BuildInsert(query_context,
                                       static_cast<const hsql::InsertStatement&>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtDelete:
            plan_context = BuildDelete(query_context,
                                       static_cast<const hsql::DeleteStatement&>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtUpdate:
            plan_context = BuildUpdate(query_context,
                                       static_cast<const hsql::UpdateStatement&>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtShow:
            plan_context = BuildShow(query_context,
                                     static_cast<const hsql::ShowStatement&>(statement),
                                     bind_context_ptr);
            break;
        case hsql::kStmtCreate:
            plan_context = BuildCreate(query_context,
                                       static_cast<const hsql::CreateStatement &>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtDrop:
            plan_context = BuildDrop(query_context,
                                     static_cast<const hsql::DropStatement &>(statement),
                                     bind_context_ptr);
            break;
        case hsql::kStmtPrepare:
            plan_context = BuildPrepare(query_context,
                                        static_cast<const hsql::PrepareStatement&>(statement),
                                        bind_context_ptr);
            break;
        case hsql::kStmtExecute:
            plan_context = BuildExecute(query_context,
                                        static_cast<const hsql::ExecuteStatement&>(statement),
                                        bind_context_ptr);
            break;
        case hsql::kStmtImport:
            plan_context = BuildImport(query_context,
                                       static_cast<const hsql::ImportStatement&>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtExport:
            plan_context = BuildExport(query_context,
                                       static_cast<const hsql::ExportStatement&>(statement),
                                       bind_context_ptr);
            break;
        case hsql::kStmtTransaction:
            plan_context = BuildTransaction(query_context,
                                            static_cast<const hsql::TransactionStatement&>(statement),
                                            bind_context_ptr);
            break;
        case hsql::kStmtAlter:
            plan_context = BuildAlter(query_context,
                                      static_cast<const hsql::AlterStatement&>(statement),
                                      bind_context_ptr);
            break;
        case hsql::kStmtError:
        case hsql::kStmtRename:
            PlannerError("Statement type not supported");
        default:
            PlannerError("Invalid statement type");
    }

    return plan_context;
}

//std::vector<SharedPtr<BindContext>>&
//PlanBuilder::BindContextArray() {
//    return bind_context_array_;
//}

//void
//PlanBuilder::AddBindContextArray(SharedPtr<BindContext>& bind_context_ptr) {
//    std::vector<SharedPtr<BindContext>>& root_bind_context_array = this->BindContextArray();
//    bind_context_ptr->id_ = root_bind_context_array.size();
//    root_bind_context_array.emplace_back(bind_context_ptr);
//}

// Build create statement to logical create operator
// Including: create table / view
PlanBuildingContext
PlanBuilder::BuildCreate(SharedPtr<QueryContext>& query_context,
                         const hsql::CreateStatement& statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    switch (statement.type) {
        case hsql::CreateType::kCreateTable: {
            return BuildCreateTable(query_context, statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateView: {
            return BuildCreateView(query_context, statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateTableFromTbl: {
            return BuildCreateTableFromTable(query_context, statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateIndex: {
            return BuildCreateIndex(query_context, statement, bind_context_ptr);;
        }
        default: {
            PlannerError("Creating type isn't supported");
        }
    }

    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildCreateTable(SharedPtr<QueryContext>& query_context,
                              const hsql::CreateStatement& statement,
                              SharedPtr<BindContext>& bind_context_ptr) {
    // Check if columns is given.
    std::vector<SharedPtr<ColumnDef>> columns;
    for(size_t idx = 0; idx < statement.columns->size(); ++ idx) {
        const hsql::ColumnDefinition* statement_column = statement.columns->at(idx);
        String column_name(statement_column->name);
        bool nullable = statement_column->nullable;
        DataType column_data_type = DataType::ConvertType(statement_column->type);
        std::set<ConstrainType> constraints;
        for(hsql::ConstraintType constraint_type: *(statement_column->column_constraints)) {
            switch (constraint_type) {
                case hsql::ConstraintType::None: {
                    TypeError("Unexpected constraint type.")
                }
                case hsql::ConstraintType::NotNull: {
                    constraints.insert(ConstrainType::kNotNull);
                    break;
                }
                case hsql::ConstraintType::Null: {
                    constraints.insert(ConstrainType::kNull);
                    break;
                }
                case hsql::ConstraintType::PrimaryKey: {
                    constraints.insert(ConstrainType::kPrimaryKey);
                    break;
                }
                case hsql::ConstraintType::Unique: {
                    constraints.insert(ConstrainType::kUnique);
                    break;
                }
            }
        }

        SharedPtr<ColumnDef> column_def_ptr = MakeShared<ColumnDef>(column_name, idx, column_data_type, constraints);
        columns.emplace_back(column_def_ptr);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = MakeShared<String>(statement.schema);
    }

    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(statement.tableName, columns, statement.ifNotExists);

    SharedPtr<LogicalNode> logical_create_table_operator
            = MakeShared<LogicalCreateTable>(schema_name_ptr, table_def_ptr, bind_context_ptr);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_create_table_operator, bind_context_ptr);

    if(statement.select != nullptr) {
        // TODO: build select here, bind context is needed.
//        SharedPtr<LogicalOperator> select_node = BuildSelect(*statement.select);
//        logical_create_table_operator->set_left_node(select_node);
    }

    PlanBuildingContext res;
    res.plan = logical_create_table_operator;

    return res;
}

PlanBuildingContext
PlanBuilder::BuildCreateTableFromTable(SharedPtr<QueryContext>& query_context,
                                       const hsql::CreateStatement &statement,
                                       SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Creating table from table isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildCreateView(SharedPtr<QueryContext>& query_context,
                             const hsql::CreateStatement &statement,
                             SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Creating view isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildCreateIndex(SharedPtr<QueryContext>& query_context,
                              const hsql::CreateStatement &statement,
                              SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Creating index isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDrop(SharedPtr<QueryContext>& query_context,
                       const hsql::DropStatement &statement,
                       SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kDropTable:
            return BuildDropTable(query_context, statement, bind_context_ptr);
        case hsql::kDropSchema:
            return BuildDropSchema(query_context, statement, bind_context_ptr);
        case hsql::kDropIndex:
            return BuildDropIndex(query_context, statement, bind_context_ptr);
        case hsql::kDropView:
            return BuildDropView(query_context, statement, bind_context_ptr);
        case hsql::kDropPreparedStatement:
            return BuildDropPreparedStatement(query_context, statement, bind_context_ptr);
        default: {
            PlannerError("Deleting type isn't supported");
        }
    }

    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDropTable(SharedPtr<QueryContext>& query_context,
                            const hsql::DropStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {

    SharedPtr<String> schema_name_ptr = MakeShared<String>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = MakeShared<String>(statement.schema);
    }

    SharedPtr<LogicalNode> logical_drop_table
            = MakeShared<LogicalDropTable>(schema_name_ptr,
                                                 MakeShared<String>(statement.name), bind_context_ptr);
    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_drop_table, bind_context_ptr);

    PlanBuildingContext res;
    res.plan = logical_drop_table;

    return res;
}

PlanBuildingContext
PlanBuilder::BuildDropSchema(SharedPtr<QueryContext>& query_context,
                             const hsql::DropStatement &statement,
                             SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping schema isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDropIndex(SharedPtr<QueryContext>& query_context,
                            const hsql::DropStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping index isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDropView(SharedPtr<QueryContext>& query_context,
                           const hsql::DropStatement &statement,
                           SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping view isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDropPreparedStatement(SharedPtr<QueryContext>& query_context,
                                        const hsql::DropStatement &statement,
                                        SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping prepared statement isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildInsert(SharedPtr<QueryContext>& query_context,
                         const hsql::InsertStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    bind_context_ptr->expression_binder_ = MakeShared<InsertBinder>(query_context);
    switch(statement.type) {
        case hsql::kInsertValues:{
            return BuildInsertValue(query_context, statement, bind_context_ptr);
        }
        case hsql::kInsertSelect:
            return BuildInsertSelect(query_context, statement, bind_context_ptr);
        default:
            PlannerError("Inserting type isn't supported");
    }
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildInsertValue(SharedPtr<QueryContext>& query_context,
                              const hsql::InsertStatement &statement,
                              SharedPtr<BindContext>& bind_context_ptr) {
    // Get schema name
    String schema_name = statement.schema == nullptr? "Default" : statement.schema;

    // Get table name
    if(statement.tableName == nullptr) {
        PlannerError("Insert statement missing table name.");
    }
    String table_name = String{statement.tableName};
    // Check schema and table in the catalog
    SharedPtr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, table_name);
    if(table_ptr == nullptr) { PlannerError(schema_name + "." + table_name + " not exists.")}

    // Create value list
    std::vector<SharedPtr<BaseExpression>> value_list;
    value_list.reserve(statement.values->size());
    for (const auto* expr : *statement.values) {
        SharedPtr<BaseExpression> value_expr
                = bind_context_ptr->expression_binder_->BuildExpression(*expr, bind_context_ptr);
        value_list.emplace_back(value_expr);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    if (statement.columns != nullptr) {
        size_t statement_column_count = statement.columns->size();
        PlannerAssert(statement_column_count == value_list.size(),
                      "INSERT: Target column count and input column count mismatch");

//        Value null_value = Value::MakeNullData();
//        SharedPtr<BaseExpression> null_value_expr = MakeShared<ValueExpression>(null_value);

        size_t table_column_count = table_ptr->ColumnCount();

        // Create value list with table column size and null value
        std::vector<std::shared_ptr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

        size_t column_idx = 0;
        for(const auto& column_name : *statement.columns) {
            // Get table index from the inserted value column name;
            size_t table_column_id = table_ptr->GetColumnIdByName(column_name);
            DataType table_column_type = table_ptr->GetColumnTypeById(table_column_id);
            DataType value_type = value_list[column_idx]->Type();
            if(value_type == table_column_type) {
                rewrite_value_list[table_column_id] = value_list[column_idx];
            } else {
                // If the inserted value type mismatches with table column type, cast the inserted value type to correct one.
                SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(value_list[column_idx], table_column_type);
                rewrite_value_list[table_column_id] = cast_expr;
            }
            ++ column_idx;
        }

        value_list = rewrite_value_list;
    } else {
        SizeT table_column_count = table_ptr->ColumnCount();

        // Create value list with table column size and null value
        std::vector<std::shared_ptr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

        for(SizeT column_idx = 0; column_idx < table_column_count; ++ column_idx) {
            DataType table_column_type = table_ptr->GetColumnTypeById(column_idx);
            DataType value_type = value_list[column_idx]->Type();
            if(table_column_type == value_type) {
                rewrite_value_list[column_idx] = value_list[column_idx];
            } else {
                SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(value_list[column_idx], table_column_type);
                rewrite_value_list[column_idx] = cast_expr;
            }
        }
        value_list = rewrite_value_list;
    }

    // Create logical insert node.
    SharedPtr<LogicalNode> logical_insert =
            MakeShared<LogicalInsert>(table_ptr, value_list, bind_context_ptr);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_insert, bind_context_ptr);

    PlanBuildingContext res;
    res.plan = logical_insert;

    return res;
}

PlanBuildingContext
PlanBuilder::BuildInsertSelect(SharedPtr<QueryContext>& query_context,
                               const hsql::InsertStatement &statement,
                               SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Inserting select isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildDelete(SharedPtr<QueryContext>& query_context,
                         const hsql::DeleteStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Delete isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildUpdate(SharedPtr<QueryContext>& query_context,
                         const hsql::UpdateStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Update isn't supported.");
    PlanBuildingContext res;
    return res;
}

SharedPtr<BoundSelectNode>
PlanBuilder::BuildSelect(SharedPtr<QueryContext>& query_context,
                         const hsql::SelectStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {

    PlannerAssert(statement.selectList != nullptr, "SELECT list is needed");
    PlannerAssert(!statement.selectList->empty(), "SELECT list can't be empty");

    // TODO: Move constructing bind context here ?
    auto bound_select_node = MakeShared<BoundSelectNode>(bind_context_ptr);

    // 1. WITH clause
    if (statement.withDescriptions != nullptr) {

        // Prepare to store the with statement
        size_t with_stmt_count = statement.withDescriptions->size();
        bind_context_ptr->CTE_map_.reserve(with_stmt_count);

        // Hash set to restrict the with statement name visibility
        std::unordered_set<String> masked_name_set;

        for(int64_t i = with_stmt_count - 1; i >= 0; -- i) {
            hsql::WithDescription* with_desc = (*statement.withDescriptions)[i];
            String name = with_desc->alias;
            if(bind_context_ptr->CTE_map_.contains(name)) {
                PlannerError("WITH query name: " + name + " occurs more than once.");
            }

            masked_name_set.insert(name);
            SharedPtr<CommonTableExpressionInfo> cte_info_ptr
                    = MakeShared<CommonTableExpressionInfo>(name, with_desc->select, masked_name_set);

            bind_context_ptr->CTE_map_[name] = cte_info_ptr;
        }
    }

    // 2. FROM clause (BaseTable, Join and Subquery)
    // 3. ON
    // 4. JOIN
    if (statement.fromTable != nullptr) {
        // Build table reference
        bound_select_node->table_ref_ptr_ = BuildFromClause(query_context, statement.fromTable, bind_context_ptr);
    } else {
        // No table reference, just evaluate the expr of the select list.
        bound_select_node->table_ref_ptr_ = BuildDummyTable(query_context, bind_context_ptr);
    }

    // TODO: Check if we need to create expression_binder here.
    bind_context_ptr->expression_binder_ = MakeShared<ProjectBinder>(query_context);

    // 5. SELECT list (aliases)
    // Unfold the star expression in the select list.
    // Star expression will be unfolded and bound as column expressions.
    std::vector<SelectItem> select_list = BuildSelectList(query_context, *statement.selectList, bind_context_ptr);

    std::unordered_map<String, const hsql::Expr *> alias2expr;
    for (const SelectItem& select_item : select_list) {

        // Bound column expression is bound due to the star expression, which won't be referenced in other place.
        // Only consider the Raw Expression case.
        if(select_item.type_ == SelectItemType::kRawExpr) {
            if(select_item.expr_->hasAlias()) {
                alias2expr.emplace(select_item.expr_->getName(), select_item.expr_);
            }
        }
    }

    SharedPtr<BindAliasProxy> bind_alias_proxy = MakeShared<BindAliasProxy>(alias2expr);

    // 6. WHERE
    if (statement.whereClause) {
        auto where_binder = MakeShared<WhereBinder>(query_context, bind_alias_proxy);
        SharedPtr<BaseExpression> where_expr =
                where_binder->BuildExpression(*statement.whereClause, bind_context_ptr);

        bound_select_node->where_conditions_ =
                SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);
    }

    // 7. GROUP BY
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    // 10. DISTINCT
    BuildGroupByHaving(query_context,  statement, bind_alias_proxy, bind_context_ptr, bound_select_node);

    // 11. SELECT (not flatten subquery)
    auto project_binder = MakeShared<ProjectBinder>(query_context);
    bind_context_ptr->project_names_.reserve(select_list.size());
    bound_select_node->projection_expressions_.reserve(select_list.size());
    for (const SelectItem& select_item : select_list) {
        switch(select_item.type_) {
            case SelectItemType::kBoundColumnExpr: {
                // This part is from unfold star expression, no alias
                const String& expr_name_ref = select_item.column_expr_->ToString();
                if(!bind_context_ptr->project_by_name_.contains(expr_name_ref)) {
                    bind_context_ptr->project_by_name_.emplace(expr_name_ref, select_item.column_expr_);
                }
                bind_context_ptr->project_names_.emplace_back(expr_name_ref);

                // Insert the bound expression into projection expressions of select node.
                bound_select_node->projection_expressions_.emplace_back(select_item.column_expr_);
                break;
            }
            case SelectItemType::kRawExpr: {

                String expr_name;
                SharedPtr<BaseExpression> bound_expr;
                if(select_item.expr_->getName() == nullptr) {
                    // Constant value in select list, call build value expression directly.
                    bound_expr = project_binder->BuildExpression(*select_item.expr_, bind_context_ptr);
                    expr_name = bound_expr->ToString();
                    bind_context_ptr->project_by_name_.emplace(expr_name, bound_expr);
                } else {
                    expr_name = select_item.expr_->getName();

                    // Alias already been bound and insert into project_by_name_ of bind context;
                    bound_expr = bind_context_ptr->project_by_name_[expr_name];
                    if(bound_expr == nullptr) {
                        bound_expr = project_binder->BuildExpression(*select_item.expr_, bind_context_ptr);
                        bind_context_ptr->project_by_name_.emplace(expr_name, bound_expr);
                    }
                }

                bind_context_ptr->project_names_.emplace_back(expr_name);

                // Insert the bound expression into projection expressions of select node.
                bound_select_node->projection_expressions_.emplace_back(bound_expr);
                break;
            }
            default:
                PlannerError("Invalid type in select list.")
        }
    }

    if(!bound_select_node->having_expressions_.empty() || !bound_select_node->group_by_expressions_.empty()) {
        if(!project_binder->BoundColumn().empty()) {
            PlannerError("Column: " + project_binder->BoundColumn() + " must appear in the GROUP BY clause or be used in an aggregate function");
        }
    }

    // 12. ORDER BY
    if(statement.order != nullptr) {
        auto order_binder = MakeShared<OrderBinder>(query_context);
        size_t order_by_count = statement.order->size();
        bound_select_node->order_by_expressions_.reserve(order_by_count);
        bound_select_node->order_by_types_.reserve(order_by_count);
        for(const hsql::OrderDescription* order_desc: *statement.order) {
            switch (order_desc->type) {
                case hsql::kOrderAsc:
                    bound_select_node->order_by_types_.emplace_back(OrderByType::kAscending);
                    break;
                case hsql::kOrderDesc:
                    bound_select_node->order_by_types_.emplace_back(OrderByType::kDescending);
                    break;
            }
            auto bound_order_expr = order_binder->BuildExpression(*order_desc->expr, bind_context_ptr);
            bound_select_node->order_by_expressions_.emplace_back(bound_order_expr);
        }
    }

    // 13. LIMIT
    if(statement.limit !=nullptr) {
        auto limit_binder = MakeShared<LimitBinder>(query_context);
        bound_select_node->limit_expression_ = limit_binder->BuildExpression(*statement.limit->limit, bind_context_ptr);

        if(statement.limit->offset != nullptr) {
            bound_select_node->offset_expression_ = limit_binder->BuildExpression(*statement.limit->offset, bind_context_ptr);
        }
    }

    // 14. TOP
    // 15. UNION/INTERSECT/EXCEPT
    // 16. LIMIT
    // 17. ORDER BY
    // 18. TOP

    return bound_select_node;
}

PlanBuildingContext
PlanBuilder::BuildShow(SharedPtr<QueryContext>& query_context,
                       const hsql::ShowStatement &statement,
                       SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kShowTables : {
            return BuildShowTables(query_context, statement, bind_context_ptr);
        }
        case hsql::kShowColumns : {
            return BuildShowColumns(query_context, statement, bind_context_ptr);
        }
        default:
            PlannerError("Don't support show type.");
    }
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildShowColumns(SharedPtr<QueryContext>& query_context,
                              const hsql::ShowStatement &statement,
                              SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Show columns isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildShowTables(SharedPtr<QueryContext>& query_context,
                             const hsql::ShowStatement &statement,
                             SharedPtr<BindContext>& bind_context_ptr) {
    SharedPtr<LogicalNode> logical_chunk_scan =
            MakeShared<LogicalChunkScan>(ChunkScanType::kShowTables, bind_context_ptr);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_chunk_scan, bind_context_ptr);
    PlanBuildingContext res;
    res.plan = logical_chunk_scan;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildImport(SharedPtr<QueryContext>& query_context,
                         const hsql::ImportStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildImportCsv(query_context, statement, bind_context_ptr);
        case hsql::kImportTbl:
            return BuildImportTbl(query_context, statement, bind_context_ptr);
        case hsql::kImportBinary:
            return BuildImportBinary(query_context, statement, bind_context_ptr);
        case hsql::kImportAuto:
            return BuildImportAuto(query_context, statement, bind_context_ptr);
    }
    PlannerError("Invalid import type.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildImportCsv(SharedPtr<QueryContext>& query_context,
                            const hsql::ImportStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Import from CSV file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildImportTbl(SharedPtr<QueryContext>& query_context,
                            const hsql::ImportStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Import from other table file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildImportBinary(SharedPtr<QueryContext>& query_context,
                               const hsql::ImportStatement &statement,
                               SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Import from binary file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildImportAuto(SharedPtr<QueryContext>& query_context,
                             const hsql::ImportStatement &statement,
                             SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Import from automatically identified format isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExport(SharedPtr<QueryContext>& query_context,
                         const hsql::ExportStatement &statement,
                         SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildExportCsv(query_context, statement, bind_context_ptr);
        case hsql::kImportTbl:
            return BuildExportTbl(query_context, statement, bind_context_ptr);
        case hsql::kImportBinary:
            return BuildExportBinary(query_context, statement, bind_context_ptr);
        case hsql::kImportAuto:
            return BuildExportAuto(query_context, statement, bind_context_ptr);
    }
    PlannerError("Invalid export type.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExportCsv(SharedPtr<QueryContext>& query_context,
                            const hsql::ExportStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to CSV file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExportTbl(SharedPtr<QueryContext>& query_context,
                            const hsql::ExportStatement &statement,
                            SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to table file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExportBinary(SharedPtr<QueryContext>& query_context,
                               const hsql::ExportStatement &statement,
                               SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to binary file isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExportAuto(SharedPtr<QueryContext>& query_context,
                             const hsql::ExportStatement &statement,
                             SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting in auto mode isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildTransaction(SharedPtr<QueryContext>& query_context,
                              const hsql::TransactionStatement &statement,
                              SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(query_context, statement, bind_context_ptr);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(query_context, statement, bind_context_ptr);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(query_context, statement, bind_context_ptr);
    }
    PlannerError("Invalid transaction command");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildTransactionBegin(SharedPtr<QueryContext>& query_context,
                                   const hsql::TransactionStatement &statement,
                                   SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: BEGIN isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildTransactionCommit(SharedPtr<QueryContext>& query_context,
                                    const hsql::TransactionStatement &statement,
                                    SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: COMMIT isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildTransactionRollback(SharedPtr<QueryContext>& query_context,
                                      const hsql::TransactionStatement &statement,
                                      SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: ROLLBACK isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildAlter(SharedPtr<QueryContext>& query_context,
                        const hsql::AlterStatement &statement,
                        SharedPtr<BindContext>& bind_context_ptr) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(query_context, statement, bind_context_ptr);
    }
    PlannerError("Invalid ALTER command");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildAlterDropColumn(SharedPtr<QueryContext>& query_context,
                                  const hsql::AlterStatement &statement,
                                  SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Alter: DROP COLUMN isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildPrepare(SharedPtr<QueryContext>& query_context,
                          const hsql::PrepareStatement &statement,
                          SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Prepare statement isn't supported.");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildExecute(SharedPtr<QueryContext>& query_context,
                          const hsql::ExecuteStatement &statement,
                          SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("Execute statement isn't supported.");
    PlanBuildingContext res;
    return res;
}

SharedPtr<TableRef>
PlanBuilder::BuildFromClause(SharedPtr<QueryContext>& query_context,
                             const hsql::TableRef* from_table,
                             SharedPtr<BindContext>& bind_context_ptr) {
    switch(from_table->type) {
        case hsql::kTableName: {
            // Only one table: select * from t1;
            return BuildTable(query_context, from_table, bind_context_ptr);
        }
        case hsql::kTableSelect: {
            // select t1.a from (select * from t2 as t1);
            String name = from_table->name != nullptr ? from_table->name : String();
            return BuildSubquery(query_context, name, *(from_table->select), bind_context_ptr);
        }
        case hsql::kTableJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            return BuildJoin(query_context, from_table, bind_context_ptr);
        }
        case hsql::kTableCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            return BuildCrossProduct(query_context, from_table, bind_context_ptr);
        }

        // TODO: No case currently, since parser doesn't support it.
//        case hsql::kExpressionList: {
//            break;
//        }
    }

    PlannerError("BuildFromClause is not implemented");
}

std::vector<SelectItem>
PlanBuilder::BuildSelectList(SharedPtr<QueryContext>& query_context,
                             const std::vector<hsql::Expr *> &select_list,
                             SharedPtr<BindContext> &bind_context_ptr) {
    std::vector<SelectItem> unfold_select_list;
    unfold_select_list.reserve(select_list.size());
    for(const hsql::Expr* select_expr: select_list) {
        switch (select_expr->type) {
            case hsql::kExprStar: {
                SharedPtr<Binding> binding;
                String table_name;
                if (select_expr->table == nullptr) {
                    // select * from t1;
                    PlannerAssert(!bind_context_ptr->table_names_.empty(), "No table is bound.");

                    // Use first table as the default table: select * from t1, t2; means select t1.* from t1, t2;
                    table_name = bind_context_ptr->table_names_[0];
                    binding = bind_context_ptr->binding_by_name_[table_name];
                } else {
                    // select t1.* from t1;
                    if (bind_context_ptr->binding_by_name_.contains(table_name)) {
                        binding = bind_context_ptr->binding_by_name_[table_name];
                    } else {
                        PlannerAssert(!bind_context_ptr->table_names_.empty(),
                                      "Table: '" + table_name + "' not found in select list.");
                    }
                }

                size_t column_count = binding->table_ptr_->ColumnCount();

                // Reserve more data in select list
                unfold_select_list.reserve(select_list.size() + column_count);

                // Build select list
                SharedPtr<String> table_name_ptr = MakeShared<String>(table_name);
                for(size_t idx = 0; idx < column_count; ++ idx) {
                    String column_name = binding->table_ptr_->GetColumnNameById(idx);
                    SharedPtr<ColumnExpression> bound_column_expr
                            = MakeShared<ColumnExpression>(binding->table_ptr_->GetColumnTypeById(idx),
                                                           table_name,
                                                           column_name,
                                                           idx,
                                                           0);
                    bound_column_expr->source_position_
                            = SourcePosition(bind_context_ptr->binding_context_id_, ExprSourceType::kBinding);
                    bound_column_expr->source_position_.binding_name_ = binding->table_name_;
                    unfold_select_list.emplace_back(bound_column_expr);

                    // Add the output heading of this bind context
                    bind_context_ptr->heading_.emplace_back(column_name);
                }

                break;
            }
            default: {
                unfold_select_list.emplace_back(select_expr);
            }
        }
    }
    return unfold_select_list;
}

SharedPtr<LogicalFilter>
PlanBuilder::BuildFilter(SharedPtr<QueryContext>& query_context,
                         const hsql::Expr* whereClause,
                         const SharedPtr<BindAliasProxy>& bind_alias_proxy,
                         SharedPtr<BindContext>& bind_context_ptr) {

    // SharedPtr<WhereBinder> where_binder
    auto where_binder = MakeShared<WhereBinder>(query_context, bind_alias_proxy);
    SharedPtr<BaseExpression> where_expr =
            where_binder->BuildExpression(*whereClause, bind_context_ptr);

    SplitExpressionByDelimiter(where_expr, ConjunctionType::kAnd);

    SharedPtr<LogicalFilter> logical_filter = MakeShared<LogicalFilter>(where_expr, bind_context_ptr);
    return logical_filter;
}

void
PlanBuilder::BuildGroupByHaving(SharedPtr<QueryContext>& query_context,
                                const hsql::SelectStatement& select,
                                const SharedPtr<BindAliasProxy>& bind_alias_proxy,
                                SharedPtr<BindContext>& bind_context_ptr,
                                SharedPtr<BoundSelectNode>& root_operator) {

    if(select.groupBy != nullptr) {
        // Start to bind GROUP BY clause
        // Set group binder
        auto group_binder = MakeShared<GroupBinder>(query_context, bind_alias_proxy);

        // Reserve the group names used in GroupBinder::BuildExpression
        bind_context_ptr->group_names_.reserve(select.groupBy->columns->size());
        for (const hsql::Expr* expr: *select.groupBy->columns) {

            // Call GroupBinder BuildExpression
            SharedPtr<BaseExpression> group_by_expr = group_binder->BuildExpression(*expr, bind_context_ptr);
            root_operator->group_by_expressions_.emplace_back(group_by_expr);

        }
    }

    // All having expr must appear in group by list or aggregate function list.
    if(select.groupBy != nullptr && select.groupBy->having != nullptr) {
        // Start to bind Having clause
        // Set having binder
        auto having_binder = MakeShared<HavingBinder>(query_context, bind_alias_proxy);
        for (const hsql::Expr* expr: *select.groupBy->having->exprList) {

            // Call HavingBinder BuildExpression
            SharedPtr<BaseExpression> having_expr = having_binder->BuildExpression(*expr, bind_context_ptr);
            root_operator->having_expressions_.emplace_back(having_expr);

        }
    }
}

PlanBuildingContext
PlanBuilder::BuildOrderBy(SharedPtr<QueryContext>& query_context,
                          const std::vector<hsql::OrderDescription*>& order_by_clause,
                          SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("BuildOrderBy is not implemented");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildLimit(SharedPtr<QueryContext>& query_context,
                        const hsql::LimitDescription& limit_description,
                        SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("BuildLimit is not implemented");
    PlanBuildingContext res;
    return res;
}

PlanBuildingContext
PlanBuilder::BuildTop(SharedPtr<QueryContext>& query_context,
                      const std::vector<hsql::OrderDescription*>& order_by_clause,
                      const hsql::LimitDescription& limit_description,
                      SharedPtr<BindContext>& bind_context_ptr) {
    PlannerError("BuildTop is not implemented");
    PlanBuildingContext res;
    return res;
}

SharedPtr<TableRef>
PlanBuilder::BuildDummyTable(SharedPtr<QueryContext>& query_context,
                SharedPtr<BindContext> &bind_context_ptr) {
    return MakeShared<DummyTableRef>();
}

SharedPtr<TableRef>
PlanBuilder::BuildTable(SharedPtr<QueryContext>& query_context,
                        const hsql::TableRef* from_table,
                        SharedPtr<BindContext>& bind_context_ptr) {
    // There are five cases here:
    // CTE*, which is subquery (may include correlated expression).
    // Recursive CTE (not supported by parser.)
    // View*, which is also a subquery (may include correlated expression).
    // Table*
    // External Table (not supported by parser.)

    // In AST, name is the alias of CTE and name of table
    // In AST, alias is the alias of table

    String name = from_table->name;
    String schema_name = from_table->schema == nullptr ? "Default" : String(from_table->schema);

    // Try to get CTE info from the cte container Query builder saved CTEs into before.
    auto cte = bind_context_ptr->GetCTE(name);
    if(cte != nullptr) {
        return BuildCTE(query_context, name, cte, bind_context_ptr);
    }

    // Base Table
    SharedPtr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, name);
    if(table_ptr != nullptr) {
        const String& table_name = name;

        // TODO: Handle table and column alias
//        if(from_table->alias != nullptr) {
//            logical_table_scan->table_alias_ = from_table->alias->name;
//            logical_table_scan->column_aliases_.reserve((from_table->alias->columns)->size());
//            for(const char* column_alias: *(from_table->alias->columns)) {
//                logical_table_scan->column_aliases_.emplace_back(column_alias);
//            }
//        }
        size_t column_count = table_ptr->ColumnCount();
        std::vector<DataType> types;
        std::vector<String> names;
        types.reserve(column_count);
        names.reserve(column_count);
        for(size_t idx = 0; idx < column_count; ++ idx) {
            types.emplace_back(table_ptr->GetColumnTypeById(idx));
            names.emplace_back(table_ptr->GetColumnNameById(idx));
        }

        String alias = from_table->getName();

        auto table_ref = MakeShared<BaseTableRef>(table_ptr, alias, names, types);

        // Insert the table in the binding context
        bind_context_ptr->AddTableBinding(table_name, table_ptr, types, names);

        return table_ref;
    }

    // View
    SharedPtr<View> view_ptr = Infinity::instance().catalog()->GetViewByName(schema_name, name);
    if(view_ptr != nullptr) {
        // Build view statement
        const String& view_name = name;
        BuildView(query_context, view_name, view_ptr, bind_context_ptr);
    }

    PlannerError("Table: " + name +" not found.");
}

SharedPtr<TableRef>
PlanBuilder::BuildSubquery(SharedPtr<QueryContext>& query_context,
                           const String &name,
                           const hsql::SelectStatement& select_stmt,
                           SharedPtr<BindContext>& bind_context_ptr) {
    // Create new bind context and add into context array;
    SharedPtr<BindContext> subquery_bind_context_ptr = MakeShared<BindContext>(bind_context_ptr);

//    this->AddBindContextArray(cte_bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = PlanBuilder::BuildSelect(query_context, select_stmt, subquery_bind_context_ptr);
    bind_context_ptr->AddSubQueryChild(subquery_bind_context_ptr);

    // Get the table index of the sub query output
    int64_t table_index = bound_select_node_ptr->GetTableIndex();

    String binding_name = name.empty() ? "subquery" + std::to_string(table_index) : name;
    // Add binding into bind context
    bind_context_ptr->AddSubqueryBinding(binding_name, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // Use binding name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_select_node_ptr, binding_name);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

SharedPtr<TableRef>
PlanBuilder::BuildCTE(SharedPtr<QueryContext>& query_context,
                      const String &name,
                      const SharedPtr<CommonTableExpressionInfo>& cte,
                      SharedPtr<BindContext>& bind_context_ptr) {
    // Table is from CTE
    if(bind_context_ptr->IsCTEBound(cte)) {
        // The CTE is bound before.
        PlannerError("CTE can only be bound only once");
    }

    // Build CTE(subquery)
    // insert into CTE Bound in bind context
    bind_context_ptr->BoundCTE(cte);

    // Create new bind context and add into context array;
    SharedPtr<BindContext> cte_bind_context_ptr = MakeShared<BindContext>(bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = PlanBuilder::BuildSelect(query_context, *cte->select_statement_, cte_bind_context_ptr);
    bind_context_ptr->AddSubQueryChild(cte_bind_context_ptr);

    // Add binding into bind context
    bind_context_ptr->AddSubqueryBinding(name, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // Use CTE name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_select_node_ptr, name);

    // TODO: Not care about the correlated expression

    return subquery_table_ref_ptr;
}

SharedPtr<TableRef>
PlanBuilder::BuildView(SharedPtr<QueryContext>& query_context,
                       const String &view_name,
                       const SharedPtr<View>& view_ptr,
                       SharedPtr<BindContext>& bind_context_ptr) {
    // Build view scan operator
    PlannerAssert(!(bind_context_ptr->IsViewBound(view_name)), "View: " + view_name + " is bound before!");
    bind_context_ptr->BoundView(view_name);

    const hsql::SQLStatement* sql_statement = view_ptr->GetSQLStatement();
    PlannerAssert(sql_statement->type() != hsql::StatementType::kStmtSelect, "View related statement isn't a select statement.");

    auto* select_stmt_ptr = static_cast<const hsql::SelectStatement*>(sql_statement);

    // Create new bind context and add into context array;
    SharedPtr<BindContext> view_bind_context_ptr = MakeShared<BindContext>(bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = PlanBuilder::BuildSelect(query_context, *select_stmt_ptr, view_bind_context_ptr);
    bind_context_ptr->AddSubQueryChild(view_bind_context_ptr);

    // Add binding into bind context
    bind_context_ptr->AddViewBinding(view_name, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // Use view name as the subquery table reference name
    auto subquery_table_ref_ptr = MakeShared<SubqueryTableRef>(bound_select_node_ptr, view_name);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

SharedPtr<TableRef>
PlanBuilder::BuildCrossProduct(SharedPtr<QueryContext>& query_context,
                               const hsql::TableRef* from_table,
                               SharedPtr<BindContext>& bind_context_ptr) {
    // Create left and right bind context
    // Build left and right table with new bind context
    // Merge two bind context into their parent context

    std::vector<SharedPtr<BindContext>> stack;
    SharedPtr<BindContext> left_context_ptr = bind_context_ptr;
    stack.emplace_back(left_context_ptr);

    SharedPtr<BindContext> cross_product_bind_context_ptr = nullptr;
    const std::vector<hsql::TableRef*>& tables = *from_table->list;
    auto table_count = tables.size();
    for(auto i = 0; i < table_count - 1; ++ i) {
        auto right_context_ptr = MakeShared<BindContext>(left_context_ptr);
        stack.emplace_back(right_context_ptr);

        cross_product_bind_context_ptr = MakeShared<BindContext>(left_context_ptr);
        left_context_ptr = cross_product_bind_context_ptr;
        stack.emplace_back(left_context_ptr);
    }

    left_context_ptr = stack.back();
    stack.pop_back();
    hsql::TableRef* left_table_ptr = tables[0];
    SharedPtr<TableRef> left_bound_table_ref = BuildFromClause(query_context, left_table_ptr, left_context_ptr);

    for(auto i = 1; i < table_count; ++ i) {
        // Right node
        auto right_context_ptr = stack.back();
        stack.pop_back();
        hsql::TableRef* right_table_ptr = tables[i];
        SharedPtr<TableRef> right_bound_table_ref = BuildFromClause(query_context, right_table_ptr, right_context_ptr);

        // Cross product node with dummy name
        auto cross_product_table_ref = MakeShared<CrossProductTableRef>(String());
        cross_product_table_ref->left_bind_context_ = left_context_ptr;
        cross_product_table_ref->left_table_ref_ = left_bound_table_ref;
        cross_product_table_ref->right_bind_context_ = right_context_ptr;
        cross_product_table_ref->right_table_ref_ = right_bound_table_ref;

        cross_product_bind_context_ptr = stack.back();
        stack.pop_back();
        cross_product_bind_context_ptr->AddLeftChild(left_context_ptr);
        cross_product_bind_context_ptr->AddRightChild(right_context_ptr);

        left_context_ptr = cross_product_bind_context_ptr;
        left_bound_table_ref = cross_product_table_ref;
    }

    bind_context_ptr = left_context_ptr;
    return left_bound_table_ref;
}

SharedPtr<TableRef>
PlanBuilder::BuildJoin(SharedPtr<QueryContext>& query_context,
                       const hsql::TableRef *from_table,
                       SharedPtr<BindContext> &bind_context_ptr) {
    //
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
    SharedPtr<BindContext> left_bind_context_ptr = MakeShared<BindContext>(bind_context_ptr);
    // AddBindContextArray(left_bind_context_ptr);
    // SharedPtr<TableRef> left_bound_table_ref
    auto left_bound_table_ref = BuildFromClause(query_context, from_table->join->left, left_bind_context_ptr);
    bind_context_ptr->AddLeftChild(left_bind_context_ptr);

    // Build right child
    SharedPtr<BindContext> right_bind_context_ptr = MakeShared<BindContext>(bind_context_ptr);
    // AddBindContextArray(right_bind_context_ptr);
    // SharedPtr<TableRef> right_bound_table_ref
    auto right_bound_table_ref = BuildFromClause(query_context, from_table->join->right, right_bind_context_ptr);
    bind_context_ptr->AddRightChild(right_bind_context_ptr);

    result->left_table_ref_ = left_bound_table_ref;
    result->left_bind_context_ = left_bind_context_ptr;

    result->right_table_ref_ = right_bound_table_ref;
    result->right_bind_context_ = right_bind_context_ptr;

    // Current parser doesn't support On using column syntax, so only consider the case of natural join.
    if(result->join_type_ == JoinType::kNatural) {
        std::unordered_set<String> left_binding_column_names;
        // TODO: Is there any way to get all_column_names size ? Collect all left binding columns numbers at very beginning?
        for(auto& left_binding_pair: left_bind_context_ptr->binding_by_name_) {
            for(auto& left_column_name: left_binding_pair.second->column_names_) {
                left_binding_column_names.emplace(left_column_name);
            }
        }

        std::vector<String> using_column_names;
        // TODO: column count of left binding tables and right binding tables is using_column_names size
        for(auto& right_binding_pair: right_bind_context_ptr->binding_by_name_) {
            for(auto& right_column_name: right_binding_pair.second->column_names_) {
                if(left_binding_column_names.contains(right_column_name)) {
                    using_column_names.emplace_back(right_column_name);
                }
            }
        }

        if(using_column_names.empty()) {
            // It is cross product, but not a natural join with dummy name
            auto cross_product_table_ref = MakeShared<CrossProductTableRef>(String());
            cross_product_table_ref->left_bind_context_ = left_bind_context_ptr;
            cross_product_table_ref->left_table_ref_ = left_bound_table_ref;

            cross_product_table_ref->right_bind_context_ = right_bind_context_ptr;
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
                PlannerAssert(left_bind_context_ptr->binding_names_by_column_.contains(column_name), "Column: " + column_name + " doesn't exist in left table");

                auto& left_column_binding_names = left_bind_context_ptr->binding_names_by_column_[column_name];

                PlannerAssert(left_column_binding_names.size() == 1, "Ambiguous column name: " + column_name + " in left table");

                auto& left_binding_name = left_column_binding_names[0];
                auto& left_binding_ptr = left_bind_context_ptr->binding_by_name_[left_binding_name];
                auto left_column_index = left_binding_ptr->name2index_[column_name];
                auto left_column_type = left_binding_ptr->column_types_[left_column_index];

                SharedPtr<ColumnExpression> left_column_expression_ptr =
                        MakeShared<ColumnExpression>(left_column_type, left_binding_ptr->table_name_, column_name, left_column_index, 0);

                PlannerAssert(right_bind_context_ptr->binding_names_by_column_.contains(column_name), "Column: " + column_name + " doesn't exist in right table");

                auto& right_column_binding_names = right_bind_context_ptr->binding_names_by_column_[column_name];

                PlannerAssert(right_column_binding_names.size() == 1, "Ambiguous column name: " + column_name + " in right table");


                auto& right_binding_name = right_column_binding_names[0];
                auto& right_binding_ptr = right_bind_context_ptr->binding_by_name_[left_binding_name];
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

    // Inner / Full / Left / Right join
    // TODO: Bind all join condition with where expression binder.

    // SharedPtr<JoinBinder> join_binder
    auto join_binder = MakeShared<JoinBinder>(query_context);

    // SharedPtr<BaseExpression> on_condition_ptr
    auto on_condition_ptr = join_binder->BuildExpression(*from_table->join->condition, bind_context_ptr);
//    SharedPtr<BaseExpression> where_expr =
//            bind_context_ptr->binder_->BuildExpression(*whereClause, bind_context_ptr);
//
//    SharedPtr<BaseExpression> condition_ptr =
//    from_table->join->condition
//    for(hsql::Expr* expr: from_table->join->condition) {
//
//    }
    result->on_conditions_ = SplitExpressionByDelimiter(on_condition_ptr, ConjunctionType::kAnd);

    return result;
}


}
