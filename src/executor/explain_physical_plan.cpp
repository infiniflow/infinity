//
// Created by jinhai on 23-3-17.
//

#include "explain_physical_plan.h"


namespace infinity {

void
ExplainPhysicalPlan::Explain(const PhysicalOperator *op,
                            SharedPtr<Vector<SharedPtr<String>>> &result,
                            i64 intent_size) {
    switch(op->operator_type()) {
        case PhysicalOperatorType::kAggregate: {
            Explain((PhysicalAggregate*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUnionAll: {
            Explain((PhysicalUnionAll*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kShow: {
            Explain((PhysicalShow*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            Explain((PhysicalTableScan*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kFilter: {
            Explain((PhysicalFilter*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            Explain((PhysicalIndexScan*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDummyScan: {
            Explain((PhysicalDummyScan*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinHash: {
            Explain((PhysicalHashJoin*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinNestedLoop: {
            Explain((PhysicalNestedLoopJoin*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinSortMerge: {
            Explain((PhysicalSortMergeJoin*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinIndex: {
            Explain((PhysicalIndexJoin*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kProduct: {
            Explain((PhysicalCrossProduct*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kLimit: {
            Explain((PhysicalLimit*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kTop: {
            Explain((PhysicalTop*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kProjection: {
            Explain((PhysicalProject*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kSort: {
            Explain((PhysicalSort*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDelete: {
            Explain((PhysicalDelete*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUpdate: {
            Explain((PhysicalUpdate*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kInsert: {
            Explain((PhysicalInsert*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kImport: {
            Explain((PhysicalImport*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kExport: {
            Explain((PhysicalExport*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kAlter: {
            Explain((PhysicalAlter*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateTable: {
            Explain((PhysicalCreateTable*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            Explain((PhysicalCreateCollection*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateSchema: {
            Explain((PhysicalCreateSchema*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            Explain((PhysicalCreateView*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            Explain((PhysicalDropTable*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            Explain((PhysicalDropCollection*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropSchema: {
            Explain((PhysicalDropSchema*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropView: {
            Explain((PhysicalDropView*)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kExplain: {
            break;
        }
        case PhysicalOperatorType::kPreparedPlan: {
            break;
        }
        default: {
            PlannerError("Unexpect logical node type");
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateSchema* create_node,
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
ExplainPhysicalPlan::Explain(const PhysicalCreateTable* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String create_str;
    if(intent_size != 0) {
        create_str = String(intent_size - 2, ' ') + "-> CREATE TABLE: ";
    } else {
        create_str = "CREATE TABLE: ";
    }

    create_str += create_node->table_definition()->schema_name() + "."
                  + create_node->table_definition()->table_name()
                  + " conflict type: " + ConflictTypeToStr(create_node->conflict_type());
    result->emplace_back(MakeShared<String>(create_str));

    SizeT column_count = create_node->table_definition()->column_count();
    if(column_count == 0) {
        PlannerError("No columns in the table");
    }
    const Vector<SharedPtr<ColumnDef>>& columns = create_node->table_definition()->columns();

    intent_size += 2;
    String columns_str = String(intent_size, ' ') + "Columns: ";
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        columns_str += columns[idx]->ToString() + ", ";
    }
    columns_str += columns.back()->ToString();
    result->emplace_back(MakeShared<String>(columns_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateCollection* create_node,
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
ExplainPhysicalPlan::Explain(const PhysicalDropSchema* drop_node,
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
ExplainPhysicalPlan::Explain(const PhysicalDropTable* drop_node,
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
ExplainPhysicalPlan::Explain(const PhysicalDropCollection* drop_node,
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
ExplainPhysicalPlan::Explain(const PhysicalInsert* insert_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String insert_str;
    if(intent_size != 0) {
        insert_str = String(intent_size - 2, ' ') + "-> INSERT: ";
    } else {
        insert_str = "INSERT: ";
    }

    insert_str += insert_node->table()->TableName() + "(";
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
    if(insert_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(insert_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalProject* project_node,
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
    if(project_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(project_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalFilter* filter_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String filter_str;
    if(intent_size != 0) {
        filter_str = String(intent_size - 2, ' ') + "-> FILTER: ";
    } else {
        filter_str = "FILTER: ";
    }
    filter_str += filter_node->condition()->ToString();
    result->emplace_back(MakeShared<String>(filter_str));
    if(filter_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(filter_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalTableScan* table_scan_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String table_scan_str;
    if(intent_size != 0) {
        table_scan_str = String(intent_size - 2, ' ') + "-> TABLE SCAN: ";
    } else {
        table_scan_str = "TABLE SCAN: ";
    }

    table_scan_str += table_scan_node->table_alias() + "(";
    SizeT column_count = table_scan_node->column_names().size();
    if(column_count == 0) {
        PlannerError(fmt::format("No column in table: {}.", table_scan_node->table_alias()));
    }
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        table_scan_str += table_scan_node->column_names()[idx] + ", ";
    }
    table_scan_str += table_scan_node->column_names().back() + ")";
    result->emplace_back(MakeShared<String>(table_scan_str));

    if(table_scan_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(table_scan_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalAggregate* aggregate_node,
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
    if(groups_count == 0 && aggregates_count == 0) {
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

    if(aggregate_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(aggregate_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalSort* sort_node,
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
    sort_str += sort_node->expressions_.back()->ToString();
    result->emplace_back(MakeShared<String>(sort_str));

    if(sort_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(sort_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalLimit* limit_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String limit_str;
    if(intent_size != 0) {
        limit_str = String(intent_size - 2, ' ') + "-> LIMIT: ";
    } else {
        limit_str = "LIMIT: ";
    }

    limit_str += limit_node->limit_expr()->ToString();
    if(limit_node->offset_expr() != 0) {
        limit_str += ", OFFSET: " + limit_node->offset_expr()->ToString();
    }

    result->emplace_back(MakeShared<String>(limit_str));

    if(limit_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(limit_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCrossProduct* cross_product_node,
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
    if(cross_product_node->left() != nullptr) {
        ExplainPhysicalPlan::Explain(cross_product_node->left().get(), result, intent_size);
    }

    if(cross_product_node->right() != nullptr) {
        ExplainPhysicalPlan::Explain(cross_product_node->right().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalNestedLoopJoin* join_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            i64 intent_size) {
    String join_str;
    if(intent_size != 0) {
        join_str = String(intent_size - 2, ' ') + "-> ";
    }

    SizeT conditions_count = join_node->conditions().size();
    if(conditions_count == 0) {
        PlannerError("JOIN without any condition.")
    }

    for(SizeT idx = 0; idx < conditions_count - 1; ++ idx) {
        join_str += join_node->conditions()[idx]->ToString() + ", ";
    }
    join_str += join_node->conditions().back()->ToString();
    result->emplace_back(MakeShared<String>(join_str));

    intent_size += 2;
    if(join_node->left() != nullptr) {
        ExplainPhysicalPlan::Explain(join_node->left().get(), result, intent_size);
    }

    if(join_node->right() != nullptr) {
        ExplainPhysicalPlan::Explain(join_node->right().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalShow* show_node,
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

    if(show_node->left() != nullptr) {
        intent_size += 2;
        ExplainPhysicalPlan::Explain(show_node->left().get(), result, intent_size);
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalUnionAll* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalIndexScan* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalDummyScan* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalHashJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalSortMergeJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalIndexJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalTop* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalDelete* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalUpdate* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalImport* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalExport* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalAlter* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateView* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalDropView* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

}
