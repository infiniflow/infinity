//
// Created by jinhai on 23-3-17.
//

#include "explain_physical_plan.h"
#include "planner/explain_logical_plan.h"
#include "executor/operator/physical_create_schema.h"
#include "executor/operator/physical_create_table.h"
#include "executor/operator/physical_create_collection.h"
#include "executor/operator/physical_drop_schema.h"
#include "executor/operator/physical_drop_table.h"
#include "executor/operator/physical_drop_collection.h"
#include "executor/operator/physical_insert.h"
#include "executor/operator/physical_project.h"
#include "executor/operator/physical_filter.h"
#include "executor/operator/physical_table_scan.h"
#include "executor/operator/physical_aggregate.h"
#include "executor/operator/physical_sort.h"
#include "executor/operator/physical_limit.h"
#include "executor/operator/physical_cross_product.h"
#include "executor/operator/physical_nested_loop_join.h"
#include "executor/operator/physical_show.h"
#include "executor/operator/physical_union_all.h"
#include "executor/operator/physical_index_scan.h"
#include "executor/operator/physical_dummy_scan.h"
#include "executor/operator/physical_hash_join.h"
#include "executor/operator/physical_sort_merge_join.h"
#include "executor/operator/physical_index_join.h"
#include "executor/operator/physical_top.h"
#include "executor/operator/physical_delete.h"
#include "executor/operator/physical_update.h"
#include "executor/operator/physical_import.h"
#include "executor/operator/physical_export.h"
#include "executor/operator/physical_alter.h"
#include "executor/operator/physical_create_view.h"
#include "executor/operator/physcial_drop_view.h"
#include "executor/operator/physical_flush.h"
#include "executor/operator/physical_source.h"


