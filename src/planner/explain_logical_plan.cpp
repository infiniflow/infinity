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
#include "expression/reference_expression.h"

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
        case LogicalNodeType::kImport: {
            Explain((LogicalImport*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExport: {
            Explain((LogicalExport*)statement, result, intent_size);
            break;
        }
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
        case LogicalNodeType::kCreateView: {
            Explain((LogicalCreateView*)statement, result, intent_size);
            break;
        }
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
        case LogicalNodeType::kDropView: {
            Explain((LogicalDropView*)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFlush: {
            Explain((LogicalFlush*)statement, result, intent_size);
            break;
        }
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
            PlannerError("Unexpected logical node type");
        }
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCreateSchema* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
ExplainLogicalPlan::Explain(const LogicalCreateTable* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
                                *create_node->table_definitions()->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->table_definitions()->column_count();
        if(column_count == 0) {
            PlannerError("No columns in the table");
        }
        const Vector<SharedPtr<ColumnDef>>& columns = create_node->table_definitions()->columns();

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
ExplainLogicalPlan::Explain(const LogicalCreateCollection* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
ExplainLogicalPlan::Explain(const LogicalCreateView* create_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
        SizeT column_count = create_node->names_ptr()->size();
        if(column_count == 0) {
            PlannerError("No columns in the table");
        }
        String columns_str = String(intent_size, ' ') + " - columns: [";
        for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
            columns_str += create_node->names_ptr()->at(idx) + " " + create_node->types_ptr()->at(idx)->ToString() + ", ";
        }
        columns_str += create_node->names_ptr()->back() + " " + create_node->types_ptr()->back()->ToString() + "]";
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

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainLogicalPlan::Explain(const LogicalDropSchema* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
ExplainLogicalPlan::Explain(const LogicalDropTable* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
ExplainLogicalPlan::Explain(const LogicalDropCollection* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
ExplainLogicalPlan::Explain(const LogicalDropView* drop_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void
ExplainLogicalPlan::Explain(const LogicalInsert* insert_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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

    if(insert_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(insert_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalProject* project_node,
                    SharedPtr<Vector<SharedPtr<String>>>& result,
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
                String(intent_size, ' ') + " - table index: #" + std::to_string(project_node->table_index_);
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
            Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(MakeShared<String>(expression_str));
    }

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
        filter_node_header = String(intent_size - 2, ' ') + "-> FILTER ";
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += "(" + std::to_string(filter_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(filter_node_header));

    // filter expression
    String filter_str = String(intent_size, ' ') + " - filter: ";
    Explain(filter_node->expression().get(), filter_str);
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
        table_scan_header = String(intent_size - 2, ' ') + "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += "(" + std::to_string(table_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + table_scan_node->TableAlias() + "(";

    DBEntry* db_entry = TableCollectionEntry::GetDBEntry(table_scan_node->table_collection_ptr());

    table_name += *db_entry->db_name_+ ".";
    table_name += *table_scan_node->table_collection_ptr()->table_collection_name_ + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(table_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = table_scan_node->GetOutputNames()->size();
    if(column_count == 0) {
        PlannerError(fmt::format("No column in table: {}.", table_scan_node->TableAlias()));
    }
    for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
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
                                       + std::to_string(aggregate_node->aggregate_index_);
        result->emplace_back(MakeShared<String>(aggregate_table_index));
    }

    // Aggregate expressions
    {
        String aggregate_expression_str = String(intent_size, ' ') + " - aggregate: [";
        if (aggregates_count != 0) {
            for (SizeT idx = 0; idx < aggregates_count - 1; ++idx) {
                Explain(aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            Explain(aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(MakeShared<String>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        String group_table_index = String(intent_size, ' ') + " - group by table index: #"
                                   + std::to_string(aggregate_node->groupby_index_);
        result->emplace_back(MakeShared<String>(group_table_index));

        String group_by_expression_str = String(intent_size, ' ') + " - group by: [";
        for (SizeT idx = 0; idx < groups_count - 1; ++idx) {
            Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
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
            Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " " + ToString(sort_node->order_by_types_[idx]) + ", ";
        }
        Explain(sort_node->expressions_.back().get(), sort_expression_str);
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

    if(sort_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(sort_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalLimit* limit_node,
        SharedPtr<Vector<SharedPtr<String>>>& result,
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
        Explain(limit_node->limit_expression_.get(), limit_value_str);
        result->emplace_back(MakeShared<String>(limit_value_str));
    }

    if(limit_node->offset_expression_ != 0) {
        String offset_value_str = String(intent_size, ' ') + " - offset: ";
        Explain(limit_node->offset_expression_.get(), offset_value_str);
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

    if(limit_node->left_node() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(limit_node->left_node().get(), result, intent_size);
    }
}

void
ExplainLogicalPlan::Explain(const LogicalCrossProduct* cross_product_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
    {
        String join_header;
        if (intent_size != 0) {
            join_header = String(intent_size - 2, ' ') + "-> ";
        }
        join_header += ToString(join_node->join_type_);
        join_header += "(" + std::to_string(join_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(join_header));
    }

    // Conditions
    {
        String condition_str = String(intent_size, ' ') + " - filters: [";

        SizeT conditions_count = join_node->conditions_.size();
        if (conditions_count == 0) {
            PlannerError("JOIN without any condition.")
        }

        for (SizeT idx = 0; idx < conditions_count - 1; ++idx) {
            Explain(join_node->conditions_[idx].get(), condition_str);
            condition_str += ", ";
        }
        Explain(join_node->conditions_.back().get(), condition_str);
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
                    " - output columns: [database, table, type, column_count, row_count, segment_count, segment_capacity]";
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

            String show_column_schema_str = String(intent_size, ' ') + " - schema: ";
            show_column_schema_str += show_node->schema_name();
            result->emplace_back(MakeShared<String>(show_column_schema_str));

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
                expr_str += ", ";
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
        case ExpressionType::kReference: {
            ReferenceExpression* reference_expression = (ReferenceExpression*)base_expression;
            expr_str += reference_expression->Name() + " (#"
                        + std::to_string(reference_expression->column_index()) + ")";
            break;
        }
        case ExpressionType::kSubQuery:
        case ExpressionType::kCorrelatedColumn:

        default: {
            PlannerError("Unsupported expression type")
        }
    }
}

void
ExplainLogicalPlan::Explain(const LogicalImport* import_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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
        DBEntry* db_entry = TableCollectionMeta::GetDBEntry(import_node->table_collection_entry()->table_collection_meta_);
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

    if(import_node->left_node() != nullptr or import_node->right_node() != nullptr) {
        PlannerError("Import node have children nodes.")
    }
}

void
ExplainLogicalPlan::Explain(const LogicalExport* export_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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

    if(export_node->left_node() != nullptr or export_node->right_node() != nullptr) {
        PlannerError("EXPORT node have children nodes.")
    }
}

void
ExplainLogicalPlan::Explain(const LogicalFlush* flush_node,
                            SharedPtr<Vector<SharedPtr<String>>>& result,
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

}
