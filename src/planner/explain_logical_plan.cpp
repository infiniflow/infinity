//
// Created by jinhai on 23-3-14.
//

#include "explain_logical_plan.h"
#include "expression/aggregate_expression.h"
#include "expression/cast_expression.h"
#include "expression/case_expression.h"
#include "expression/function_expression.h"
#include "expression/between_expression.h"
#include "expression/subquery_expression.h"
#include "expression/value_expression.h"
#include "expression/in_expression.h"

namespace infinity {

void
ExplainLogicalPlan::Explain(const LogicalNode *statement,
                            SharedPtr<Vector<SharedPtr<String>>> &result,
                            i64 intent_size) {
    switch(statement->operator_type()) {
        case LogicalNodeType::kAggregate: {
            Explain((LogicalAggregate*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion:
            break;
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin: {
            Explain((LogicalJoin*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            Explain((LogicalCrossProduct*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kLimit: {
            Explain((LogicalLimit*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFilter: {
            Explain((LogicalFilter*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kProjection: {
            Explain((LogicalProject*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kSort: {
            Explain((LogicalSort*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDelete:
            break;
        case LogicalNodeType::kUpdate:
            break;
        case LogicalNodeType::kInsert: {
            Explain((LogicalInsert*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kImport:
            break;
        case LogicalNodeType::kExport:
            break;
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable: {
            Explain((LogicalCreateTable*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            Explain((LogicalCreateCollection*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            Explain((LogicalCreateSchema*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateView:
            break;
        case LogicalNodeType::kDropTable: {
            Explain((LogicalDropTable*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            Explain((LogicalDropCollection*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            Explain((LogicalDropSchema*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropView:
            break;
        case LogicalNodeType::kShow: {
            Explain((LogicalShow*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kTableScan:{
            Explain((LogicalTableScan*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kViewScan:
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kExplain:
            break;
        case LogicalNodeType::kPrepare:
            break;
        default: {
            PlannerError("Unexpect logical node type");
        }
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCreateSchema* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE SCHEMA: ";
    } else {
        create_str = "CREATE SCHEMA: ";
    }

    create_str += *create_node->schema_name()  + "conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));
}

void
ExplainLogicalPlan::Explain(const LogicalCreateTable* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE TABLE: ";
    } else {
        create_str = "CREATE TABLE: ";
    }

    create_str += *create_node->schema_name() + "."
               + create_node->table_definitions()->table_name()
               + " conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));

    SizeT column_count = create_node->table_definitions()->column_count();
    if(column_count == 0) {
        PlannerError("No columns in the table");
    }
    const Vector<SharedPtr<ColumnDef>>& columns = create_node->table_definitions()->columns();

    intent_size += 2;
    String columns_str = String(intent_size, ' ') + "Columns: ";
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        columns_str += columns[idx]->ToString() + ", ";
    }
    columns_str += columns.back()->ToString();
    result->emplace_back(MakeShared<String>(columns_str));
}

void
ExplainLogicalPlan::Explain(const LogicalCreateCollection* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE COLLECTION: ";
    } else {
        create_str = "CREATE COLLECTION: ";
    }

    create_str += *create_node->schema_name() + "." + *create_node->collection_name()
                + " conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropSchema* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP SCHEMA: ";
    } else {
        drop_str = "DROP SCHEMA: ";
    }

    drop_str += *drop_node->schema_name()  + "conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropTable* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP TABLE: ";
    } else {
        drop_str = "DROP TABLE: ";
    }

    drop_str += *drop_node->schema_name() + "." + *drop_node->table_name()
             + " conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

void
ExplainLogicalPlan::Explain(const LogicalDropCollection* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String drop_str;
    if(intent_size != 0) {
        drop_str = String(intent_size - 2, ' ') + "-> DROP COLLECTION: ";
    } else {
        drop_str = "DROP COLLECTION: ";
    }

    drop_str += *drop_node->schema_name() + "." + *drop_node->collection_name()
                + " conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
    result->emplace_back(MakeShared<String>(drop_str));
}

void
ExplainLogicalPlan::Explain(const LogicalInsert* insert_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String insert_str;
    if(intent_size != 0) {
        insert_str = String(intent_size - 2, ' ') + "-> INSERT: ";
    } else {
        insert_str = "INSERT: ";
    }

    insert_str += insert_node->table_ptr()->TableName() + "(";
    SizeT value_count = insert_node->value_list().size();
    if(value_count == 0) {
        PlannerError("No value list in insert statement");
    }
    for(SizeT idx = 0; idx < value_count - 1; ++ idx) {
        auto& value_expr = insert_node->value_list()[idx];
        insert_str += value_expr->Name() + ", ";
    }
    insert_str += insert_node->value_list().back()->Name();
    result->emplace_back(MakeShared<String>(insert_str));
    if(insert_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(insert_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalProject* project_node,
                    SharedPtr<Vector<SharedPtr<String>>>& result,
                    i64 intent_size) {
    String project_header;
    if(intent_size != 0) {
        project_header = String(intent_size - 2, ' ') + "-> PROJECT: ";
    } else {
        project_header = "PROJECT: ";
    }

    project_header += "(" + std::to_string(project_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(project_header));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(project_node->table_index_);
    result->emplace_back(MakeShared<String>(table_index));

    // Expressions
    String expression_str = String(intent_size, ' ') + " - expressions: [";
    SizeT expr_count = project_node->expressions_.size();
    if(expr_count == 0) {
        PlannerError("No expression list in projection node.");
    }
    for(SizeT idx = 0; idx < expr_count - 1; ++ idx) {
        Explain(project_node->expressions_[idx].get(), expression_str);
        expression_str += ", ";
    }
    Explain(project_node->expressions_.back().get(), expression_str);
    expression_str += "]";

    result->emplace_back(MakeShared<String>(expression_str));
    if(project_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(project_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalFilter* filter_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String filter_node_header;
    if(intent_size != 0) {
        filter_node_header = String(intent_size - 2, ' ') + "-> FILTER: ";
    } else {
        filter_node_header = "FILTER: ";
    }

    filter_node_header += "(" + std::to_string(filter_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(filter_node_header));

    // filter expression
    String filter_str = String(intent_size, ' ') + " - filter: ";
    Explain(filter_node->expression().get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    if(filter_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(filter_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalTableScan* table_scan_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String table_scan_header;
    if(intent_size != 0) {
        table_scan_header = String(intent_size - 2, ' ') + "-> TABLE SCAN: ";
    } else {
        table_scan_header = "TABLE SCAN: ";
    }

    table_scan_header += "(" + std::to_string(table_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + table_scan_node->table_alias_ + "(";
    table_name += table_scan_node->table_ptr()->SchemaName() + ".";
    table_name += table_scan_node->table_ptr()->TableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(table_scan_node->table_index_);
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output_columns: [";
    SizeT column_count = table_scan_node->column_names_.size();
    if(column_count == 0) {
        PlannerError(fmt::format("No column in table: {}.", table_scan_node->table_alias_));
    }
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        output_columns += table_scan_node->column_names_[idx] + ", ";
    }
    output_columns += table_scan_node->column_names_.back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if(table_scan_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(table_scan_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalAggregate* aggregate_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if(groups_count == 0 && aggregate_node == 0) {
        PlannerError("Both groups and aggregates are empty.")
    }

    String agg_header;
    if(intent_size != 0) {
        agg_header = String(intent_size - 2, ' ') + "-> AGGREGATE: ";
    } else {
        agg_header = "AGGREGATE: ";
    }

    agg_header += "(" + std::to_string(aggregate_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(agg_header));

    // Aggregate Table index
    String aggregate_table_index = String(intent_size, ' ') + " - aggregate table index: #"
                                 + std::to_string(aggregate_node->aggregate_index_);
    result->emplace_back(MakeShared<String>(aggregate_table_index));

    // Aggregate expressions
    String aggregate_expression_str = String(intent_size, ' ') + " - aggregate: [";
    if(aggregates_count != 0) {
        for(SizeT idx = 0; idx < aggregates_count - 1; ++ idx) {
            Explain(aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
            aggregate_expression_str += ", ";
        }
        Explain(aggregate_node->aggregates_.back().get(), aggregate_expression_str);
    }
    aggregate_expression_str += "]";
    result->emplace_back(MakeShared<String>(aggregate_expression_str));

    // Group by expressions

    if(groups_count != 0) {
        // Group by table index
        String group_table_index = String(intent_size, ' ') + " - group by table index: #"
                                       + std::to_string(aggregate_node->groupby_index_);
        result->emplace_back(MakeShared<String>(group_table_index));

        String group_by_expression_str = String(intent_size, ' ') + " - group by: [";
        for(SizeT idx = 0; idx < groups_count - 1; ++ idx) {
            Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            aggregate_expression_str += ", ";
        }
        Explain(aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(MakeShared<String>(group_by_expression_str));
    }

    if(aggregate_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(aggregate_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalSort* sort_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    String sort_header;
    if(intent_size != 0) {
        sort_header = String(intent_size - 2, ' ') + "-> ORDER BY: ";
    } else {
        sort_header = "ORDER BY: ";
    }

    sort_header += "(" + std::to_string(sort_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(sort_header));

    String sort_expression_str = String(intent_size, ' ') + " - expressions: [";
    SizeT order_by_count = sort_node->expressions_.size();
    if(order_by_count == 0) {
        PlannerError("ORDER BY without any expression.")
    }

    for(SizeT idx = 0; idx < order_by_count - 1; ++ idx) {
        Explain(sort_node->expressions_[idx].get(), sort_expression_str);
        sort_expression_str += " " + ToString(sort_node->order_by_types_[idx]) + ", ";
    }
    Explain(sort_node->expressions_.back().get(), sort_expression_str);
    sort_expression_str += " " + ToString(sort_node->order_by_types_.back());
    result->emplace_back(MakeShared<String>(sort_expression_str));

    if(sort_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(sort_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalLimit* limit_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    String limit_header;
    if(intent_size != 0) {
        limit_header = String(intent_size - 2, ' ') + "-> LIMIT: ";
    } else {
        limit_header = "LIMIT: ";
    }

    limit_header += "(" + std::to_string(limit_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(limit_header));

    String limit_value_str = String(intent_size, ' ') + " - limit: ";
    Explain(limit_node->limit_expression_.get(), limit_value_str);
    result->emplace_back(MakeShared<String>(limit_value_str));

    if(limit_node->offset_expression_ != 0) {
        String offset_value_str = String(intent_size, ' ') + " - offset: ";
        Explain(limit_node->offset_expression_.get(), offset_value_str);
        result->emplace_back(MakeShared<String>(offset_value_str));
    }

    if(limit_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(limit_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCrossProduct* cross_product_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String cross_product_header;
    if(intent_size != 0) {
        cross_product_header = String(intent_size - 2, ' ') + "-> CROSS PRODUCT: ";
    } else {
        cross_product_header = "CROSS PRODUCT: ";
    }
    cross_product_header += "(" + std::to_string(cross_product_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(cross_product_header));

    // Table index
    // TODO: cross product shouldn't generate new table. It is to be deleted.
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(cross_product_node->table_index_);
    result->emplace_back(MakeShared<String>(table_index));

    intent_size += 2;
    if(cross_product_node->left_node() != nullptr) {
        ExplainLogicalPlan::Explain(cross_product_node->left_node().get(), result, intent_size);
    }

    if(cross_product_node->right_node() != nullptr) {
        ExplainLogicalPlan::Explain(cross_product_node->right_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalJoin* join_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String join_header;
    if(intent_size != 0) {
        join_header = String(intent_size - 2, ' ') + "-> ";
    }
    join_header += ToString(join_node->join_type_);
    join_header += "(" + std::to_string(join_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(join_header));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(join_node->table_index_);
    result->emplace_back(MakeShared<String>(table_index));

    // Conditions
    String condition_str = String(intent_size, ' ') + " - filters: [";

    SizeT conditions_count = join_node->conditions_.size();
    if(conditions_count == 0) {
        PlannerError("JOIN without any condition.")
    }

    for(SizeT idx = 0; idx < conditions_count - 1; ++ idx) {
        Explain(join_node->conditions_[idx].get(), condition_str);
        condition_str += ", ";
    }
    Explain(join_node->conditions_.back().get(), condition_str);
    result->emplace_back(MakeShared<String>(condition_str));

    intent_size += 2;
    if(join_node->left_node() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->left_node().get(), result, intent_size);
    }

    if(join_node->right_node() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->right_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalShow* show_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String show_str;
    if(intent_size != 0) {
        show_str = String(intent_size - 2, ' ') + "-> CHUNK SCAN: ";
    } else {
        show_str = "CHUNK SCAN: ";
    }
    show_str += ToString(show_node->scan_type());
    result->emplace_back(MakeShared<String>(show_str));

    if(show_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(show_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const BaseExpression* base_expression, String& expr_str) {
    switch(base_expression->type()) {
        case ExpressionType::kAggregate: {
            AggregateExpression* aggregate_expression = (AggregateExpression*)base_expression;
            PlannerAssert(aggregate_expression->arguments().size() == 1, "More than one argument in aggregate function")
            expr_str += aggregate_expression->aggregate_function_.name() + "(";
            Explain(aggregate_expression->arguments()[0].get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kCast: {
            CastExpression* cast_expression = (CastExpression*)base_expression;
            PlannerAssert(cast_expression->arguments().size() == 1, "More than one argument in cast function")
            expr_str += "CAST(";
            Explain(cast_expression->arguments()[0].get(), expr_str);
            expr_str += " AS " + cast_expression->Type().ToString() + ")";
            break;
        }
        case ExpressionType::kCase: {
            CaseExpression* case_expression = (CaseExpression*)base_expression;

            expr_str += "CASE ";
            for(auto& check: case_expression->CaseExpr()) {
                expr_str += " WHEN(";
                Explain(check.when_expr_.get(), expr_str);
                expr_str += ")";

                expr_str += " THEN(";
                Explain(check.then_expr_.get(), expr_str);
                expr_str += ")";
            }

            expr_str += " ELSE";
            Explain(case_expression->ElseExpr().get(), expr_str);
            expr_str += " END";
            break;
        }
        case ExpressionType::kColumn: {
            ColumnExpression* column_expression = (ColumnExpression*)base_expression;
            expr_str += column_expression->Name() + " (#"
                     + std::to_string(column_expression->binding().table_idx) + "."
                     + std::to_string(column_expression->binding().column_idx) + ")";
            break;
        }
        case ExpressionType::kFunction: {
            FunctionExpression* function_expression = (FunctionExpression*)base_expression;
            if(function_expression->arguments().empty()) {
                // No argument function
                expr_str += function_expression->func_.name() + "()";
                break;
            }

            if(function_expression->arguments().size() == 1) {
                // Unary function
                expr_str += function_expression->func_.name() + "(";
                Explain(function_expression->arguments()[0].get(), expr_str);
                expr_str += ")";
                break;
            }
            if(function_expression->arguments().size() == 2) {
                // Binary function
                Explain(function_expression->arguments()[0].get(), expr_str);
                expr_str += " " + function_expression->func_.name() + " ";
                Explain(function_expression->arguments()[1].get(), expr_str);
                break;
            }

            // More than two arguments function
            expr_str += function_expression->func_.name() + "(";
            SizeT argument_count = function_expression->arguments().size();
            for(SizeT idx = 0; idx < argument_count - 1; ++ idx) {
                Explain(function_expression->arguments()[idx].get(), expr_str);
                expr_str += ", ";
            }
            Explain(function_expression->arguments().back().get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kBetween: {
            BetweenExpression* between_expression = (BetweenExpression*)base_expression;
            PlannerAssert(between_expression->arguments().size() == 3, "Between expression should have three arguments.")
            Explain(between_expression->arguments()[0].get(), expr_str);
            expr_str += " BETWEEN ";
            Explain(between_expression->arguments()[1].get(), expr_str);
            expr_str += " AND ";
            Explain(between_expression->arguments()[2].get(), expr_str);
            break;
        }
        case ExpressionType::kIn: {
            InExpression* in_expression = (InExpression*)base_expression;
            expr_str += "IN[";
            SizeT argument_count = in_expression->arguments().size();
            for(SizeT idx = 0; idx < argument_count - 1; ++ idx) {

                Explain(in_expression->arguments()[idx].get(), expr_str);
            }
            Explain(in_expression->arguments().back().get(), expr_str);
            expr_str += "]";
            break;
        }
        case ExpressionType::kValue: {
            ValueExpression* value_expression = (ValueExpression*)base_expression;
            expr_str += value_expression->ToString();
            break;
        }
        case ExpressionType::kSubQuery:
        case ExpressionType::kCorrelatedColumn:
        case ExpressionType::kReference:
        default: {
            PlannerError("Unsupported expression type")
        }
    }
}

}
