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

module explain_logical_plan;

import stl;
import logical_node;
import logical_create_schema;
import logical_create_table;
import logical_create_index;
import logical_create_collection;
import logical_create_view;
import logical_drop_schema;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_view;
import logical_insert;
import logical_delete;
import logical_update;
import logical_project;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_aggregate;
import logical_sort;
import logical_limit;
import logical_top;
import logical_cross_product;
import logical_join;
import logical_show;
import logical_import;
import logical_export;
import logical_flush;
import logical_optimize;
import logical_match;
import logical_match_tensor_scan;
import logical_match_sparse_scan;
import logical_fusion;
import base_expression;

import logical_node_type;
import third_party;

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
import internal_types;
import knn_expr;
import select_statement;
import extra_ddl_info;
import column_def;
import join_reference;
import statement_common;
import flush_statement;
import optimize_statement;
import logger;

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
        case LogicalNodeType::kTop: {
            Explain((LogicalTop *)statement, result, intent_size);
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
        case LogicalNodeType::kOptimize: {
            Explain((LogicalOptimize *)statement, result, intent_size);
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
        case LogicalNodeType::kIndexScan: {
            Explain((LogicalIndexScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kKnnScan: {
            Explain((LogicalKnnScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatchSparseScan: {
            Explain((LogicalMatchSparseScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatch: {
            Explain((LogicalMatch *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatchTensorScan: {
            Explain((LogicalMatchTensorScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFusion: {
            Explain((LogicalFusion *)statement, result, intent_size);
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
            String error_message = "Unexpected logical node type";
            UnrecoverableError(error_message);
        }
    }
    if (statement->left_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(statement->left_node().get(), result, intent_size + 2);
    }
    if (statement->right_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(statement->right_node().get(), result, intent_size + 2);
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE SCHEMA ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE SCHEMA ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE TABLE ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE TABLE ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = fmt::format("{} - table name: {}", String(intent_size, ' '), *create_node->table_definitions()->table_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->table_definitions()->column_count();
        if (column_count == 0) {
            String error_message = "No columns in the table";
            UnrecoverableError(error_message);
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
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
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

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    {
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *create_node->base_table_ref()->schema_name();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    {
        String table_name_str = String(intent_size, ' ') + " - table name: " + *create_node->base_table_ref()->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Index definition
    {
        String index_def_str = String(intent_size, ' ') + " - index definition: " + create_node->index_definition()->ToString();
        result->emplace_back(MakeShared<String>(index_def_str));
    }

    {
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
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
            create_header_str = fmt::format("{}-> CREATE COLLECTION ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE COLLECTION ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String collection_name_str = fmt::format("{} - collection name: {}", String(intent_size, ' '), *create_node->collection_name());
        result->emplace_back(MakeShared<String>(collection_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE VIEW ", String(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE VIEW ";
        }
        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(MakeShared<String>(create_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), create_node->create_view_info()->schema_name_);
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String view_name_str = fmt::format("{} - view name: {}", String(intent_size, ' '), create_node->create_view_info()->view_name_);
        result->emplace_back(MakeShared<String>(view_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->names_ptr()->size();
        if (column_count == 0) {
            String error_message = "No columns in the table";
            UnrecoverableError(error_message);
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
            fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(create_node->create_view_info()->conflict_type_));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Text
    {
        String sql_text = fmt::format("{} - text: Not implemented", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(sql_text));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> CREATE SCHEMA ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP SCHEMA ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP TABLE ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP TABLE ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = fmt::format("{} - table: {}", String(intent_size, ' '), *drop_node->table_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP COLLECTION ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP COLLECTION ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Collection name
    {
        String table_name_str = fmt::format("{} - collection: {}", String(intent_size, ' '), *drop_node->collection_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalDropView *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP VIEW ", String(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP VIEW ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(MakeShared<String>(drop_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String table_name_str = fmt::format("{} - view name: {}", String(intent_size, ' '), *drop_node->view_name());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Conflict type
    {
        String conflict_type_str = fmt::format("{} - conflict type: {}", String(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = fmt::format("{} - output columns: [OK]", String(intent_size, ' '));
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String insert_header_str;
        if (intent_size != 0) {
            insert_header_str = fmt::format("{}-> INSERT ", String(intent_size - 2, ' '));
        } else {
            insert_header_str = "INSERT ";
        }

        insert_header_str += fmt::format("({})", insert_node->node_id());
        result->emplace_back(MakeShared<String>(insert_header_str));
    }

    // Schema name
    {
        String schema_name_str = fmt::format("{} - schema name: {}", String(intent_size, ' '), *insert_node->table_entry()->GetDBName());

        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = fmt::format("{} - table name: {}", String(intent_size, ' '), *insert_node->table_entry()->GetTableName());
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Values
    {
        String insert_str;
        insert_str = " - values ";
        SizeT value_count = insert_node->value_list().size();
        if (value_count == 0) {
            String error_message = "No value list in insert statement";
            UnrecoverableError(error_message);
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
}

void ExplainLogicalPlan::Explain(const LogicalDelete *delete_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header = delete_node->ToString(intent_size);
    result->emplace_back(MakeShared<String>(header));
}

void ExplainLogicalPlan::Explain(const LogicalUpdate *update_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header = update_node->ToString(intent_size);
    result->emplace_back(MakeShared<String>(header));
}

void ExplainLogicalPlan::Explain(const LogicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String project_header;
        if (intent_size != 0) {
            project_header = fmt::format("{}-> PROJECT ", String(intent_size - 2, ' '));
        } else {
            project_header = "PROJECT ";
        }

        project_header += fmt::format("({})", project_node->node_id());
        result->emplace_back(MakeShared<String>(project_header));
    }

    // Table index
    {
        String table_index = fmt::format("{} - table index: #{}", String(intent_size, ' '), project_node->table_index_);
        result->emplace_back(MakeShared<String>(table_index));
    }

    // Expressions
    {
        String expression_str;
        expression_str.append(String(intent_size, ' ')).append(" - expressions: [");
        SizeT expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            String error_message = "No expression list in projection node.";
            UnrecoverableError(error_message);
        }
        for (SizeT idx = 0; idx < expr_count - 1; ++idx) {
            Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(MakeShared<String>(expression_str));
    }
}

void ExplainLogicalPlan::Explain(const LogicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String filter_node_header;
    if (intent_size != 0) {
        filter_node_header = fmt::format("{}-> FILTER ", String(intent_size - 2, ' '));
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += fmt::format("({})", filter_node->node_id());
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
}

void ExplainLogicalPlan::Explain(const LogicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String table_scan_header;
    if (intent_size != 0) {
        table_scan_header = String(intent_size - 2, ' ');
        table_scan_header += "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += fmt::format("({})", table_scan_node->node_id());
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ');
    table_name += " - table name: ";
    table_name += table_scan_node->TableAlias();
    table_name += "(";

    table_name += *table_scan_node->table_collection_ptr()->GetDBName();
    table_name += ".";
    table_name += *table_scan_node->table_collection_ptr()->GetTableName();
    table_name += ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(table_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ');
    output_columns += " - output columns: [";
    SizeT column_count = table_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = fmt::format("No column in table: {}.", table_scan_node->TableAlias());
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainLogicalPlan::Explain(const LogicalIndexScan *index_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String index_scan_header;
    if (intent_size != 0) {
        index_scan_header = String(intent_size - 2, ' ');
        index_scan_header += "-> INDEX SCAN ";
    } else {
        index_scan_header = "INDEX SCAN ";
    }

    index_scan_header += fmt::format("({})", index_scan_node->node_id());
    result->emplace_back(MakeShared<String>(index_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ');
    table_name += " - table name: ";
    table_name += index_scan_node->TableAlias();
    table_name += "(";

    table_name += *index_scan_node->table_collection_ptr()->GetDBName();
    table_name += ".";
    table_name += *index_scan_node->table_collection_ptr()->GetTableName();
    table_name += ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(index_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // filter expression
    String filter_str = String(intent_size, ' ');
    filter_str += " - filter: ";
    Explain(index_scan_node->index_filter_.get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    // Output columns
    String output_columns = String(intent_size, ' ');
    output_columns += " - output columns: [";
    SizeT column_count = index_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = fmt::format("No column in table: {}.", index_scan_node->TableAlias());
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += index_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += index_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    // TODO: load meta
}

void ExplainLogicalPlan::Explain(const LogicalKnnScan *knn_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String knn_scan_header;
    if (intent_size != 0) {
        knn_scan_header = String(intent_size - 2, ' ');
        knn_scan_header += "-> KNN SCAN ";
    } else {
        knn_scan_header = "KNN SCAN ";
    }

    knn_scan_header += fmt::format("({})", knn_scan_node->node_id());
    result->emplace_back(MakeShared<String>(knn_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ');
    table_name += " - table name: ";
    table_name += knn_scan_node->TableAlias();
    table_name += "(";

    table_name += *knn_scan_node->table_collection_ptr()->GetDBName();
    table_name += ".";
    table_name += *knn_scan_node->table_collection_ptr()->GetTableName();
    table_name += ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(knn_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    KnnExpression *knn_expr_raw = knn_scan_node->knn_expression().get();
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
    embedding_type_str += std::to_string(knn_expr_raw->dimension_);
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

    // filter expression
    if (knn_scan_node->filter_expression_.get() != nullptr) {
        String filter_str = String(intent_size, ' ');
        filter_str += " - filter: ";
        Explain(knn_scan_node->filter_expression_.get(), filter_str);
        result->emplace_back(MakeShared<String>(filter_str));
    }

    // Output columns
    String output_columns = String(intent_size, ' ');
    output_columns += " - output columns: [";
    SizeT column_count = knn_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = fmt::format("No column in table: {}.", knn_scan_node->TableAlias());
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += knn_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += knn_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainLogicalPlan::Explain(const LogicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if (groups_count == 0 && aggregate_node == 0) {
        String error_message = "Both groups and aggregates are empty.";
        UnrecoverableError(error_message);
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
        agg_header += std::to_string(aggregate_node->node_id());
        agg_header += ")";
        result->emplace_back(MakeShared<String>(agg_header));
    }

    // Aggregate Table index
    {
        String aggregate_table_index = String(intent_size, ' ');
        aggregate_table_index += " - aggregate table index: #";
        aggregate_table_index += std::to_string(aggregate_node->aggregate_index_);
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
        group_table_index += std::to_string(aggregate_node->groupby_index_);
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
        sort_header += std::to_string(sort_node->node_id());
        sort_header += ")";
        result->emplace_back(MakeShared<String>(sort_header));
    }

    {
        String sort_expression_str = String(intent_size, ' ');
        sort_expression_str += " - expressions: [";
        SizeT order_by_count = sort_node->expressions_.size();
        if (order_by_count == 0) {
            String error_message = "ORDER BY without any expression.";
            UnrecoverableError(error_message);
        }

        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " ";
            sort_expression_str += SelectStatement::ToString(sort_node->order_by_types_[idx]);
            sort_expression_str += ", ";
        }
        Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " ";
        sort_expression_str += SelectStatement::ToString(sort_node->order_by_types_.back());
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
        limit_header += std::to_string(limit_node->node_id());
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
}

void ExplainLogicalPlan::Explain(const LogicalTop *top_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String top_header;
        if (intent_size != 0) {
            top_header = String(intent_size - 2, ' ') + "-> TOP ";
        } else {
            top_header = "TOP ";
        }

        top_header += "(" + std::to_string(top_node->node_id()) + ")";
        result->emplace_back(MakeShared<String>(top_header));
    }

    {
        String sort_expression_str = String(intent_size, ' ') + " - sort expressions: [";
        auto &sort_expressions = top_node->sort_expressions_;
        SizeT order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            String error_message = "TOP without any sort expression.";
            UnrecoverableError(error_message);
        }

        auto &order_by_types = top_node->order_by_types_;
        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    {
        String limit_value_str = String(intent_size, ' ') + " - limit: ";
        Explain(top_node->limit_expression_.get(), limit_value_str);
        result->emplace_back(MakeShared<String>(limit_value_str));
    }

    if (top_node->offset_expression_.get() != nullptr) {
        String offset_value_str = String(intent_size, ' ') + " - offset: ";
        Explain(top_node->offset_expression_.get(), offset_value_str);
        result->emplace_back(MakeShared<String>(offset_value_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = top_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
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
        cross_product_header += std::to_string(cross_product_node->node_id());
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
}

void ExplainLogicalPlan::Explain(const LogicalJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    {
        String join_header;
        if (intent_size != 0) {
            join_header = String(intent_size - 2, ' ');
            join_header += "-> ";
        }
        join_header += JoinReference::ToString(join_node->join_type_);
        join_header += "(";
        join_header += std::to_string(join_node->node_id());
        join_header += ")";
        result->emplace_back(MakeShared<String>(join_header));
    }

    // Conditions
    {
        String condition_str = String(intent_size, ' ');
        condition_str += " - filters: [";

        SizeT conditions_count = join_node->conditions_.size();
        if (conditions_count == 0) {
            String error_message = "JOIN without any condition.";
            UnrecoverableError(error_message);
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
}

void ExplainLogicalPlan::Explain(const LogicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    switch (show_node->show_type()) {
        case ShowType::kShowDatabase: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASE ";
            } else {
                show_str = "SHOW DATABASE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowTable: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowIndex: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX ";
            } else {
                show_str = "SHOW INDEX ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowIndexSegment: {
            String show_str;
            if (intent_size != 0) {
            show_str = String(intent_size - 2, ' ');
            show_str += "-> SHOW INDEX SEGMENT ";
            } else {
            show_str = "SHOW INDEX SEGMENT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowIndexChunk: {
            String show_str;
            if (intent_size != 0) {
            show_str = String(intent_size - 2, ' ');
            show_str += "-> SHOW INDEX CHUNK ";
            } else {
            show_str = "SHOW INDEX CHUNK ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowTables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
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
            show_str += std::to_string(show_node->node_id());
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
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_column_schema_str = String(intent_size, ' ');
            show_column_schema_str += " - database: ";
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
        case ShowType::kShowDatabases: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASES ";
            } else {
                show_str = "SHOW DATABASES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [database]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowConfigs: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIGS ";
            } else {
                show_str = "SHOW CONFIGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [config_name, value, description]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowBuffer: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW BUFFER ";
            } else {
                show_str = "SHOW BUFFER ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [path, status, size, buffered_type, type]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowMemIndex: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW MEMINDEX ";
            } else {
                show_str = "SHOW MEMINDEX ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [index_name, table_name, db_name, size, row_count]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowProfiles: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW PROFILES ";
            } else {
                show_str = "SHOW PROFILES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [record_no, parser, logical planner, optimizer, physical planner, pipeline builder, task "
                                  "builder, executor, total_cost]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowQueries: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW QUERIES ";
            } else {
                show_str = "SHOW QUERIES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [session_id, query_id, query_kind, start_time, time_consumption]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowQuery: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW QUERY ";
            } else {
                show_str = "SHOW QUERY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowTransactions: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTIONS ";
            } else {
                show_str = "SHOW TRANSACTIONS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [transaction_id, transaction_text]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowTransaction: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTION ";
            } else {
                show_str = "SHOW TRANSACTION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowIndexes: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW INDEXES ";
            } else {
                show_str = "SHOW INDEXES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [index_name, index_type, column_names, other_parameters]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowSegments: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENTS ";
            } else {
                show_str = "SHOW SEGMENTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            if (show_node->segment_id().has_value()) {
                String output_columns_str = String(intent_size, ' ');
                output_columns_str += " - segment: " + std::to_string(*show_node->segment_id());
                result->emplace_back(MakeShared<String>(output_columns_str));
            }

            if (show_node->block_id().has_value()) {
                String output_columns_str = String(intent_size, ' ');
                output_columns_str += " - block: " + std::to_string(*show_node->block_id());
                result->emplace_back(MakeShared<String>(output_columns_str));
            }

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [id, status, size]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowSegment: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENT ";
            } else {
                show_str = "SHOW SEGMENT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_segment_str = String(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(MakeShared<String>(show_segment_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str +=
                " - output columns: [id, status, dir, size, block_count, row_capacity, row_count, actual_row_count, room, column_count]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowBlocks: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCKS ";
            } else {
                show_str = "SHOW BLOCKS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            if (show_node->segment_id().has_value()) {
                String output_columns_str = String(intent_size, ' ');
                output_columns_str += " - segment: " + std::to_string(*show_node->segment_id());
                result->emplace_back(MakeShared<String>(output_columns_str));
            }

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [id, size, row_count]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowBlock: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK ";
            } else {
                show_str = "SHOW BLOCK ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_segment_str = String(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(MakeShared<String>(show_segment_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowBlockColumn: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK COLUMN";
            } else {
                show_str = "SHOW BLOCK COLUMN";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_segment_str = String(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(MakeShared<String>(show_segment_str));

            String show_block_str = String(intent_size, ' ');
            show_block_str += " - block: " + std::to_string(*show_node->block_id());
            result->emplace_back(MakeShared<String>(show_block_str));

            String show_column_str = String(intent_size, ' ');
            show_column_str += " - column: " + std::to_string(*show_node->column_id());
            result->emplace_back(MakeShared<String>(show_column_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowSessionVariable: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLE ";
            } else {
                show_str = "SHOW SESSION VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += fmt::format(" - variable: {}", show_node->object_name());
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowSessionVariables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLES ";
            } else {
                show_str = "SHOW SESSION VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowGlobalVariable: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLE ";
            } else {
                show_str = "SHOW GLOBAL VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += fmt::format(" - variable: {}", show_node->object_name());
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowGlobalVariables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLES ";
            } else {
                show_str = "SHOW GLOBAL VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowConfig: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIG ";
            } else {
                show_str = "SHOW CONFIG ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += fmt::format(" - config: {}", show_node->object_name());
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowLogs: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW LOGS ";
            } else {
                show_str = "SHOW LOGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowDeltaLogs: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW DELTA LOGS ";
            } else {
                show_str = "SHOW DELTA LOGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowCatalogs: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW CATALOGS ";
            } else {
                show_str = "SHOW CATALOGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowPersistenceFiles: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE FILES ";
            } else {
                show_str = "SHOW PERSISTENCE FILES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowPersistenceObjects: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECTS ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowPersistenceObject: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECT ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowMemory: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ";
            } else {
                show_str = "SHOW MEMORY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowMemoryObjects: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY OBJECTS ";
            } else {
                show_str = "SHOW MEMORY OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kShowMemoryAllocation: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ALLOCATION ";
            } else {
                show_str = "SHOW MEMORY ALLOCATION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));
            break;
        }
        case ShowType::kInvalid: {
            String error_message = "Invalid show type";
            UnrecoverableError(error_message);
        }
    }
}

void ExplainLogicalPlan::Explain(const BaseExpression *base_expression, String &expr_str, bool consider_add_parentheses) {
    switch (base_expression->type()) {
        case ExpressionType::kAggregate: {
            AggregateExpression *aggregate_expression = (AggregateExpression *)base_expression;
            if (aggregate_expression->arguments().size() != 1) {
                String error_message = "More than one argument in aggregate function";
                UnrecoverableError(error_message);
            }
            expr_str += aggregate_expression->aggregate_function_.name();
            expr_str += "(";
            Explain(aggregate_expression->arguments()[0].get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kCast: {
            CastExpression *cast_expression = (CastExpression *)base_expression;
            if (cast_expression->arguments().size() != 1) {
                String error_message = "More than one argument in cast function";
                UnrecoverableError(error_message);
            }
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
            expr_str += std::to_string(column_expression->binding().table_idx);
            expr_str += ".";
            expr_str += std::to_string(column_expression->binding().column_idx);
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
                if (consider_add_parentheses) {
                    expr_str += "(";
                }
                Explain(function_expression->arguments()[0].get(), expr_str, true);
                expr_str += " ";
                expr_str += function_expression->func_.name();
                expr_str += " ";
                Explain(function_expression->arguments()[1].get(), expr_str, true);
                if (consider_add_parentheses) {
                    expr_str += ")";
                }
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
            if (between_expression->arguments().size() != 3) {
                String error_message = "Between expression should have three arguments.";
                UnrecoverableError(error_message);
            }
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
            expr_str += std::to_string(reference_expression->column_index());
            expr_str += ")";
            break;
        }
        case ExpressionType::kFilterFullText: {
            expr_str += base_expression->ToString();
            break;
        }
        case ExpressionType::kSubQuery:
        case ExpressionType::kCorrelatedColumn:
        default: {
            String error_message = "Unsupported expression type";
            UnrecoverableError(error_message);
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
        import_header_str += std::to_string(import_node->node_id());
        import_header_str += ")";
        result->emplace_back(MakeShared<String>(import_header_str));
    }

    {
        SharedPtr<String> schema_name =
            MakeShared<String>(fmt::format("{} - schema name: {}", String(intent_size, ' '), *import_node->table_entry()->GetDBName()));
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name =
            MakeShared<String>(fmt::format("{} - table name: {}", String(intent_size, ' '), *import_node->table_entry()->GetTableName()));
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(fmt::format("{} - file: {}", String(intent_size, ' '), import_node->file_path()));
        result->emplace_back(path);
    }

    switch (import_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: CSV", String(intent_size, ' ')));
            result->emplace_back(file_type);

            SharedPtr<String> header =
                MakeShared<String>(fmt::format("{} - header: {}", String(intent_size, ' '), (import_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            SharedPtr<String> delimiter =
                MakeShared<String>(fmt::format("{} - header: {} - delimiter: ", String(intent_size, ' '), import_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: JSON", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: JSONL", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: FVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = MakeShared<String>(fmt::format("{} - type: CSR", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: BVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: PARQUET", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            String error_message = "Invalid file type";
            UnrecoverableError(error_message);
        }
    }

    if (import_node->left_node().get() != nullptr or import_node->right_node().get() != nullptr) {
        String error_message = "Import node have children nodes.";
        UnrecoverableError(error_message);
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
        export_header_str += std::to_string(export_node->node_id());
        export_header_str += ")";
        result->emplace_back(MakeShared<String>(export_header_str));
    }

    {
        SharedPtr<String> schema_name = MakeShared<String>(fmt::format("{} - schema name: {}", String(intent_size, ' '), export_node->schema_name()));
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name = MakeShared<String>(fmt::format("{} - table name: {}", String(intent_size, ' '), export_node->table_name()));
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(fmt::format("{} - file: {}", String(intent_size, ' '), export_node->file_path()));
        result->emplace_back(path);
    }

    switch (export_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: CSV", String(intent_size, ' ')));
            result->emplace_back(file_type);

            SharedPtr<String> header =
                MakeShared<String>(fmt::format("{} - header: {}", String(intent_size, ' '), (export_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            SharedPtr<String> delimiter =
                MakeShared<String>(fmt::format("{} - header: {} - delimiter: ", String(intent_size, ' '), export_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: JSON", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: JSONL", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: FVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = MakeShared<String>(fmt::format("{} - type: CSR", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: BVECS", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET : {
            SharedPtr<String> file_type = MakeShared<String>(fmt::format("{} - type: PARQUET", String(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            String error_message = "Invalid file type";
            UnrecoverableError(error_message);
        }
    }

    if (export_node->left_node().get() != nullptr or export_node->right_node().get() != nullptr) {
        String error_message = "EXPORT node have children nodes.";
        UnrecoverableError(error_message);
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
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kLog:
            flush_header_str += "LOG (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kBuffer:
            flush_header_str += "BUFFER (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
    }
    result->emplace_back(MakeShared<String>(flush_header_str));
}

void ExplainLogicalPlan::Explain(const LogicalOptimize *optimize_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String optimize_header_str;
    if (intent_size != 0) {
        optimize_header_str = String(intent_size - 2, ' ');
        optimize_header_str += "-> OPTIMIZE ";
    } else {
        optimize_header_str = "OPTIMIZE ";
    }

    result->emplace_back(MakeShared<String>(optimize_header_str));
}

void ExplainLogicalPlan::Explain(const LogicalMatch *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    IStringStream iss(match_node->ToString(intent_size));
    String line;
    while (std::getline(iss, line)) {
        result->emplace_back(MakeShared<String>(std::move(line)));
    }
}

void ExplainLogicalPlan::Explain(const LogicalMatchSparseScan *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    IStringStream iss(match_node->ToString(intent_size));
    String line;
    while (std::getline(iss, line)) {
        result->emplace_back(MakeShared<String>(std::move(line)));
    }
}

void ExplainLogicalPlan::Explain(const LogicalMatchTensorScan *match_tensor_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    IStringStream iss(match_tensor_node->ToString(intent_size));
    String line;
    while (std::getline(iss, line)) {
        result->emplace_back(MakeShared<String>(std::move(line)));
    }
}

void ExplainLogicalPlan::Explain(const LogicalFusion *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    IStringStream iss(fusion_node->ToString(intent_size));
    String line;
    while (std::getline(iss, line)) {
        result->emplace_back(MakeShared<String>(std::move(line)));
    }
}

} // namespace infinity
