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

module explain_physical_plan;

import stl;
import internal_types;
import physical_operator;
import physical_union_all;
import physical_index_scan;
import physical_dummy_scan;
import physical_hash_join;
import physical_sort_merge_join;
import physical_index_join;
import physical_top;
import physical_delete;
import physical_update;
import physical_import;
import physical_export;
import physical_alter;
import physical_create_view;
import physical_drop_view;
import physical_create_schema;
import physical_create_table;
import physical_create_index_prepare;
import physical_create_collection;
import physical_drop_schema;
import physical_drop_table;
import physical_drop_collection;
import physical_insert;
import physical_project;
import physical_filter;
import physical_table_scan;
import physical_knn_scan;
import physical_aggregate;
import physical_sort;
import physical_limit;
import physical_cross_product;
import physical_nested_loop_join;
import physical_show;
import physical_flush;
import physical_source;
import physical_sink;
import physical_parallel_aggregate;
import physical_merge_parallel_aggregate;
import physical_intersect;
import physical_except;
import physical_hash;
import physical_merge_hash;
import physical_merge_limit;
import physical_merge_top;
import physical_merge_sort;
import physical_merge_knn;
import physical_merge_match_tensor;
import physical_match;
import physical_match_tensor_scan;
import physical_fusion;
import physical_merge_aggregate;
import status;
import physical_operator_type;

import explain_logical_plan;
import logical_show;
import infinity_exception;

import base_expression;
import knn_expression;
import third_party;
import select_statement;
import knn_expr;
import extra_ddl_info;
import column_def;
import statement_common;
import flush_statement;
import common_query_filter;
import table_entry;
import logger;