namespace infinity {

void
ExplainPhysicalPlan::Explain(const PhysicalOperator *op,
                            SharedPtr<Vector<SharedPtr<String>>> &result,
                            bool is_recursive,
                            i64 intent_size) {
    switch(op->operator_type()) {
        case PhysicalOperatorType::kAggregate: {
            Explain((PhysicalAggregate*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kUnionAll: {
            Explain((PhysicalUnionAll*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kShow: {
            Explain((PhysicalShow*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            Explain((PhysicalTableScan*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kFilter: {
            Explain((PhysicalFilter*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            Explain((PhysicalIndexScan*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDummyScan: {
            Explain((PhysicalDummyScan*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinHash: {
            Explain((PhysicalHashJoin*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinNestedLoop: {
            Explain((PhysicalNestedLoopJoin*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinMerge: {
            Explain((PhysicalSortMergeJoin*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinIndex: {
            Explain((PhysicalIndexJoin*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kCrossProduct: {
            Explain((PhysicalCrossProduct*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kLimit: {
            Explain((PhysicalLimit*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kTop: {
            Explain((PhysicalTop*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kProjection: {
            Explain((PhysicalProject*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kSort: {
            Explain((PhysicalSort*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDelete: {
            Explain((PhysicalDelete*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kUpdate: {
            Explain((PhysicalUpdate*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kInsert: {
            Explain((PhysicalInsert*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kImport: {
            Explain((PhysicalImport*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kExport: {
            Explain((PhysicalExport*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kAlter: {
            Explain((PhysicalAlter*)op, result, is_recursive,intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateTable: {
            Explain((PhysicalCreateTable*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            Explain((PhysicalCreateCollection*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateDatabase: {
            Explain((PhysicalCreateSchema*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            Explain((PhysicalCreateView*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            Explain((PhysicalDropTable*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            Explain((PhysicalDropCollection*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropDatabase: {
            Explain((PhysicalDropSchema*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropView: {
            Explain((PhysicalDropView*)op, result, is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kExplain: {
            break;
        }
        case PhysicalOperatorType::kPreparedPlan: {
            break;
        }
        case PhysicalOperatorType::kFlush: {
            Explain((PhysicalFlush*)op, result,is_recursive, intent_size);
            break;
        }
        case PhysicalOperatorType::kSource: {
            break;
        }
        case PhysicalOperatorType::kSink: {
            break;
        }
        case PhysicalOperatorType::kInvalid: {
            break;
        }
        case PhysicalOperatorType::kParallelAggregate: {
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate: {
            break;
        }
        case PhysicalOperatorType::kIntersect: {
            break;
        }
        case PhysicalOperatorType::kExcept: {
            break;
        }
        case PhysicalOperatorType::kHash: {
            break;
        }
        case PhysicalOperatorType::kMergeHash: {
            break;
        }
        case PhysicalOperatorType::kMergeLimit: {
            break;
        }
        case PhysicalOperatorType::kMergeTop: {
            break;
        }
        case PhysicalOperatorType::kMergeSort: {
            break;
        }
        default: {
            PlannerError("Unexpected logical node type");
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateSchema* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = String(intent_size - 2, ' ') + "-> CREATE SCHEMA ";
        } else {
            create_header_str = "CREATE SCHEMA ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *create_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateTable* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = String(intent_size - 2, ' ') + "-> CREATE TABLE ";
        } else {
            create_header_str = "CREATE TABLE ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *create_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = String(intent_size, ' ') + " - table name: " +
                                *create_node->table_definition()->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->table_definition()->column_count();
        if(column_count == 0) {
            PlannerError("No columns in the table");
        }
        const Vector<SharedPtr<ColumnDef>>& columns = create_node->table_definition()->columns();

        String columns_str = String(intent_size, ' ') + " - columns: [";
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            columns_str += columns[idx]->ToString() + ", ";
        }
        columns_str += columns.back()->ToString() + "]";
        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateCollection* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = String(intent_size - 2, ' ') + "-> CREATE COLLECTION ";
        } else {
            create_header_str = "CREATE COLLECTION ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *create_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String collection_name_str = String(intent_size, ' ') + " - collection name: " + *create_node->collection_name();
        result->emplace_back(MakeShared<String>(collection_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalDropSchema* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = String(intent_size - 2, ' ') + "-> DROP SCHEMA ";
        } else {
            drop_header_str = "DROP SCHEMA ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *drop_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalDropTable* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = String(intent_size - 2, ' ') + "-> DROP TABLE ";
        } else {
            drop_header_str = "DROP TABLE ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *drop_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = String(intent_size, ' ') + " - table name: " + *drop_node->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalDropCollection* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = String(intent_size - 2, ' ') + "-> DROP COLLECTION ";
        } else {
            drop_header_str = "DROP COLLECTION ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *drop_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String table_name_str = String(intent_size, ' ') + " - collection name: " + *drop_node->collection_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalInsert* insert_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String insert_header_str;
        if (intent_size != 0) {
            insert_header_str = String(intent_size - 2, ' ') + "-> INSERT ";
        } else {
            insert_header_str = "INSERT ";
        }

        insert_header_str += "(" + std::to_string(insert_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(insert_header_str));
    }

    // Schema name
    {
        DBEntry* db_entry = TableCollectionMeta::GetDBEntry(insert_node->table_collection_entry()->table_collection_meta_);
        String schema_name_str = String(intent_size, ' ')
                + " - schema name: "
                + *db_entry->db_name_;
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = String(intent_size, ' ')
                + " - table name: "
                + *insert_node->table_collection_entry()->table_collection_name_;
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Values
    {
        String insert_str;
        insert_str = " - values (";
        SizeT value_count = insert_node->value_list().size();
        if(value_count == 0) {
            PlannerError("No value list in insert statement");
        }
        for(SizeT idx = 0; idx < value_count - 1; ++ idx) {
            auto& value_expr = insert_node->value_list()[idx];
            insert_str += value_expr->Name() + ", ";
        }
        insert_str += insert_node->value_list().back()->Name() + ")";
        result->emplace_back(MakeShared<String>(insert_str));
    }

    if(insert_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(insert_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalProject* project_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String project_header;
        if (intent_size != 0) {
            project_header = String(intent_size - 2, ' ') + "-> PROJECT ";
        } else {
            project_header = "PROJECT ";
        }

        project_header += "(" + std::to_string(project_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(project_header));
    }

    // Table index
    {
        String table_index =
                String(intent_size, ' ') + " - table index: #" + std::to_string(project_node->TableIndex());
        result->emplace_back(MakeShared<String>(table_index));
    }

    // Expressions
    {
        String expression_str = String(intent_size, ' ') + " - expressions: [";
        SizeT expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            PlannerError("No expression list in projection node.");
        }
        for (SizeT idx = 0; idx < expr_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(MakeShared<String>(expression_str));
    }

    if(project_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(project_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalFilter* filter_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    String filter_node_header;
    if(intent_size != 0) {
        filter_node_header = String(intent_size - 2, ' ') + "-> FILTER ";
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += "(" + std::to_string(filter_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(filter_node_header));

    // filter expression
    String filter_str = String(intent_size, ' ') + " - filter: ";
    ExplainLogicalPlan::Explain(filter_node->condition().get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = filter_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if(filter_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(filter_node->left().get(), result, is_recursive, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalTableScan* table_scan_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    String table_scan_header;
    if(intent_size != 0) {
        table_scan_header = String(intent_size - 2, ' ') + "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += "(" + std::to_string(table_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    DBEntry* db_entry = TableCollectionEntry::GetDBEntry(table_scan_node->TableEntry());

    String table_name = String(intent_size, ' ') + " - table name: " + table_scan_node->table_alias() + "(";
    table_name += *db_entry->db_name_ + ".";
    table_name += *table_scan_node->TableEntry()->table_collection_name_ + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(table_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output_columns: [";
    SizeT column_count = table_scan_node->GetOutputNames()->size();
    if(column_count == 0) {
        PlannerError(fmt::format("No column in table: {}.", table_scan_node->TableIndex()));
    }
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if(table_scan_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(table_scan_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalAggregate* aggregate_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if(groups_count == 0 && aggregate_node == 0) {
        PlannerError("Both groups and aggregates are empty.")
    }

    {
        String agg_header;
        if (intent_size != 0) {
            agg_header = String(intent_size - 2, ' ') + "-> AGGREGATE ";
        } else {
            agg_header = "AGGREGATE ";
        }

        agg_header += "(" + std::to_string(aggregate_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(agg_header));
    }

    // Aggregate Table index
    {
        String aggregate_table_index = String(intent_size, ' ') + " - aggregate table index: #"
                                       + std::to_string(aggregate_node->AggregateTableIndex());
        result->emplace_back(MakeShared<String>(aggregate_table_index));
    }

    // Aggregate expressions
    {
        String aggregate_expression_str = String(intent_size, ' ') + " - aggregate: [";
        if (aggregates_count != 0) {
            for (SizeT idx = 0; idx < aggregates_count - 1; ++idx) {
                ExplainLogicalPlan::Explain(aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            ExplainLogicalPlan::Explain(aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(MakeShared<String>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        String group_table_index = String(intent_size, ' ') + " - group by table index: #"
                                   + std::to_string(aggregate_node->GroupTableIndex());
        result->emplace_back(MakeShared<String>(group_table_index));

        String group_by_expression_str = String(intent_size, ' ') + " - group by: [";
        for (SizeT idx = 0; idx < groups_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(MakeShared<String>(group_by_expression_str));
    }

    if(aggregate_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(aggregate_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalSort* sort_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String sort_header;
        if (intent_size != 0) {
            sort_header = String(intent_size - 2, ' ') + "-> SORT ";
        } else {
            sort_header = "SORT ";
        }

        sort_header += "(" + std::to_string(sort_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(sort_header));
    }

    {
        String sort_expression_str = String(intent_size, ' ') + " - expressions: [";
        SizeT order_by_count = sort_node->expressions_.size();
        if (order_by_count == 0) {
            PlannerError("ORDER BY without any expression.")
        }

        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " " + ToString(sort_node->order_by_types_[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " " + ToString(sort_node->order_by_types_.back()) + "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = sort_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if(sort_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(sort_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalLimit* limit_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String limit_header;
        if (intent_size != 0) {
            limit_header = String(intent_size - 2, ' ') + "-> LIMIT ";
        } else {
            limit_header = "LIMIT ";
        }

        limit_header += "(" + std::to_string(limit_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(limit_header));
    }

    {
        String limit_value_str = String(intent_size, ' ') + " - limit: ";
        ExplainLogicalPlan::Explain(limit_node->limit_expr().get(), limit_value_str);
        result->emplace_back(MakeShared<String>(limit_value_str));
    }

    if(limit_node->offset_expr() != 0) {
        String offset_value_str = String(intent_size, ' ') + " - offset: ";
        ExplainLogicalPlan::Explain(limit_node->offset_expr().get(), offset_value_str);
        result->emplace_back(MakeShared<String>(offset_value_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = limit_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if(limit_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(limit_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalCrossProduct* cross_product_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    {
        String cross_product_header;
        if (intent_size != 0) {
            cross_product_header = String(intent_size - 2, ' ') + "-> CROSS PRODUCT ";
        } else {
            cross_product_header = "CROSS PRODUCT ";
        }
        cross_product_header += "(" + std::to_string(cross_product_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(cross_product_header));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = cross_product_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    intent_size += 2;
    if(cross_product_node->left() != nullptr) {
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(cross_product_node->left().get(), result, false, intent_size);
        }
    }

    if(cross_product_node->right() != nullptr) {
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(cross_product_node->right().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalNestedLoopJoin* join_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    String join_header;
    if(intent_size != 0) {
        join_header = String(intent_size - 2, ' ') + "-> NESTED LOOP JOIN";
    } else {
        join_header = "NESTED LOOP JOIN ";
    }

    join_header += "(" + std::to_string(join_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(join_header));

    // Conditions
    {
        String condition_str = String(intent_size, ' ') + " - filters: [";

        SizeT conditions_count = join_node->conditions().size();
        if (conditions_count == 0) {
            PlannerError("JOIN without any condition.")
        }

        for (SizeT idx = 0; idx < conditions_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(join_node->conditions()[idx].get(), condition_str);
            condition_str += ", ";
        }
        ExplainLogicalPlan::Explain(join_node->conditions().back().get(), condition_str);
        result->emplace_back(MakeShared<String>(condition_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = join_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    intent_size += 2;
    if(join_node->left() != nullptr) {
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(join_node->left().get(), result, false, intent_size);
        }
    }

    if(join_node->right() != nullptr) {
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(join_node->right().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalShow* show_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
                            bool is_recursive,
                            i64 intent_size) {
    switch(show_node->scan_type()) {
        case ShowType::kShowTables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ') + "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ') +
                                        " - output columns: [schema, table, type, column_count, row_count, block_count, block_size]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowViews: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ') + "-> SHOW VIEWS ";
            } else {
                show_str = "SHOW VIEWS ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ') + " - output columns: [schema, view, column_count]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowColumn: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ') + "-> DESCRIBE TABLE/COLLECTION ";
            } else {
                show_str = "DESCRIBE TABLE/COLLECTION ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_column_db_str = String(intent_size, ' ') + " - schema: ";
            show_column_db_str += show_node->db_name();
            result->emplace_back(MakeShared<String>(show_column_db_str));

            String show_column_table_str = String(intent_size, ' ') + " - table/collection: ";
            show_column_table_str += show_node->object_name();
            result->emplace_back(MakeShared<String>(show_column_table_str));

            String output_columns_str = String(intent_size, ' ') + " - output columns: [column_name, column_type, constraint]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        default: {
            PlannerError("Invalid show type")
        }
    }

    if(show_node->left() != nullptr) {
        intent_size += 2;
        if (is_recursive) {
            return;
        } else {
            ExplainPhysicalPlan::Explain(show_node->left().get(), result, false, intent_size);
        }
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalUnionAll* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalIndexScan* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalDummyScan* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalHashJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalSortMergeJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalIndexJoin* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalTop* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalDelete* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalUpdate* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalImport* import_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    {
        String import_header_str;
        if (intent_size != 0) {
            import_header_str = String(intent_size - 2, ' ') + "-> IMPORT ";
        } else {
            import_header_str = "IMPORT ";
        }

        import_header_str += "(" + std::to_string(import_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(import_header_str));
    }

    {
        DBEntry* db_entry
            = TableCollectionMeta::GetDBEntry(import_node->table_collection_entry()->table_collection_meta_);

        SharedPtr<String> schema_name
                = MakeShared<String>(String(intent_size, ' ')
                        + " - schema name: "
                        + *db_entry->db_name_);
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name
                = MakeShared<String>(String(intent_size, ' ')
                        + " - table name: "
                        + *import_node->table_collection_entry()->table_collection_name_);
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path
                = MakeShared<String>(String(intent_size, ' ') + " - file: " + import_node->file_path());
        result->emplace_back(path);
    }

    switch(import_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            SharedPtr<String> header
                    = MakeShared<String>(String(intent_size, ' ') + " - header: " + (import_node->header() ? "Yes": "No"));
            result->emplace_back(header);

            SharedPtr<String> delimiter
                    = MakeShared<String>(String(intent_size, ' ') + " - delimiter: " + import_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);
            break;
        }
    }

    if(import_node->left() != nullptr or import_node->right() != nullptr) {
        PlannerError("Import node have children nodes.")
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalExport* export_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    {
        String export_header_str;
        if (intent_size != 0) {
            export_header_str = String(intent_size - 2, ' ') + "-> EXPORT ";
        } else {
            export_header_str = "EXPORT ";
        }

        export_header_str += "(" + std::to_string(export_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(export_header_str));
    }

    {
        SharedPtr<String> schema_name
                = MakeShared<String>(String(intent_size, ' ') + " - schema name: " + export_node->schema_name());
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name
                = MakeShared<String>(String(intent_size, ' ') + " - table name: " + export_node->table_name());
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path
                = MakeShared<String>(String(intent_size, ' ') + " - file: " + export_node->file_path());
        result->emplace_back(path);
    }

    switch(export_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            SharedPtr<String> header
                    = MakeShared<String>(String(intent_size, ' ') + " - header: " + (export_node->header() ? "Yes": "No"));
            result->emplace_back(header);

            SharedPtr<String> delimiter
                    = MakeShared<String>(String(intent_size, ' ') + " - delimiter: " + export_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);
            break;
        }
    }

    if(export_node->left() != nullptr or export_node->right() != nullptr) {
        PlannerError("EXPORT node have children nodes.")
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalAlter* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    NotImplementError("Not implemented");
}

void
ExplainPhysicalPlan::Explain(const PhysicalCreateView* create_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = String(intent_size - 2, ' ') + "-> CREATE VIEW ";
        } else {
            create_header_str = "CREATE VIEW ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: "
                                 + create_node->create_view_info()->schema_name_;
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String view_name_str = String(intent_size, ' ') + " - view name: "
                               + create_node->create_view_info()->view_name_;
        result->emplace_back(MakeShared<String>(view_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->GetOutputNames()->size();
        if(column_count == 0) {
            PlannerError("No columns in the table");
        }
        String columns_str = String(intent_size, ' ') + " - columns: [";
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            columns_str += create_node->GetOutputNames()->at(idx) + " " + create_node->GetOutputTypes()->at(idx)->ToString() + ", ";
        }
        columns_str += create_node->GetOutputNames()->back() + " " + create_node->GetOutputTypes()->back()->ToString() + "]";
        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: "
                + ConflictTypeToStr(create_node->create_view_info()->conflict_type_);
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Text
    {
        String sql_text = String(intent_size, ' ') + " - text: Not implemented";
        result->emplace_back(MakeShared<String>(sql_text));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalDropView* drop_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = String(intent_size - 2, ' ') + "-> DROP VIEW ";
        } else {
            drop_header_str = "DROP VIEW ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *drop_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String table_name_str = String(intent_size, ' ') + " - view name: " + *drop_node->view_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str =
                String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }
}

void
ExplainPhysicalPlan::Explain(const PhysicalFlush* flush_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    String flush_header_str;
    if (intent_size != 0) {
        flush_header_str = String(intent_size - 2, ' ') + "-> FLUSH ";
    } else {
        flush_header_str = "FLUSH ";
    }

    switch(flush_node->flush_type()) {
        case FlushType::kData:
            flush_header_str += "DATA (" + std::to_string(flush_node->node_id()) + ")";
            break;
        case FlushType::kLog:
            flush_header_str += "LOG (" + std::to_string(flush_node->node_id()) + ")";
            break;
        case FlushType::kBuffer:
            flush_header_str += "BUFFER (" + std::to_string(flush_node->node_id()) + ")";
            break;
    }

    result->emplace_back(MakeShared<String>(flush_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalSource* source_node,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             bool is_recursive,
                             i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> SOURCE ";
    } else {
        explain_header_str = "SOURCE ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalSink* flush_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> SINK ";
    } else {
        explain_header_str = "SINK ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalParallelAggregate* parallel_aggregate_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "PARALLEL AGGREGATE ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalMergeParallelAggregate* merge_parallel_aggregate_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "MERGE PARALLEL AGGREGATE ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalIntersect* intersect_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> INTERSECT ";
    } else {
        explain_header_str = "INTERSECT ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalExcept* except_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> EXCEPT ";
    } else {
        explain_header_str = "EXCEPT ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalHash* hash_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> HASH ";
    } else {
        explain_header_str = "HASH ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalMergeHash* merge_hash_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE HASH ";
    } else {
        explain_header_str = "MERGE HASH ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalMergeLimit* merge_limit_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE LIMIT ";
    } else {
        explain_header_str = "MERGE LIMIT ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalMergeTop* merge_top_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE TOP ";
    } else {
        explain_header_str = "MERGE TOP ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void
ExplainPhysicalPlan::Explain(const PhysicalMergeSort* merge_sort_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
        bool is_recursive,
        i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE SORT ";
    } else {
        explain_header_str = "MERGE SORT ";
    }
    result->emplace_back(MakeShared<String>(explain_header_str));
}

}
