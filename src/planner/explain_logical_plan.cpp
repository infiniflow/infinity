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

import stl;
import logical_node;
import logical_node_type;
import logical_create_schema;
import logical_create_view;
import logical_create_table;
import logical_create_collection;
import logical_drop_schema;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_view;
import logical_delete;
import logical_update;
import logical_flush;
import logical_insert;
import logical_project;
import logical_filter;
import logical_table_scan;
import logical_knn_scan;
import logical_aggregate;
import logical_sort;
import logical_limit;
import logical_cross_product;
import logical_join;
import logical_show;
import logical_export;
import logical_import;
import logical_create_index;

import third_party;
import parser;
import db_entry;
import table_collection_entry;
import table_collection_meta;

import expression_type;
import knn_expression;
import aggregate_expression;
import cast_expression;
import case_expression;
import column_expression;
import function_expression;
import between_expression;
import in_expression;
import value_expression;
import reference_expression;

import infinity_exception;

module explain_logical_plan;

namespace infinity {

void ExplainLogicalPlan::Explain(const LogicalNode *statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    switch (statement->operator_type()) {
        case LogicalNodeType::kAggregate: {
            Explain((LogicalAggregate *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion:
            break;
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin: {
            Explain((LogicalJoin *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            Explain((LogicalCrossProduct *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kLimit: {
            Explain((LogicalLimit *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFilter: {
            Explain((LogicalFilter *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kProjection: {
            Explain((LogicalProject *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kSort: {
            Explain((LogicalSort *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDelete:
            Explain((LogicalDelete *)statement, result, intent_size);
            break;
        case LogicalNodeType::kUpdate:
            Explain((LogicalUpdate *)statement, result, intent_size);
            break;
        case LogicalNodeType::kInsert: {
            Explain((LogicalInsert *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kImport: {
            Explain((LogicalImport *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExport: {
            Explain((LogicalExport *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable: {
            Explain((LogicalCreateTable *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateIndex: {
            Explain((LogicalCreateIndex *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            Explain((LogicalCreateCollection *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            Explain((LogicalCreateSchema *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateView: {
            Explain((LogicalCreateView *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropTable: {
            Explain((LogicalDropTable *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            Explain((LogicalDropCollection *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            Explain((LogicalDropSchema *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropView: {
            Explain((LogicalDropView *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFlush: {
            Explain((LogicalFlush *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kShow: {
            Explain((LogicalShow *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kTableScan: {
            Explain((LogicalTableScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kKnnScan: {
            Explain((LogicalKnnScan *)statement, result, intent_size);
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
            Error<PlannerException>("Unexpected logical node type");
        }
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = Format("{}-> CREATE SCHEMA ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE SCHEMA ";
        }

        create_header_str += Format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = Format("{}-> CREATE TABLE ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE TABLE ";
        }

        create_header_str += Format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = Format("{} - table name: {}", String(intent_size, ' '), *create_node->table_definitions()->table_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->table_definitions()->column_count();
        if (column_count == 0) {
            Error<PlannerException>("No columns in the table");
        }
        const Vector<SharedPtr<ColumnDef>> &columns = create_node->table_definitions()->columns();

        String columns_str;
        columns_str.append(String(intent_size, ' ')).append(" - columns: [");
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            columns_str.append(columns[idx]->ToString()).append(", ");
        }
        columns_str.append(columns.back()->ToString()).append("]");
        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateIndex *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = String(intent_size - 2, ' ') + "-> CREATE INDEX ";
        } else {
            create_header_str = "CREATE INDEX ";
        }

        create_header_str += "(" + ToStr(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *create_node->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    {
        String table_name_str = String(intent_size, ' ') + " - table name: " + *create_node->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Index definition
    {
        String index_def_str = String(intent_size, ' ') + " - index definition: " + create_node->index_definition()->ToString();
        result->emplace_back(MakeShared<String>(index_def_str));
    }

    {
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictType2Str(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateCollection *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = Format("{}-> CREATE COLLECTION ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE COLLECTION ";
        }

        create_header_str += Format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String collection_name_str = Format("{} - collection name: {}", String(intent_size, ' '), *create_node->collection_name());
        result->emplace_back(MakeShared<String>(collection_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = Format("{}-> CREATE VIEW ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE VIEW ";
        }
        create_header_str += Format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), create_node->create_view_info()->schema_name_);
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String view_name_str = Format("{} - view name: {}", String(intent_size, ' '), create_node->create_view_info()->view_name_);
        result->emplace_back(MakeShared<String>(view_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->names_ptr()->size();
        if (column_count == 0) {
            Error<PlannerException>("No columns in the table");
        }
        String columns_str;
        columns_str.append(String(intent_size, ' ')).append(" - columns: [");
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            columns_str.append(create_node->names_ptr()->at(idx)).append(" ").append(create_node->types_ptr()->at(idx)->ToString()).append(", ");
        }
        columns_str.append(create_node->names_ptr()->back()).append(" ").append(create_node->types_ptr()->back()->ToString()).append("]");
        result->emplace_back(MakeShared<String>(columns_str));

        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str =
            Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(create_node->create_view_info()->conflict_type_));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Text
    {
        String sql_text = Format("{} - text: Not implemented", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(sql_text));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = Format("{}-> CREATE SCHEMA ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP SCHEMA ";
        }

        drop_header_str += Format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = Format("{}-> DROP TABLE ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP TABLE ";
        }

        drop_header_str += Format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = Format("{} - table: {}", String(intent_size, ' '), *drop_node->table_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = Format("{}-> DROP COLLECTION ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP COLLECTION ";
        }

        drop_header_str += Format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String table_name_str = Format("{} - collection: {}", String(intent_size, ' '), *drop_node->collection_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropView *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = Format("{}-> DROP VIEW ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP VIEW ";
        }

        drop_header_str += Format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String table_name_str = Format("{} - view name: {}", String(intent_size, ' '), *drop_node->view_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = Format("{} - conflict type: {}", String(intent_size, ' '), ConflictType2Str(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = Format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String insert_header_str;
        if (intent_size != 0) {
            insert_header_str = Format("{}-> INSERT ", String(intent_size - 2, ' '));
        } else {
            insert_header_str = "INSERT ";
        }

        insert_header_str += Format("({})", insert_node->node_id());
        result->emplace_back(MakeShared<String>(insert_header_str));
    }

    // Schema name
    {
        DBEntry *db_entry = TableCollectionMeta::GetDBEntry(insert_node->table_collection_entry()->table_collection_meta_);
        String schema_name_str = Format("{} - schema name: {}", String(intent_size, ' '), *db_entry->db_name_);

        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str =
            Format("{} - table name: {}", String(intent_size, ' '), *insert_node->table_collection_entry()->table_collection_name_);
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Values
    {
        String insert_str;
        insert_str = " - values ";
        SizeT value_count = insert_node->value_list().size();
        if (value_count == 0) {
            Error<PlannerException>("No value list in insert statement");
        }
        for (SizeT idx = 0; idx < value_count; ++idx) {
            if (idx != 0)
                insert_str += ", ";
            const Vector<SharedPtr<BaseExpression>> &value = insert_node->value_list()[idx];
            SizeT column_count = value.size();
            for (SizeT col = 0; col < column_count; ++col) {
                auto &value_expr = value[idx];
                if (col == 0)
                    insert_str += "(";
                else
                    insert_str += ", ";
                insert_str += value_expr->Name();
                if (col == column_count - 1)
                    insert_str += ")";
            }
        }

        result->emplace_back(MakeShared<String>(insert_str));
    }

    if (insert_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(insert_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalDelete *delete_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header = delete_node->ToString(intent_size);
    result->emplace_back(MakeShared<String>(header));
    if (delete_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(delete_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalUpdate *update_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header = update_node->ToString(intent_size);
    result->emplace_back(MakeShared<String>(header));
    if (update_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(update_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String project_header;
        if (intent_size != 0) {
            project_header = Format("{}-> PROJECT ", String(intent_size - 2, ' '));
        } else {
            project_header = "PROJECT ";
        }

        project_header += Format("({})", project_node->node_id());
        result->emplace_back(MakeShared<String>(project_header));
    }

    // Table index
    {
        String table_index = Format("{} - table index: #{}", String(intent_size, ' '), project_node->table_index_);
        result->emplace_back(MakeShared<String>(table_index));
    }

    // Expressions
    {
        String expression_str;
        expression_str.append(String(intent_size, ' ')).append(" - expressions: [");
        SizeT expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            Error<PlannerException>("No expression list in projection node.");
        }
        for (SizeT idx = 0; idx < expr_count - 1; ++idx) {
            Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(MakeShared<String>(expression_str));
    }

    if (project_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(project_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String filter_node_header;
    if (intent_size != 0) {
        filter_node_header = Format("{}-> FILTER ", String(intent_size - 2, ' '));
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += Format("({})", filter_node->node_id());
    result->emplace_back(MakeShared<String>(filter_node_header));

    // filter expression
    String filter_str = String(intent_size, ' ');
    filter_str += " - filter: ";
    Explain(filter_node->expression().get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    // Output column
    {
        String output_columns_str = String(intent_size, ' ');
        output_columns_str += " - output columns: [";
        SharedPtr<Vector<String>> output_columns = filter_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";

        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if (filter_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(filter_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String table_scan_header;
    if (intent_size != 0) {
        table_scan_header = String(intent_size - 2, ' ');
        table_scan_header += "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += Format("({})", table_scan_node->node_id());
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ');
    table_name += " - table name: ";
    table_name += table_scan_node->TableAlias();
    table_name += "(";

    DBEntry *db_entry = TableCollectionEntry::GetDBEntry(table_scan_node->table_collection_ptr());

    table_name += *db_entry->db_name_;
    table_name += ".";
    table_name += *table_scan_node->table_collection_ptr()->table_collection_name_;
    table_name += ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ');
    table_index += " - table index: #";
    table_index += ToStr(table_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ');
    output_columns += " - output columns: [";
    SizeT column_count = table_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        Error<PlannerException>(Format("No column in table: {}.", table_scan_node->TableAlias()));
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (table_scan_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(table_scan_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalKnnScan *knn_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String knn_scan_header;
    if (intent_size != 0) {
        knn_scan_header = String(intent_size - 2, ' ');
        knn_scan_header += "-> KNN SCAN ";
    } else {
        knn_scan_header = "KNN SCAN ";
    }

    knn_scan_header += Format("({})", knn_scan_node->node_id());
    result->emplace_back(MakeShared<String>(knn_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ');
    table_name += " - table name: ";
    table_name += knn_scan_node->TableAlias();
    table_name += "(";

    DBEntry *db_entry = TableCollectionEntry::GetDBEntry(knn_scan_node->table_collection_ptr());

    table_name += *db_entry->db_name_;
    table_name += ".";
    table_name += *knn_scan_node->table_collection_ptr()->table_collection_name_;
    table_name += ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ');
    table_index += " - table index: #";
    table_index += ToStr(knn_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    SizeT knn_expr_count = knn_scan_node->knn_expressions_.size();
    for (SizeT idx = 0; idx < knn_expr_count; ++idx) {
        const auto &knn_expression = knn_scan_node->knn_expressions_[idx];
        KnnExpression *knn_expr_raw = static_cast<KnnExpression *>(knn_expression.get());
        // Embedding info
        String embedding_info = String(intent_size, ' ');
        embedding_info += " - embedding info: ";
        embedding_info += knn_expr_raw->arguments().at(0)->Name();
        result->emplace_back(MakeShared<String>(embedding_info));

        String embedding_type_str = String(intent_size + 2, ' ');
        embedding_type_str += " - element type: ";
        embedding_type_str += EmbeddingT::EmbeddingDataType2String(knn_expr_raw->embedding_data_type_);
        result->emplace_back(MakeShared<String>(embedding_type_str));

        String embedding_dimension_str = String(intent_size + 2, ' ');
        embedding_dimension_str += " - dimension: ";
        embedding_type_str += ToStr(knn_expr_raw->dimension_);
        result->emplace_back(MakeShared<String>(embedding_dimension_str));

        String distance_type_str = String(intent_size + 2, ' ');
        distance_type_str += " - distance type: ";
        distance_type_str += KnnExpr::KnnDistanceType2Str(knn_expr_raw->distance_type_);
        result->emplace_back(MakeShared<String>(distance_type_str));

        // Query embedding
        String query_embedding = String(intent_size + 2, ' ');
        query_embedding += " - query embedding: ";
        query_embedding += EmbeddingT::Embedding2String(knn_expr_raw->query_embedding_, knn_expr_raw->embedding_data_type_, knn_expr_raw->dimension_);
        result->emplace_back(MakeShared<String>(query_embedding));
    }

    // filter expression
    String filter_str = String(intent_size, ' ');
    filter_str += " - filter: ";
    Explain(knn_scan_node->filter_expression_.get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    // Output columns
    String output_columns = String(intent_size, ' ');
    output_columns += " - output columns: [";
    SizeT column_count = knn_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        Error<PlannerException>(Format("No column in table: {}.", knn_scan_node->TableAlias()));
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += knn_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += knn_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (knn_scan_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(knn_scan_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if (groups_count == 0 && aggregate_node == 0) {
        Error<PlannerException>("Both groups and aggregates are empty.");
    }

    {
        String agg_header;
        if (intent_size != 0) {
            agg_header = String(intent_size - 2, ' ');
            agg_header += "-> AGGREGATE ";
        } else {
            agg_header = "AGGREGATE ";
        }

        agg_header += "(";
        agg_header += ToStr(aggregate_node->node_id());
        agg_header += ")";
        result->emplace_back(MakeShared<String>(agg_header));
    }

    // Aggregate Table index
    {
        String aggregate_table_index = String(intent_size, ' ');
        aggregate_table_index += " - aggregate table index: #";
        aggregate_table_index += ToStr(aggregate_node->aggregate_index_);
        result->emplace_back(MakeShared<String>(aggregate_table_index));
    }

    // Aggregate expressions
    {
        String aggregate_expression_str = String(intent_size, ' ');
        aggregate_expression_str += " - aggregate: [";
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
        String group_table_index = String(intent_size, ' ');
        group_table_index += " - group by table index: #";
        group_table_index += ToStr(aggregate_node->groupby_index_);
        result->emplace_back(MakeShared<String>(group_table_index));

        String group_by_expression_str = String(intent_size, ' ');
        group_by_expression_str += " - group by: [";
        for (SizeT idx = 0; idx < groups_count - 1; ++idx) {
            Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        Explain(aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(MakeShared<String>(group_by_expression_str));
    }

    if (aggregate_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(aggregate_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalSort *sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String sort_header;
        if (intent_size != 0) {
            sort_header = String(intent_size - 2, ' ');
            sort_header += "-> SORT ";
        } else {
            sort_header = "SORT ";
        }

        sort_header += "(";
        sort_header += ToStr(sort_node->node_id());
        sort_header += ")";
        result->emplace_back(MakeShared<String>(sort_header));
    }

    {
        String sort_expression_str = String(intent_size, ' ');
        sort_expression_str += " - expressions: [";
        SizeT order_by_count = sort_node->expressions_.size();
        if (order_by_count == 0) {
            Error<PlannerException>("ORDER BY without any expression.");
        }

        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " ";
            sort_expression_str += OrderBy2Str(sort_node->order_by_types_[idx]);
            sort_expression_str += ", ";
        }
        Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " ";
        sort_expression_str += OrderBy2Str(sort_node->order_by_types_.back());
        sort_expression_str += "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ');
        output_columns_str += " - output columns: [";
        SharedPtr<Vector<String>> output_columns = sort_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if (sort_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(sort_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalLimit *limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String limit_header;
        if (intent_size != 0) {
            limit_header = String(intent_size - 2, ' ');
            limit_header += "-> LIMIT ";
        } else {
            limit_header = "LIMIT ";
        }

        limit_header += "(";
        limit_header += ToStr(limit_node->node_id());
        limit_header += ")";
        result->emplace_back(MakeShared<String>(limit_header));
    }

    {
        String limit_value_str = String(intent_size, ' ');
        limit_value_str += " - limit: ";
        Explain(limit_node->limit_expression_.get(), limit_value_str);
        result->emplace_back(MakeShared<String>(limit_value_str));
    }

    if (limit_node->offset_expression_.get() != nullptr) {
        String offset_value_str = String(intent_size, ' ');
        offset_value_str += " - offset: ";
        Explain(limit_node->offset_expression_.get(), offset_value_str);
        result->emplace_back(MakeShared<String>(offset_value_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ');
        output_columns_str += " - output columns: [";
        SharedPtr<Vector<String>> output_columns = limit_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    if (limit_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(limit_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalCrossProduct *cross_product_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String cross_product_header;
        if (intent_size != 0) {
            cross_product_header = String(intent_size - 2, ' ');
            cross_product_header += "-> CROSS PRODUCT ";
        } else {
            cross_product_header = "CROSS PRODUCT ";
        }
        cross_product_header += "(";
        cross_product_header += ToStr(cross_product_node->node_id());
        cross_product_header += ")";
        result->emplace_back(MakeShared<String>(cross_product_header));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ');
        output_columns_str += " - output columns: [";
        SharedPtr<Vector<String>> output_columns = cross_product_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    intent_size += 2;
    if (cross_product_node->left_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(cross_product_node->left_node().get(), result, intent_size);
    }

    if (cross_product_node->right_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(cross_product_node->right_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String join_header;
        if (intent_size != 0) {
            join_header = String(intent_size - 2, ' ');
            join_header += "-> ";
        }
        join_header += JoinType2Str(join_node->join_type_);
        join_header += "(";
        join_header += ToStr(join_node->node_id());
        join_header += ")";
        result->emplace_back(MakeShared<String>(join_header));
    }

    // Conditions
    {
        String condition_str = String(intent_size, ' ');
        condition_str += " - filters: [";

        SizeT conditions_count = join_node->conditions_.size();
        if (conditions_count == 0) {
            Error<PlannerException>("JOIN without any condition.");
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
        String output_columns_str = String(intent_size, ' ');
        output_columns_str += " - output columns: [";
        SharedPtr<Vector<String>> output_columns = join_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }

    intent_size += 2;
    if (join_node->left_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->left_node().get(), result, intent_size);
    }

    if (join_node->right_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(join_node->right_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const LogicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    switch (show_node->scan_type()) {
        case ShowType::kShowTables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(";
            show_str += ToStr(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [database, table, type, column_count, row_count, segment_count, block_count, segment_capacity]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowViews: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW VIEWS ";
            } else {
                show_str = "SHOW VIEWS ";
            }
            show_str += "(";
            show_str += ToStr(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [schema, view, column_count]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowColumn: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> DESCRIBE TABLE/COLLECTION ";
            } else {
                show_str = "DESCRIBE TABLE/COLLECTION ";
            }
            show_str += "(";
            show_str += ToStr(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_column_schema_str = String(intent_size, ' ');
            show_column_schema_str += " - schema: ";
            show_column_schema_str += show_node->schema_name();
            result->emplace_back(MakeShared<String>(show_column_schema_str));

            String show_column_table_str = String(intent_size, ' ');
            show_column_table_str += " - table/collection: ";
            show_column_table_str += show_node->object_name();
            result->emplace_back(MakeShared<String>(show_column_table_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [column_name, column_type, constraint]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        default: {
            Error<PlannerException>("Invalid show type");
        }
    }

    if (show_node->left_node().get() != nullptr) {
        intent_size += 2;
        ExplainLogicalPlan::Explain(show_node->left_node().get(), result, intent_size);
    }
}

void ExplainLogicalPlan::Explain(const BaseExpression *base_expression, String &expr_str) {
    switch (base_expression->type()) {
        case ExpressionType::kAggregate: {
            AggregateExpression *aggregate_expression = (AggregateExpression *)base_expression;
            Assert<PlannerException>(aggregate_expression->arguments().size() == 1, "More than one argument in aggregate function");
            expr_str += aggregate_expression->aggregate_function_.name();
            expr_str += "(";
            Explain(aggregate_expression->arguments()[0].get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kCast: {
            CastExpression *cast_expression = (CastExpression *)base_expression;
            Assert<PlannerException>(cast_expression->arguments().size() == 1, "More than one argument in cast function");
            expr_str += "CAST(";
            Explain(cast_expression->arguments()[0].get(), expr_str);
            expr_str += " AS ";
            expr_str += cast_expression->Type().ToString();
            expr_str += ")";
            break;
        }
        case ExpressionType::kCase: {
            CaseExpression *case_expression = (CaseExpression *)base_expression;

            expr_str += "CASE ";
            SizeT case_expr_count = case_expression->CaseExpr().size();
            for (SizeT idx = 0; idx < case_expr_count; ++idx) {
                auto &check = case_expression->CaseExpr()[idx];
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
            ColumnExpression *column_expression = (ColumnExpression *)base_expression;
            expr_str += column_expression->Name();
            expr_str += " (#";
            expr_str += ToStr(column_expression->binding().table_idx);
            expr_str += ".";
            expr_str += ToStr(column_expression->binding().column_idx);
            expr_str += ")";
            break;
        }
        case ExpressionType::kFunction: {
            FunctionExpression *function_expression = (FunctionExpression *)base_expression;
            if (function_expression->arguments().empty()) {
                // No argument function
                expr_str += function_expression->func_.name();
                expr_str += "()";
                break;
            }

            if (function_expression->arguments().size() == 1) {
                // Unary function
                expr_str += function_expression->func_.name();
                expr_str += "(";
                Explain(function_expression->arguments()[0].get(), expr_str);
                expr_str += ")";
                break;
            }
            if (function_expression->arguments().size() == 2) {
                // Binary function
                Explain(function_expression->arguments()[0].get(), expr_str);
                expr_str += " ";
                expr_str += function_expression->func_.name();
                expr_str += " ";
                Explain(function_expression->arguments()[1].get(), expr_str);
                break;
            }

            // More than two arguments function
            expr_str += function_expression->func_.name();
            expr_str += "(";
            SizeT argument_count = function_expression->arguments().size();
            for (SizeT idx = 0; idx < argument_count - 1; ++idx) {
                Explain(function_expression->arguments()[idx].get(), expr_str);
                expr_str += ", ";
            }
            Explain(function_expression->arguments().back().get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kBetween: {
            BetweenExpression *between_expression = (BetweenExpression *)base_expression;
            Assert<PlannerException>(between_expression->arguments().size() == 3, "Between expression should have three arguments.");
            Explain(between_expression->arguments()[0].get(), expr_str);
            expr_str += " BETWEEN ";
            Explain(between_expression->arguments()[1].get(), expr_str);
            expr_str += " AND ";
            Explain(between_expression->arguments()[2].get(), expr_str);
            break;
        }
        case ExpressionType::kIn: {
            InExpression *in_expression = (InExpression *)base_expression;
            expr_str += "IN[";
            SizeT argument_count = in_expression->arguments().size();
            for (SizeT idx = 0; idx < argument_count - 1; ++idx) {
                Explain(in_expression->arguments()[idx].get(), expr_str);
                expr_str += ", ";
            }
            Explain(in_expression->arguments().back().get(), expr_str);
            expr_str += "]";
            break;
        }
        case ExpressionType::kValue: {
            ValueExpression *value_expression = (ValueExpression *)base_expression;
            expr_str += value_expression->ToString();
            break;
        }
        case ExpressionType::kReference: {
            ReferenceExpression *reference_expression = (ReferenceExpression *)base_expression;
            expr_str += reference_expression->Name();
            expr_str += " (#";
            expr_str += ToStr(reference_expression->column_index());
            expr_str += ")";
            break;
        }
        case ExpressionType::kSubQuery:
        case ExpressionType::kCorrelatedColumn:

        default: {
            Error<PlannerException>("Unsupported expression type");
        }
    }
}

void ExplainLogicalPlan::Explain(const LogicalImport *import_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String import_header_str;
        if (intent_size != 0) {
            import_header_str = String(intent_size - 2, ' ');
            import_header_str += "-> IMPORT ";
        } else {
            import_header_str = "IMPORT ";
        }

        import_header_str += "(";
        import_header_str += ToStr(import_node->node_id());
        import_header_str += ")";
        result->emplace_back(MakeShared<String>(import_header_str));
    }

    {
        DBEntry *db_entry = TableCollectionMeta::GetDBEntry(import_node->table_collection_entry()->table_collection_meta_);
        SharedPtr<String> schema_name = MakeShared<String>(Format("{} - schema name: {}", String(intent_size, ' '), *db_entry->db_name_));
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name = MakeShared<String>(
            Format("{} - table name: {}", String(intent_size, ' '), *import_node->table_collection_entry()->table_collection_name_));
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(Format("{} - file: {}", String(intent_size, ' '), import_node->file_path()));
        result->emplace_back(path);
    }

    switch (import_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: CSV", String(intent_size, ' ')));
            result->emplace_back(file_type);

            SharedPtr<String> header =
                MakeShared<String>(Format("{} - header: {}", String(intent_size, ' '), (import_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            SharedPtr<String> delimiter =
                MakeShared<String>(Format("{} - header: {} - delimiter: ", String(intent_size, ' '), import_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: JSON", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: FVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
    }

    if (import_node->left_node().get() != nullptr or import_node->right_node().get() != nullptr) {
        Error<PlannerException>("Import node have children nodes.");
    }
}

void ExplainLogicalPlan::Explain(const LogicalExport *export_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String export_header_str;
        if (intent_size != 0) {
            export_header_str = String(intent_size - 2, ' ');
            export_header_str += "-> EXPORT ";
        } else {
            export_header_str = "EXPORT ";
        }

        export_header_str += "(";
        export_header_str += ToStr(export_node->node_id());
        export_header_str += ")";
        result->emplace_back(MakeShared<String>(export_header_str));
    }

    {
        SharedPtr<String> schema_name = MakeShared<String>(Format("{} - schema name: {}", String(intent_size, ' '), export_node->schema_name()));
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name = MakeShared<String>(Format("{} - table name: {}", String(intent_size, ' '), export_node->table_name()));
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(Format("{} - file: {}", String(intent_size, ' '), export_node->file_path()));
        result->emplace_back(path);
    }

    switch (export_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: CSV", String(intent_size, ' ')));
            result->emplace_back(file_type);

            SharedPtr<String> header =
                MakeShared<String>(Format("{} - header: {}", String(intent_size, ' '), (export_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            SharedPtr<String> delimiter =
                MakeShared<String>(Format("{} - header: {} - delimiter: ", String(intent_size, ' '), export_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: JSON", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(Format("{} - type: FVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
    }

    if (export_node->left_node().get() != nullptr or export_node->right_node().get() != nullptr) {
        Error<PlannerException>("EXPORT node have children nodes.");
    }
}

void ExplainLogicalPlan::Explain(const LogicalFlush *flush_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {

    String flush_header_str;
    if (intent_size != 0) {
        flush_header_str = String(intent_size - 2, ' ');
        flush_header_str += "-> FLUSH ";
    } else {
        flush_header_str = "FLUSH ";
    }

    switch (flush_node->flush_type()) {
        case FlushType::kData:
            flush_header_str += "DATA (";
            flush_header_str += ToStr(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kLog:
            flush_header_str += "LOG (";
            flush_header_str += ToStr(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kBuffer:
            flush_header_str += "BUFFER (";
            flush_header_str += ToStr(flush_node->node_id());
            flush_header_str += ")";
            break;
    }

    result->emplace_back(MakeShared<String>(flush_header_str));
}

} // namespace infinity