namespace infinity {

void ExplainPhysicalPlan::Explain(const PhysicalOperator *op, SharedPtr<Vector<SharedPtr<String>>> &result, bool is_recursive, i64 intent_size) {
    switch (op->operator_type()) {
        case PhysicalOperatorType::kAggregate: {
            Explain((PhysicalAggregate *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUnionAll: {
            Explain((PhysicalUnionAll *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kShow: {
            Explain((PhysicalShow *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            Explain((PhysicalTableScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            Explain((PhysicalKnnScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kFilter: {
            Explain((PhysicalFilter *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            Explain((PhysicalIndexScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDummyScan: {
            Explain((PhysicalDummyScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinHash: {
            Explain((PhysicalHashJoin *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinNestedLoop: {
            Explain((PhysicalNestedLoopJoin *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinMerge: {
            Explain((PhysicalSortMergeJoin *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kJoinIndex: {
            Explain((PhysicalIndexJoin *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCrossProduct: {
            Explain((PhysicalCrossProduct *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kLimit: {
            Explain((PhysicalLimit *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kTop: {
            Explain((PhysicalTop *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kProjection: {
            Explain((PhysicalProject *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kSort: {
            Explain((PhysicalSort *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDelete: {
            Explain((PhysicalDelete *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUpdate: {
            Explain((PhysicalUpdate *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kInsert: {
            Explain((PhysicalInsert *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kImport: {
            Explain((PhysicalImport *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kExport: {
            Explain((PhysicalExport *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kAlter: {
            Explain((PhysicalAlter *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateTable: {
            Explain((PhysicalCreateTable *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            Explain((PhysicalCreateIndexPrepare *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            Explain((PhysicalCreateCollection *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateDatabase: {
            Explain((PhysicalCreateSchema *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            Explain((PhysicalCreateView *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            Explain((PhysicalDropTable *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            Explain((PhysicalDropCollection *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropDatabase: {
            Explain((PhysicalDropSchema *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kDropView: {
            Explain((PhysicalDropView *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kExplain: {
            break;
        }
        case PhysicalOperatorType::kPreparedPlan: {
            break;
        }
        case PhysicalOperatorType::kFlush: {
            Explain((PhysicalFlush *)op, result, intent_size);
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
            Explain((PhysicalMergeTop *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeSort: {
            break;
        }
        case PhysicalOperatorType::kMergeKnn: {
            Explain((PhysicalMergeKnn *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeMatchTensor: {
            Explain((PhysicalMergeMatchTensor *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMatch: {
            Explain((PhysicalMatch *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMatchTensorScan: {
            Explain((PhysicalMatchTensorScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kFusion: {
            Explain((PhysicalFusion *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeAggregate: {
            Explain((PhysicalMergeAggregate *)op, result, intent_size);
            break;
        }
        default: {
            String error_message = "Unexpected physical operator type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    if (is_recursive) {
        if (op->left() != nullptr) {
            ExplainPhysicalPlan::Explain(op->left(), result, is_recursive, intent_size + 2);
        }

        if (op->right() != nullptr) {
            ExplainPhysicalPlan::Explain(op->right(), result, is_recursive, intent_size + 2);
        }
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String table_name_str = String(intent_size, ' ') + " - table name: " + *create_node->table_definition()->table_name();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->table_definition()->column_count();
        if (column_count == 0) {
            String error_message = "No columns in the table";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        const Vector<SharedPtr<ColumnDef>> &columns = create_node->table_definition()->columns();

        String columns_str = String(intent_size, ' ') + " - columns: [";
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            columns_str += columns[idx]->ToString() + ", ";
        }
        columns_str += columns.back()->ToString() + "]";
        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateIndexPrepare *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        auto &&db_name = create_node->base_table_ref_->schema_name();
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *db_name;
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    {
        auto &&table_name = create_node->base_table_ref_->table_name();
        String table_name_str = String(intent_size, ' ') + " - table name: " + *table_name;
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Index definition
    {
        String index_def_str = String(intent_size, ' ') + " - index definition: " + create_node->index_def_ptr_->ToString();
        result->emplace_back(MakeShared<String>(index_def_str));
    }

    {
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type_);
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateCollection *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + *insert_node->table_entry()->GetDBName();
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // Table name
    {
        String table_name_str = String(intent_size, ' ') + " - table name: " + *insert_node->table_entry()->GetTableName();
        result->emplace_back(MakeShared<String>(table_name_str));
    }

    // Values
    {
        String insert_str;
        insert_str = " - values ";
        SizeT value_count = insert_node->value_list().size();
        if (value_count == 0) {
            String error_message = "No value list in insert statement";
            LOG_CRITICAL(error_message);
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

void ExplainPhysicalPlan::Explain(const PhysicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(project_node->TableIndex());
        result->emplace_back(MakeShared<String>(table_index));
    }

    // Expressions
    {
        String expression_str = String(intent_size, ' ') + " - expressions: [";
        SizeT expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            String error_message = "No expression list in projection node.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        for (SizeT idx = 0; idx < expr_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(MakeShared<String>(expression_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String filter_node_header;
    if (intent_size != 0) {
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
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String table_scan_header;
    if (intent_size != 0) {
        table_scan_header = String(intent_size - 2, ' ') + "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += "(" + std::to_string(table_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(table_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + table_scan_node->table_alias() + "(";
    table_name += *table_scan_node->TableEntry()->GetDBName() + ".";
    table_name += *table_scan_node->TableEntry()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(table_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output_columns: [";
    SizeT column_count = table_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", table_scan_node->table_alias()));
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalIndexScan *index_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    // UnrecoverableError("Not implement: Explain Physical Index Scan");
    String index_scan_header;
    if (intent_size != 0) {
        index_scan_header = String(intent_size - 2, ' ') + "-> INDEX SCAN ";
    } else {
        index_scan_header = "INDEX SCAN ";
    }

    index_scan_header += "(" + std::to_string(index_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(index_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + index_scan_node->table_alias() + "(";
    table_name += *index_scan_node->TableEntry()->GetDBName() + ".";
    table_name += *index_scan_node->TableEntry()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(index_scan_node->TableIndex());
    result->emplace_back(MakeShared<String>(table_index));

    // filter expression
    String filter_str = String(intent_size, ' ') + " - filter: ";
    ExplainLogicalPlan::Explain(index_scan_node->FilterExpression().get(), filter_str);
    result->emplace_back(MakeShared<String>(filter_str));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output_columns: [";
    SizeT column_count = index_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", index_scan_node->table_alias()));
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += index_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += index_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalKnnScan *knn_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String knn_scan_header;
    if (intent_size != 0) {
        knn_scan_header = String(intent_size - 2, ' ') + "-> KNN SCAN ";
    } else {
        knn_scan_header = "KNN SCAN ";
    }

    knn_scan_header += "(" + std::to_string(knn_scan_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(knn_scan_header));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + knn_scan_node->TableAlias() + "(";

    table_name += *knn_scan_node->table_collection_ptr()->GetDBName() + ".";
    table_name += *knn_scan_node->table_collection_ptr()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(knn_scan_node->knn_table_index_);
    result->emplace_back(MakeShared<String>(table_index));

    KnnExpression *knn_expr_raw = knn_scan_node->knn_expression_.get();
    // Embedding info
    String embedding_info = String(intent_size, ' ') + " - embedding info: " + knn_expr_raw->arguments().at(0)->Name();
    result->emplace_back(MakeShared<String>(embedding_info));

    String embedding_type_str =
        String(intent_size + 2, ' ') + " - element type: " + EmbeddingT::EmbeddingDataType2String(knn_expr_raw->embedding_data_type_);
    result->emplace_back(MakeShared<String>(embedding_type_str));

    String embedding_dimension_str = String(intent_size + 2, ' ') + " - dimension: " + std::to_string(knn_expr_raw->dimension_);
    result->emplace_back(MakeShared<String>(embedding_dimension_str));

    String distance_type_str = String(intent_size + 2, ' ') + " - distance type: " + KnnExpr::KnnDistanceType2Str(knn_expr_raw->distance_type_);
    result->emplace_back(MakeShared<String>(distance_type_str));

    // Query embedding
    String query_embedding =
        String(intent_size + 2, ' ') + " - query embedding: " +
        EmbeddingT::Embedding2String(knn_expr_raw->query_embedding_, knn_expr_raw->embedding_data_type_, knn_expr_raw->dimension_);
    result->emplace_back(MakeShared<String>(query_embedding));

    // filter expression
    if (knn_scan_node->common_query_filter_ and knn_scan_node->common_query_filter_->original_filter_) {
        String filter_str = String(intent_size, ' ') + " - filter: ";
        ExplainLogicalPlan::Explain(knn_scan_node->common_query_filter_->original_filter_.get(), filter_str);
        result->emplace_back(MakeShared<String>(filter_str));
    }

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = knn_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", knn_scan_node->TableAlias()));
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += knn_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += knn_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (knn_scan_node->left() != nullptr) {
        String error_message = "Knn scan node have children nodes.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    SizeT groups_count = aggregate_node->groups_.size();
    SizeT aggregates_count = aggregate_node->aggregates_.size();
    if (groups_count == 0 && aggregates_count == 0) {
        String error_message = "Both groups and aggregates are empty.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
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
        String aggregate_table_index =
            String(intent_size, ' ') + " - aggregate table index: #" + std::to_string(aggregate_node->AggregateTableIndex());
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
        String group_table_index = String(intent_size, ' ') + " - group by table index: #" + std::to_string(aggregate_node->GroupTableIndex());
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
}

void ExplainPhysicalPlan::Explain(const PhysicalSort *sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
            String error_message = "ORDER BY without any expression.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }

        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(sort_node->order_by_types_[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(sort_node->order_by_types_.back()) + "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = sort_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalLimit *limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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

    if (limit_node->offset_expr().get() != 0) {
        String offset_value_str = String(intent_size, ' ') + " - offset: ";
        ExplainLogicalPlan::Explain(limit_node->offset_expr().get(), offset_value_str);
        result->emplace_back(MakeShared<String>(offset_value_str));
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = limit_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalTop *top_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        auto &sort_expressions = top_node->GetSortExpressions();
        SizeT order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            String error_message = "TOP without any sort expression.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        auto &order_by_types = top_node->GetOrderbyTypes();
        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    {
        auto limit = top_node->GetLimit();
        static_assert(std::is_same_v<decltype(limit), u32>);
        auto offset = top_node->GetOffset();
        static_assert(std::is_same_v<decltype(offset), u32>);
        auto limit_after_offset = limit - offset;
        if (limit_after_offset < 0) {
            String error_message = "TOP with limit < 0.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        String limit_value_str = String(intent_size, ' ') + " - limit: " + std::to_string(limit_after_offset);
        result->emplace_back(MakeShared<String>(limit_value_str));
        if (offset) {
            String offset_value_str = String(intent_size, ' ') + " - offset: " + std::to_string(offset);
            result->emplace_back(MakeShared<String>(offset_value_str));
        }
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

void ExplainPhysicalPlan::Explain(const PhysicalCrossProduct *cross_product_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalNestedLoopJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String join_header;
    if (intent_size != 0) {
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
            String error_message = "JOIN without any condition.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
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
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    switch (show_node->scan_type()) {
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
        case ShowType::kShowTables: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ') + "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(MakeShared<String>(show_str));

            String output_columns_str =
                String(intent_size, ' ') + " - output columns: [schema, table, type, column_count, row_count, block_count, block_size]";
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

            String show_column_db_str = String(intent_size, ' ') + " - database: ";
            show_column_db_str += show_node->db_name();
            result->emplace_back(MakeShared<String>(show_column_db_str));

            String show_column_table_str = String(intent_size, ' ') + " - table/collection: ";
            show_column_table_str += show_node->object_name();
            result->emplace_back(MakeShared<String>(show_column_table_str));

            String output_columns_str = String(intent_size, ' ') + " - output columns: [column_name, column_type, constraint]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowIndexes: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ') + "-> SHOW INDEXES ";
            } else {
                show_str = "SHOW INDEXES ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(MakeShared<String>(show_str));

            String show_column_db_str = String(intent_size, ' ') + " - database: ";
            show_column_db_str += show_node->db_name();
            result->emplace_back(MakeShared<String>(show_column_db_str));

            String show_column_table_str = String(intent_size, ' ') + " - table/collection: ";
            show_column_table_str += show_node->object_name();
            result->emplace_back(MakeShared<String>(show_column_table_str));

            String output_columns_str = String(intent_size, ' ') + " - output columns: [index_name, method_type, column_names, other_parameters]";
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

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kShowBlockColumn: {
            String show_str;
            if (intent_size != 0) {
                show_str = String(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK COLUMN ";
            } else {
                show_str = "SHOW BLOCK COLUMN ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(MakeShared<String>(show_str));

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
            output_columns_str += " - output columns: [name, value]";
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

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
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
            output_columns_str += " - output columns: [name, value]";
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

            String output_columns_str = String(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
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
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(MakeShared<String>(output_columns_str));
            break;
        }
        case ShowType::kInvalid: {
            String error_message = "Invalid show type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalUnionAll *, SharedPtr<Vector<SharedPtr<String>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalDummyScan *, SharedPtr<Vector<SharedPtr<String>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalHashJoin *, SharedPtr<Vector<SharedPtr<String>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalSortMergeJoin *, SharedPtr<Vector<SharedPtr<String>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalIndexJoin *, SharedPtr<Vector<SharedPtr<String>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalDelete *delete_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header;
    if (intent_size != 0) {
        header = String(intent_size - 2, ' ') + "-> DELETE FROM ";
    } else {
        header = "DELETE FROM ";
    }

    TableEntry *table_entry = delete_node->table_entry_ptr_;
    header += *table_entry->GetDBName() + "." + *table_entry->GetTableName();
    result->emplace_back(MakeShared<String>(header));
}

void ExplainPhysicalPlan::Explain(const PhysicalUpdate *update_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String header;
    if (intent_size != 0) {
        header = String(intent_size - 2, ' ') + "-> UPDATE ";
    } else {
        header = "UPDATE ";
    }

    TableEntry *table_entry = update_node->table_entry_ptr_;
    header += *table_entry->GetDBName() + "." + *table_entry->GetTableName();
    result->emplace_back(MakeShared<String>(header));
}

void ExplainPhysicalPlan::Explain(const PhysicalImport *import_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        SharedPtr<String> schema_name = MakeShared<String>(String(intent_size, ' ') + " - schema name: " + *import_node->table_entry()->GetDBName());
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name = MakeShared<String>(String(intent_size, ' ') + " - table name: " + *import_node->table_entry()->GetTableName());
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(String(intent_size, ' ') + " - file: " + import_node->file_path());
        result->emplace_back(path);
    }

    switch (import_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            SharedPtr<String> header = MakeShared<String>(String(intent_size, ' ') + " - header: " + (import_node->header() ? "Yes" : "No"));
            result->emplace_back(header);

            SharedPtr<String> delimiter = MakeShared<String>(String(intent_size, ' ') + " - delimiter: " + import_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: JSON");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: JSONL");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: FVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            String error_message = "Invalid show type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    if (import_node->left() != nullptr or import_node->right() != nullptr) {
        String error_message = "IMPORT node have children nodes.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalExport *export_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        SharedPtr<String> schema_name = MakeShared<String>(String(intent_size, ' ') + " - schema name: " + export_node->schema_name());
        result->emplace_back(schema_name);
    }

    {
        SharedPtr<String> table_name = MakeShared<String>(String(intent_size, ' ') + " - table name: " + export_node->table_name());
        result->emplace_back(table_name);
    }

    {
        SharedPtr<String> path = MakeShared<String>(String(intent_size, ' ') + " - file: " + export_node->file_path());
        result->emplace_back(path);
    }

    switch (export_node->FileType()) {
        case CopyFileType::kCSV: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            SharedPtr<String> header = MakeShared<String>(String(intent_size, ' ') + " - header: " + (export_node->header() ? "Yes" : "No"));
            result->emplace_back(header);

            SharedPtr<String> delimiter = MakeShared<String>(String(intent_size, ' ') + " - delimiter: " + export_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: JSON");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: JSONL");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            SharedPtr<String> file_type = MakeShared<String>(String(intent_size, ' ') + " - type: FVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            String error_message = "Invalid file type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }

    if (export_node->left() != nullptr or export_node->right() != nullptr) {
        String error_message = "EXPORT node have children nodes.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalAlter *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    Status status = Status::NotSupport("Not implemented");
    LOG_ERROR(status.message());
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String schema_name_str = String(intent_size, ' ') + " - schema name: " + create_node->create_view_info()->schema_name_;
        result->emplace_back(MakeShared<String>(schema_name_str));
    }

    // View name
    {
        String view_name_str = String(intent_size, ' ') + " - view name: " + create_node->create_view_info()->view_name_;
        result->emplace_back(MakeShared<String>(view_name_str));
    }

    // Column definition
    {
        SizeT column_count = create_node->GetOutputNames()->size();
        if (column_count == 0) {
            String error_message = "No column in the table";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        String columns_str = String(intent_size, ' ') + " - columns: [";
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            columns_str += create_node->GetOutputNames()->at(idx) + " " + create_node->GetOutputTypes()->at(idx)->ToString() + ", ";
        }
        columns_str += create_node->GetOutputNames()->back() + " " + create_node->GetOutputTypes()->back()->ToString() + "]";
        result->emplace_back(MakeShared<String>(columns_str));
    }

    // Conflict type
    {
        String conflict_type_str =
            String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->create_view_info()->conflict_type_);
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }

    // Text
    {
        String sql_text = String(intent_size, ' ') + " - text: Not implemented";
        result->emplace_back(MakeShared<String>(sql_text));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropView *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
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
        String conflict_type_str = String(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(MakeShared<String>(conflict_type_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFlush *flush_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String flush_header_str;
    if (intent_size != 0) {
        flush_header_str = String(intent_size - 2, ' ') + "-> FLUSH ";
    } else {
        flush_header_str = "FLUSH ";
    }

    switch (flush_node->flush_type()) {
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

void ExplainPhysicalPlan::Explain(const PhysicalSource *source_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> SOURCE ";
    } else {
        explain_header_str = "SOURCE ";
    }
    explain_header_str += "(" + std::to_string(source_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalSink *sink_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> SINK ";
    } else {
        explain_header_str = "SINK ";
    }
    explain_header_str += "(" + std::to_string(sink_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalParallelAggregate *parallel_aggregate_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "PARALLEL AGGREGATE ";
    }
    explain_header_str += "(" + std::to_string(parallel_aggregate_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeParallelAggregate *merge_parallel_aggregate_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "MERGE PARALLEL AGGREGATE ";
    }
    explain_header_str += "(" + std::to_string(merge_parallel_aggregate_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalIntersect *intersect_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> INTERSECT ";
    } else {
        explain_header_str = "INTERSECT ";
    }
    explain_header_str += "(" + std::to_string(intersect_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalExcept *except_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> EXCEPT ";
    } else {
        explain_header_str = "EXCEPT ";
    }
    explain_header_str += "(" + std::to_string(except_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalHash *hash_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> HASH ";
    } else {
        explain_header_str = "HASH ";
    }
    explain_header_str += "(" + std::to_string(hash_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeHash *merge_hash_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE HASH ";
    } else {
        explain_header_str = "MERGE HASH ";
    }
    explain_header_str += "(" + std::to_string(merge_hash_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeLimit *merge_limit_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE LIMIT ";
    } else {
        explain_header_str = "MERGE LIMIT ";
    }
    explain_header_str += "(" + std::to_string(merge_limit_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeTop *merge_top_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE TOP ";
    } else {
        explain_header_str = "MERGE TOP ";
    }
    explain_header_str += "(" + std::to_string(merge_top_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    {
        String sort_expression_str = String(intent_size, ' ') + " - sort expressions: [";
        auto &sort_expressions = merge_top_node->GetSortExpressions();
        SizeT order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            String error_message = "MERGE TOP without any sort expression.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        auto &order_by_types = merge_top_node->GetOrderbyTypes();
        for (SizeT idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(MakeShared<String>(sort_expression_str));
    }

    {
        auto limit = merge_top_node->GetLimit();
        static_assert(std::is_same_v<decltype(limit), u32>);
        auto offset = merge_top_node->GetOffset();
        static_assert(std::is_same_v<decltype(offset), u32>);
        if (limit < offset) {
            String error_message = "MERGE TOP with limit < 0.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        auto limit_after_offset = limit - offset;
        String limit_value_str = String(intent_size, ' ') + " - limit: " + std::to_string(limit_after_offset);
        result->emplace_back(MakeShared<String>(limit_value_str));
        if (offset) {
            String offset_value_str = String(intent_size, ' ') + " - offset: " + std::to_string(offset);
            result->emplace_back(MakeShared<String>(offset_value_str));
        }
    }

    // Output column
    {
        String output_columns_str = String(intent_size, ' ') + " - output columns: [";
        SharedPtr<Vector<String>> output_columns = merge_top_node->GetOutputNames();
        SizeT column_count = output_columns->size();
        for (SizeT idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(MakeShared<String>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeSort *merge_sort_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE SORT ";
    } else {
        explain_header_str = "MERGE SORT ";
    }
    explain_header_str += "(" + std::to_string(merge_sort_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeKnn *merge_knn_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,

                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE KNN ";
    } else {
        explain_header_str = "MERGE KNN ";
    }
    explain_header_str += "(" + std::to_string(merge_knn_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(merge_knn_node->knn_table_index());
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = merge_knn_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = "No column in merge knn node.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_knn_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_knn_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalMatch *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MATCH ";
    } else {
        explain_header_str = "MATCH ";
    }
    explain_header_str += "(" + std::to_string(match_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + match_node->TableAlias() + "(";

    table_name += *match_node->table_collection_ptr()->GetDBName() + ".";
    table_name += *match_node->table_collection_ptr()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(match_node->table_index());
    result->emplace_back(MakeShared<String>(table_index));

    String match_expression = String(intent_size, ' ') + " - match expression: " + match_node->match_expr()->ToString();
    result->emplace_back(MakeShared<String>(match_expression));

    // filter expression
    if (const CommonQueryFilter *filter = match_node->common_query_filter(); filter) {
        {
            String filter_str = String(intent_size, ' ') + " - filter for secondary index: ";
            if (const auto *filter_expr = filter->secondary_index_filter_qualified_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(MakeShared<String>(filter_str));
        }
        {
            String filter_str = String(intent_size, ' ') + " - filter except secondary index: ";
            if (const auto *filter_expr = filter->filter_leftover_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(MakeShared<String>(filter_str));
        }
    }

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = match_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = "No column in Match node.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += match_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += match_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (match_node->left() != nullptr) {
        String error_message = "Match node have children nodes.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMatchTensorScan *match_tensor_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MatchTensorScan ";
    } else {
        explain_header_str = "MatchTensorScan ";
    }
    explain_header_str += "(" + std::to_string(match_tensor_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + match_tensor_node->TableAlias() + "(";

    table_name += *match_tensor_node->table_collection_ptr()->GetDBName() + ".";
    table_name += *match_tensor_node->table_collection_ptr()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(match_tensor_node->table_index());
    result->emplace_back(MakeShared<String>(table_index));

    String match_tensor_expression = String(intent_size, ' ') + " - MatchTensor expression: " + match_tensor_node->match_tensor_expr()->ToString();
    result->emplace_back(MakeShared<String>(std::move(match_tensor_expression)));

    String top_n_expression = String(intent_size, ' ') + " - Top N: " + std::to_string(match_tensor_node->GetTopN());
    result->emplace_back(MakeShared<String>(std::move(top_n_expression)));

    // filter expression
    if (const CommonQueryFilter *filter = match_tensor_node->common_query_filter(); filter) {
        {
            String filter_str = String(intent_size, ' ') + " - filter for secondary index: ";
            if (const auto *filter_expr = filter->secondary_index_filter_qualified_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(MakeShared<String>(filter_str));
        }
        {
            String filter_str = String(intent_size, ' ') + " - filter except secondary index: ";
            if (const auto *filter_expr = filter->filter_leftover_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(MakeShared<String>(filter_str));
        }
    }

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = match_tensor_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = "No column in PhysicalMatchTensorScan node.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += match_tensor_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += match_tensor_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (match_tensor_node->left() != nullptr) {
        String error_message = "PhysicalMatchTensorScan node should not have children nodes.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeMatchTensor *merge_match_tensor_node,
                                  SharedPtr<Vector<SharedPtr<String>>> &result,
                                  i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> MERGE MatchTensor ";
    } else {
        explain_header_str = "MERGE MatchTensor ";
    }
    explain_header_str += "(" + std::to_string(merge_match_tensor_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Table alias and name
    String table_name = String(intent_size, ' ') + " - table name: " + merge_match_tensor_node->TableAlias() + "(";

    table_name += *merge_match_tensor_node->table_collection_ptr()->GetDBName() + ".";
    table_name += *merge_match_tensor_node->table_collection_ptr()->GetTableName() + ")";
    result->emplace_back(MakeShared<String>(table_name));

    // Table index
    String table_index = String(intent_size, ' ') + " - table index: #" + std::to_string(merge_match_tensor_node->table_index());
    result->emplace_back(MakeShared<String>(table_index));

    String match_tensor_expression =
        String(intent_size, ' ') + " - MatchTensor expression: " + merge_match_tensor_node->match_tensor_expr()->ToString();
    result->emplace_back(MakeShared<String>(std::move(match_tensor_expression)));

    String top_n_expression = String(intent_size, ' ') + " - Top N: " + std::to_string(merge_match_tensor_node->GetTopN());
    result->emplace_back(MakeShared<String>(std::move(top_n_expression)));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = merge_match_tensor_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = "No column in PhysicalMergeMatchTensor node.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_match_tensor_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_match_tensor_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));

    if (merge_match_tensor_node->left() == nullptr) {
        String error_message = "PhysicalMergeMatchTensor should have child node!";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFusion *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> FUSION ";
    } else {
        explain_header_str = "FUSION ";
    }
    explain_header_str += "(" + std::to_string(fusion_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Fusion expression
    String table_index = String(intent_size, ' ') + " - fusion: #" + fusion_node->fusion_expr_->ToString();
    result->emplace_back(MakeShared<String>(table_index));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = fusion_node->GetOutputNames()->size();
    if (column_count == 0) {
        String error_message = "No column in fusion node.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += fusion_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += fusion_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeAggregate *merge_aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size) {
    String explain_header_str;
    if (intent_size != 0) {
        explain_header_str = String(intent_size - 2, ' ') + "-> Merge aggregate ";
    } else {
        explain_header_str = "Merge aggregate ";
    }
    explain_header_str += "(" + std::to_string(merge_aggregate_node->node_id()) + ")";
    result->emplace_back(MakeShared<String>(explain_header_str));

    // Output columns
    String output_columns = String(intent_size, ' ') + " - output columns: [";
    SizeT column_count = merge_aggregate_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in merge aggregate node.");
    }
    for (SizeT idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_aggregate_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_aggregate_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(MakeShared<String>(output_columns));
}

} // namespace infinity
