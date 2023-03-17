//
// Created by jinhai on 23-3-14.
//

#include "explain_logical_plan.h"

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
        case LogicalNodeType::kChunkScan: {
            Explain((LogicalChunkScan*)statement, result, intent_size);
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
        insert_str += value_expr->ToString() + ", ";
    }
    insert_str += insert_node->value_list().back()->ToString();
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
    String project_str;
    if(intent_size != 0) {
        project_str = String(intent_size - 2, ' ') + "-> PROJECT: ";
    } else {
        project_str = "PROJECT: ";
    }

    SizeT expr_count = project_node->expressions_.size();
    if(expr_count == 0) {
        PlannerError("No expression list in projection node.");
    }
    for(SizeT idx = 0; idx < expr_count - 1; ++ idx) {
        project_str += project_node->expressions_[idx]->ToString() + ", ";
    }
    project_str += project_node->expressions_.back()->ToString();
    result->emplace_back(MakeShared<String>(project_str));
    if(project_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(project_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalFilter* filter_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String filter_str;
    if(intent_size != 0) {
        filter_str = String(intent_size - 2, ' ') + "-> FILTER: ";
    } else {
        filter_str = "FILTER: ";
    }
    filter_str += filter_node->expression()->ToString();
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
    String table_scan_str;
    if(intent_size != 0) {
        table_scan_str = String(intent_size - 2, ' ') + "-> TABLE SCAN: ";
    } else {
        table_scan_str = "TABLE SCAN: ";
    }

    table_scan_str += table_scan_node->table_alias_ + "(";
    SizeT column_count = table_scan_node->column_names_.size();
    if(column_count == 0) {
        PlannerError(fmt::format("No column in table: {}.", table_scan_node->table_alias_));
    }
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        table_scan_str += table_scan_node->column_names_[idx] + ", ";
    }
    table_scan_str += table_scan_node->column_names_.back() + ")";
    result->emplace_back(MakeShared<String>(table_scan_str));

    if(table_scan_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(table_scan_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalAggregate* aggregate_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    String agg_str;
    if(intent_size != 0) {
        agg_str = String(intent_size - 2, ' ') + "-> AGGREGATE: ";
    } else {
        agg_str = "AGGREGATE: ";
    }

    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if(groups_count == 0 && aggregate_node == 0) {
        PlannerError("Both groups and aggregates are empty.")
    }

    if(aggregates_count != 0) {
        for(SizeT idx = 0; idx < aggregates_count - 1; ++ idx) {
            agg_str += aggregate_node->aggregates_[idx]->ToString() + ", ";
        }
        agg_str += aggregate_node->aggregates_.back()->ToString();
        if(groups_count != 0) {
            agg_str += ", ";
        }
    }

    if(groups_count != 0) {
        agg_str += "GROUP BY: ";
        for(SizeT idx = 0; idx < groups_count - 1; ++ idx) {
            agg_str += aggregate_node->groups_[idx]->ToString() + ", ";
        }
        agg_str += aggregate_node->groups_.back()->ToString();
    }

    result->emplace_back(MakeShared<String>(agg_str));

    if(aggregate_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(aggregate_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalSort* sort_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    String sort_str;
    if(intent_size != 0) {
        sort_str = String(intent_size - 2, ' ') + "-> ORDER BY: ";
    } else {
        sort_str = "ORDER BY: ";
    }

    SizeT order_by_count = sort_node->expressions_.size();
    if(order_by_count == 0) {
        PlannerError("ORDER BY without any expression.")
    }

    for(SizeT idx = 0; idx < order_by_count - 1; ++ idx) {
        sort_str += sort_node->expressions_[idx]->ToString() + " " + ToString(sort_node->order_by_types_[idx]) + ", ";
    }
    result->emplace_back(MakeShared<String>(sort_str));

    if(sort_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(sort_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalLimit* limit_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        i64 intent_size) {
    String limit_str;
    if(intent_size != 0) {
        limit_str = String(intent_size - 2, ' ') + "-> LIMIT: ";
    } else {
        limit_str = "LIMIT: ";
    }

    limit_str += limit_node->limit_expression_->ToString();
    if(limit_node->offset_expression_ != 0) {
        limit_str += ", OFFSET: " + limit_node->offset_expression_->ToString();
    }

    result->emplace_back(MakeShared<String>(limit_str));

    if(limit_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(limit_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCrossProduct* cross_product_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String cross_product_str;
    if(intent_size != 0) {
        cross_product_str = String(intent_size - 2, ' ') + "-> CROSS PRODUCT: ";
    } else {
        cross_product_str = "CROSS PRODUCT: ";
    }
    result->emplace_back(MakeShared<String>(cross_product_str));

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
    String join_str;
    if(intent_size != 0) {
        join_str = String(intent_size - 2, ' ') + "-> ";
    }
    join_str += ToString(join_node->join_type_) + ": ";

    SizeT conditions_count = join_node->conditions_.size();
    if(conditions_count == 0) {
        PlannerError("JOIN without any condition.")
    }

    for(SizeT idx = 0; idx < conditions_count - 1; ++ idx) {
        join_str += join_node->conditions_[idx]->ToString() + ", ";
    }
    join_str += join_node->conditions_.back()->ToString();
    result->emplace_back(MakeShared<String>(join_str));

    intent_size += 2;
    if(join_node->left_node() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->left_node().get(), result, intent_size);
    }

    if(join_node->right_node() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->right_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalChunkScan* chunk_scan_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String chunk_scan_str;
    if(intent_size != 0) {
        chunk_scan_str = String(intent_size - 2, ' ') + "-> CHUNK SCAN: ";
    } else {
        chunk_scan_str = "CHUNK SCAN: ";
    }
    chunk_scan_str += ToString(chunk_scan_node->scan_type());
    result->emplace_back(MakeShared<String>(chunk_scan_str));

    if(chunk_scan_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(chunk_scan_node->left_node().get(), result, intent_size);
    }
}

}
