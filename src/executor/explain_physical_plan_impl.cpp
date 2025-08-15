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

module infinity_core:explain_physical_plan.impl;

import :explain_physical_plan;

import :stl;
import :physical_operator;
import :physical_union_all;
import :physical_index_scan;
import :physical_dummy_scan;
import :physical_hash_join;
import :physical_sort_merge_join;
import :physical_index_join;
import :physical_top;
import :physical_delete;
import :physical_update;
import :physical_import;
import :physical_export;
import :physical_alter;
import :physical_create_view;
import :physical_drop_view;
import :physical_create_schema;
import :physical_create_table;
import :physical_create_index_prepare;
import :physical_create_collection;
import :physical_drop_schema;
import :physical_drop_table;
import :physical_drop_collection;
import :physical_insert;
import :physical_project;
import :physical_filter;
import :physical_table_scan;
import :physical_knn_scan;
import :physical_aggregate;
import :physical_sort;
import :physical_limit;
import :physical_cross_product;
import :physical_nested_loop_join;
import :physical_show;
import :physical_flush;
import :physical_source;
import :physical_sink;
import :physical_parallel_aggregate;
import :physical_merge_parallel_aggregate;
import :physical_intersect;
import :physical_except;
import :physical_hash;
import :physical_merge_hash;
import :physical_merge_limit;
import :physical_merge_top;
import :physical_merge_sort;
import :physical_merge_knn;
import :physical_merge_match_tensor;
import :physical_match;
import :physical_match_tensor_scan;
import :physical_fusion;
import :physical_match_sparse_scan;
import :physical_merge_match_sparse;
import :physical_merge_aggregate;
import :status;
import :physical_operator_type;
import :physical_read_cache;
import :physical_unnest;
import :physical_unnest_aggregate;
import :physical_check;
import :explain_logical_plan;
import :logical_show;
import :infinity_exception;
import :base_expression;
import :knn_expression;
import :common_query_filter;
import :logger;
import :base_table_ref;
import :meta_info;

import std;
import third_party;

import select_statement;
import knn_expr;
import extra_ddl_info;
import column_def;
import statement_common;
import flush_statement;
import internal_types;
import show_statement;
import check_statement;

namespace infinity {

void ExplainPhysicalPlan::Explain(const PhysicalOperator *op, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, bool is_recursive, i64 intent_size) {
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
            Explain((PhysicalSource *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kSink: {
            Explain((PhysicalSink *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kInvalid: {
            break;
        }
        case PhysicalOperatorType::kParallelAggregate: {
            Explain((PhysicalParallelAggregate *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate: {
            Explain((PhysicalMergeParallelAggregate *)op, result, intent_size);
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
            Explain((PhysicalMergeLimit *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeTop: {
            Explain((PhysicalMergeTop *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeSort: {
            Explain((PhysicalMergeSort *)op, result, intent_size);
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
        case PhysicalOperatorType::kMatchSparseScan: {
            Explain((PhysicalMatchSparseScan *)op, result, intent_size);
            break;
        }
        case PhysicalOperatorType::kMergeMatchSparse: {
            Explain((PhysicalMergeMatchSparse *)op, result, intent_size);
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
        case PhysicalOperatorType::kReadCache: {
            Explain(static_cast<const PhysicalReadCache *>(op), result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUnnest: {
            Explain(static_cast<const PhysicalUnnest *>(op), result, intent_size);
            break;
        }
        case PhysicalOperatorType::kUnnestAggregate: {
            Explain(static_cast<const PhysicalUnnestAggregate *>(op), result, intent_size);
            break;
        }
        case PhysicalOperatorType::kCheck: {
            Explain(static_cast<const PhysicalCheck *>(op), result, intent_size);
            break;
        }
        default: {
            UnrecoverableError("Unexpected physical operator type");
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

void ExplainPhysicalPlan::Explain(const PhysicalCreateSchema *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE SCHEMA ";
        } else {
            create_header_str = "CREATE SCHEMA ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *create_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateTable *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE TABLE ";
        } else {
            create_header_str = "CREATE TABLE ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *create_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str = std::string(intent_size, ' ') + " - table name: " + *create_node->table_definition()->table_name();
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Column definition
    {
        size_t column_count = create_node->table_definition()->column_count();
        if (column_count == 0) {
            UnrecoverableError("No columns in the table");
        }
        const std::vector<std::shared_ptr<ColumnDef>> &columns = create_node->table_definition()->columns();

        std::string columns_str = std::string(intent_size, ' ') + " - columns: [";
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            columns_str += columns[idx]->ToString() + ", ";
        }
        columns_str += columns.back()->ToString() + "]";
        result->emplace_back(std::make_shared<std::string>(columns_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateIndexPrepare *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE INDEX ";
        } else {
            create_header_str = "CREATE INDEX ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    {
        auto &&db_name = create_node->base_table_ref_->db_name();
        std::string db_name_str = std::string(intent_size, ' ') + " - database name: " + *db_name;
        result->emplace_back(std::make_shared<std::string>(db_name_str));
    }

    {
        auto &&table_name = create_node->base_table_ref_->table_name();
        std::string table_name_str = std::string(intent_size, ' ') + " - table name: " + *table_name;
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Index definition
    {
        std::string index_def_str = std::string(intent_size, ' ') + " - index definition: " + create_node->index_def_ptr_->ToString();
        result->emplace_back(std::make_shared<std::string>(index_def_str));
    }

    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type_);
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateCollection *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE COLLECTION ";
        } else {
            create_header_str = "CREATE COLLECTION ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *create_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Collection name
    {
        std::string collection_name_str = std::string(intent_size, ' ') + " - collection name: " + *create_node->collection_name();
        result->emplace_back(std::make_shared<std::string>(collection_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropSchema *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = std::string(intent_size - 2, ' ') + "-> DROP SCHEMA ";
        } else {
            drop_header_str = "DROP SCHEMA ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *drop_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropTable *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = std::string(intent_size - 2, ' ') + "-> DROP TABLE ";
        } else {
            drop_header_str = "DROP TABLE ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *drop_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str = std::string(intent_size, ' ') + " - table name: " + *drop_node->table_name();
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropCollection *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = std::string(intent_size - 2, ' ') + "-> DROP COLLECTION ";
        } else {
            drop_header_str = "DROP COLLECTION ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *drop_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Collection name
    {
        std::string table_name_str = std::string(intent_size, ' ') + " - collection name: " + *drop_node->collection_name();
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalInsert *insert_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string insert_header_str;
        if (intent_size != 0) {
            insert_header_str = std::string(intent_size - 2, ' ') + "-> INSERT ";
        } else {
            insert_header_str = "INSERT ";
        }

        insert_header_str += "(" + std::to_string(insert_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(insert_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *insert_node->table_info()->db_name_;
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str = std::string(intent_size, ' ') + " - table name: " + *insert_node->table_info()->table_name_;
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Values
    {
        std::string insert_str;
        insert_str = " - values ";
        size_t value_count = insert_node->value_list().size();
        if (value_count == 0) {
            UnrecoverableError("No value list in insert statement");
        }
        for (size_t idx = 0; idx < value_count; ++idx) {
            if (idx != 0)
                insert_str += ", ";
            const std::vector<std::shared_ptr<BaseExpression>> &value = insert_node->value_list()[idx];
            size_t column_count = value.size();
            for (size_t col = 0; col < column_count; ++col) {
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

        result->emplace_back(std::make_shared<std::string>(insert_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalProject *project_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string project_header;
        if (intent_size != 0) {
            project_header = std::string(intent_size - 2, ' ') + "-> PROJECT ";
        } else {
            project_header = "PROJECT ";
        }

        project_header += "(" + std::to_string(project_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(project_header));
    }

    // Table index
    {
        std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(project_node->TableIndex());
        result->emplace_back(std::make_shared<std::string>(table_index));
    }

    // Expressions
    {
        std::string expression_str = std::string(intent_size, ' ') + " - expressions: [";
        size_t expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            UnrecoverableError("No expression list in projection node.");
        }
        for (size_t idx = 0; idx < expr_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(expression_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFilter *filter_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string filter_node_header;
    if (intent_size != 0) {
        filter_node_header = std::string(intent_size - 2, ' ') + "-> FILTER ";
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += "(" + std::to_string(filter_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(filter_node_header));

    // filter expression
    std::string filter_str = std::string(intent_size, ' ') + " - filter: ";
    ExplainLogicalPlan::Explain(filter_node->condition().get(), filter_str);
    result->emplace_back(std::make_shared<std::string>(filter_str));

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = filter_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalTableScan *table_scan_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string table_scan_header;
    if (intent_size != 0) {
        table_scan_header = std::string(intent_size - 2, ' ') + "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += "(" + std::to_string(table_scan_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(table_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + table_scan_node->table_alias() + "(";
    table_name += *table_scan_node->table_info()->db_name_ + ".";
    table_name += *table_scan_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(table_scan_node->TableIndex());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output_columns: [";
    size_t column_count = table_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", table_scan_node->table_alias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalIndexScan *index_scan_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string index_scan_header;
    if (intent_size != 0) {
        index_scan_header = std::string(intent_size - 2, ' ') + "-> INDEX SCAN ";
    } else {
        index_scan_header = "INDEX SCAN ";
    }

    index_scan_header += "(" + std::to_string(index_scan_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(index_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + index_scan_node->table_alias() + "(";
    table_name += *index_scan_node->table_info()->db_name_ + ".";
    table_name += *index_scan_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(index_scan_node->TableIndex());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // filter expression
    std::string filter_str = std::string(intent_size, ' ') + " - filter: ";
    ExplainLogicalPlan::Explain(index_scan_node->FilterExpression().get(), filter_str);
    result->emplace_back(std::make_shared<std::string>(filter_str));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output_columns: [";
    size_t column_count = index_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", index_scan_node->table_alias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += index_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += index_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalKnnScan *knn_scan_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string knn_scan_header;
    if (intent_size != 0) {
        knn_scan_header = std::string(intent_size - 2, ' ') + "-> KNN SCAN ";
    } else {
        knn_scan_header = "KNN SCAN ";
    }

    knn_scan_header += "(" + std::to_string(knn_scan_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(knn_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + knn_scan_node->TableAlias() + "(";

    table_name += *knn_scan_node->table_info()->db_name_ + ".";
    table_name += *knn_scan_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(knn_scan_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    KnnExpression *knn_expr_raw = knn_scan_node->knn_expression_.get();
    // Embedding info
    std::string embedding_info = std::string(intent_size, ' ') + " - embedding info: " + knn_expr_raw->arguments().at(0)->Name();
    result->emplace_back(std::make_shared<std::string>(embedding_info));

    std::string embedding_type_str =
        std::string(intent_size + 2, ' ') + " - element type: " + EmbeddingT::EmbeddingDataType2String(knn_expr_raw->embedding_data_type_);
    result->emplace_back(std::make_shared<std::string>(embedding_type_str));

    std::string embedding_dimension_str = std::string(intent_size + 2, ' ') + " - dimension: " + std::to_string(knn_expr_raw->dimension_);
    result->emplace_back(std::make_shared<std::string>(embedding_dimension_str));

    std::string distance_type_str = std::string(intent_size + 2, ' ') + " - distance type: " + KnnExpr::KnnDistanceType2Str(knn_expr_raw->distance_type_);
    result->emplace_back(std::make_shared<std::string>(distance_type_str));

    // Query embedding
    std::string query_embedding =
        std::string(intent_size + 2, ' ') + " - query embedding: " +
        EmbeddingT::Embedding2String(knn_expr_raw->query_embedding_, knn_expr_raw->embedding_data_type_, knn_expr_raw->dimension_);
    result->emplace_back(std::make_shared<std::string>(query_embedding));

    // filter expression
    if (knn_scan_node->common_query_filter_ and knn_scan_node->common_query_filter_->original_filter_) {
        std::string filter_str = std::string(intent_size, ' ') + " - filter: ";
        ExplainLogicalPlan::Explain(knn_scan_node->common_query_filter_->original_filter_.get(), filter_str);
        result->emplace_back(std::make_shared<std::string>(filter_str));
    }

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = knn_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", knn_scan_node->TableAlias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += knn_scan_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += knn_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    if (knn_scan_node->left() != nullptr) {
        UnrecoverableError("Knn scan node have children nodes.");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalAggregate *aggregate_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    size_t groups_count = aggregate_node->groups_.size();
    size_t aggregates_count = aggregate_node->aggregates_.size();
    if (groups_count == 0 && aggregates_count == 0) {
        UnrecoverableError("Both groups and aggregates are empty.");
    }

    {
        std::string agg_header;
        if (intent_size != 0) {
            agg_header = std::string(intent_size - 2, ' ') + "-> AGGREGATE ";
        } else {
            agg_header = "AGGREGATE ";
        }

        agg_header += "(" + std::to_string(aggregate_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(agg_header));
    }

    // Aggregate Table index
    {
        std::string aggregate_table_index =
            std::string(intent_size, ' ') + " - aggregate table index: #" + std::to_string(aggregate_node->AggregateTableIndex());
        result->emplace_back(std::make_shared<std::string>(aggregate_table_index));
    }

    // Aggregate expressions
    {
        std::string aggregate_expression_str = std::string(intent_size, ' ') + " - aggregate: [";
        if (aggregates_count != 0) {
            for (size_t idx = 0; idx < aggregates_count - 1; ++idx) {
                ExplainLogicalPlan::Explain(aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            ExplainLogicalPlan::Explain(aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        std::string group_table_index = std::string(intent_size, ' ') + " - group by table index: #" + std::to_string(aggregate_node->GroupTableIndex());
        result->emplace_back(std::make_shared<std::string>(group_table_index));

        std::string group_by_expression_str = std::string(intent_size, ' ') + " - group by: [";
        for (size_t idx = 0; idx < groups_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(group_by_expression_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalSort *sort_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string sort_header;
        if (intent_size != 0) {
            sort_header = std::string(intent_size - 2, ' ') + "-> SORT ";
        } else {
            sort_header = "SORT ";
        }

        sort_header += "(" + std::to_string(sort_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(sort_header));
    }

    {
        std::string sort_expression_str = std::string(intent_size, ' ') + " - expressions: [";
        size_t order_by_count = sort_node->expressions_.size();
        if (order_by_count == 0) {
            UnrecoverableError("ORDER BY without any expression.");
        }

        for (size_t idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(sort_node->order_by_types_[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(sort_node->order_by_types_.back()) + "]";
        result->emplace_back(std::make_shared<std::string>(sort_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = sort_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalLimit *limit_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string limit_header;
        if (intent_size != 0) {
            limit_header = std::string(intent_size - 2, ' ') + "-> LIMIT ";
        } else {
            limit_header = "LIMIT ";
        }

        limit_header += "(" + std::to_string(limit_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(limit_header));
    }

    {
        std::string limit_value_str = std::string(intent_size, ' ') + " - limit: ";
        ExplainLogicalPlan::Explain(limit_node->limit_expr().get(), limit_value_str);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
    }

    if (limit_node->offset_expr().get() != 0) {
        std::string offset_value_str = std::string(intent_size, ' ') + " - offset: ";
        ExplainLogicalPlan::Explain(limit_node->offset_expr().get(), offset_value_str);
        result->emplace_back(std::make_shared<std::string>(offset_value_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = limit_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalTop *top_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string top_header;
        if (intent_size != 0) {
            top_header = std::string(intent_size - 2, ' ') + "-> TOP ";
        } else {
            top_header = "TOP ";
        }

        top_header += "(" + std::to_string(top_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(top_header));
    }

    {
        std::string sort_expression_str = std::string(intent_size, ' ') + " - sort expressions: [";
        auto &sort_expressions = top_node->GetSortExpressions();
        size_t order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            UnrecoverableError("TOP without any sort expression.");
        }
        auto &order_by_types = top_node->GetOrderbyTypes();
        for (size_t idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(std::make_shared<std::string>(sort_expression_str));
    }

    {
        auto limit = top_node->GetLimit();
        static_assert(std::is_same_v<decltype(limit), u32>);
        auto offset = top_node->GetOffset();
        static_assert(std::is_same_v<decltype(offset), u32>);
        auto limit_after_offset = limit - offset;
        if (limit_after_offset < 0) {
            UnrecoverableError("TOP with limit < 0.");
        }
        std::string limit_value_str = std::string(intent_size, ' ') + " - limit: " + std::to_string(limit_after_offset);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
        if (offset) {
            std::string offset_value_str = std::string(intent_size, ' ') + " - offset: " + std::to_string(offset);
            result->emplace_back(std::make_shared<std::string>(offset_value_str));
        }
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = top_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCrossProduct *cross_product_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string cross_product_header;
        if (intent_size != 0) {
            cross_product_header = std::string(intent_size - 2, ' ') + "-> CROSS PRODUCT ";
        } else {
            cross_product_header = "CROSS PRODUCT ";
        }
        cross_product_header += "(" + std::to_string(cross_product_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(cross_product_header));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = cross_product_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalNestedLoopJoin *join_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string join_header;
    if (intent_size != 0) {
        join_header = std::string(intent_size - 2, ' ') + "-> NESTED LOOP JOIN";
    } else {
        join_header = "NESTED LOOP JOIN ";
    }

    join_header += "(" + std::to_string(join_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(join_header));

    // Conditions
    {
        std::string condition_str = std::string(intent_size, ' ') + " - filters: [";

        size_t conditions_count = join_node->conditions().size();
        if (conditions_count == 0) {
            UnrecoverableError("JOIN without any condition.");
        }

        for (size_t idx = 0; idx < conditions_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(join_node->conditions()[idx].get(), condition_str);
            condition_str += ", ";
        }
        ExplainLogicalPlan::Explain(join_node->conditions().back().get(), condition_str);
        result->emplace_back(std::make_shared<std::string>(condition_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = join_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalShow *show_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (show_node->show_type()) {
        case ShowStmtType::kDatabase: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASE ";
            } else {
                show_str = "SHOW DATABASE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TABLE ";
            } else {
                show_str = "SHOW TABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kCollections: {
            UnrecoverableError("Show collections are not supported now");
            break;
        }
        case ShowStmtType::kIndex: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX ";
            } else {
                show_str = "SHOW INDEX ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexSegment: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX SEGMENT";
            } else {
                show_str = "SHOW INDEX SEGMENT";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexChunk: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX CHUNK";
            } else {
                show_str = "SHOW INDEX CHUNK";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str =
                std::string(intent_size, ' ') + " - output columns: [schema, table, type, column_count, row_count, block_count, block_size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBuffer: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW BUFFER ";
            } else {
                show_str = "SHOW BUFFER ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [path, status, size, buffered_type, type]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kMemIndex: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW MEM INDEX ";
            } else {
                show_str = "SHOW MEM INDEX ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [index_name, table_name, db_name, size, row_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTasks: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW TASKS ";
            } else {
                show_str = "SHOW TASKS ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [schema, view, column_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kColumns: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW COLUMN ";
            } else {
                show_str = "SHOW COLUMN ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_column_db_str = std::string(intent_size, ' ') + " - database: ";
            show_column_db_str += show_node->db_name();
            result->emplace_back(std::make_shared<std::string>(show_column_db_str));

            std::string show_column_table_str = std::string(intent_size, ' ') + " - table/collection: ";
            show_column_table_str += *(show_node->object_name());
            result->emplace_back(std::make_shared<std::string>(show_column_table_str));

            std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [column_name, column_type, constraint]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexes: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ') + "-> SHOW INDEXES ";
            } else {
                show_str = "SHOW INDEXES ";
            }
            show_str += "(" + std::to_string(show_node->node_id()) + ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_column_db_str = std::string(intent_size, ' ') + " - database: ";
            show_column_db_str += show_node->db_name();
            result->emplace_back(std::make_shared<std::string>(show_column_db_str));

            std::string show_column_table_str = std::string(intent_size, ' ') + " - table/collection: ";
            show_column_table_str += *(show_node->object_name());
            result->emplace_back(std::make_shared<std::string>(show_column_table_str));

            std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [index_name, method_type, column_names, other_parameters]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kDatabases: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASES ";
            } else {
                show_str = "SHOW DATABASES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [database]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kConfigs: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIGS ";
            } else {
                show_str = "SHOW CONFIGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [config_name, value, description]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kProfiles: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PROFILES ";
            } else {
                show_str = "SHOW PROFILES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [record_no, parser, logical planner, optimizer, physical planner, pipeline builder, task "
                                  "builder, executor, total_cost]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kQueries: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW QUERIES ";
            } else {
                show_str = "SHOW QUERIES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [session_id, query_id, query_kind, start_time, time_consumption]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kQuery: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW QUERY ";
            } else {
                show_str = "SHOW QUERY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }

        case ShowStmtType::kTransactions: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTIONS ";
            } else {
                show_str = "SHOW TRANSACTIONS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [transaction_id, transaction_text]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTransaction: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTION ";
            } else {
                show_str = "SHOW TRANSACTION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTION HISTORY";
            } else {
                show_str = "SHOW TRANSACTION HISTORY";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [transactions]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSegments: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENTS ";
            } else {
                show_str = "SHOW SEGMENTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, status, size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSegment: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENT ";
            } else {
                show_str = "SHOW SEGMENT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str +=
                " - output columns: [id, status, dir, size, block_count, row_capacity, row_count, actual_row_count, room, column_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlocks: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCKS ";
            } else {
                show_str = "SHOW BLOCKS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, size, row_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlock: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK ";
            } else {
                show_str = "SHOW BLOCK ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlockColumn: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK COLUMN ";
            } else {
                show_str = "SHOW BLOCK COLUMN ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSessionVariable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLE ";
            } else {
                show_str = "SHOW SESSION VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSessionVariables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLES ";
            } else {
                show_str = "SHOW SESSION VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLE ";
            } else {
                show_str = "SHOW GLOBAL VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLES ";
            } else {
                show_str = "SHOW GLOBAL VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kConfig: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIG ";
            } else {
                show_str = "SHOW CONFIG ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kLogs: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW LOGS ";
            } else {
                show_str = "SHOW LOGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [commit_ts, transaction_id, command_type, text]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kCatalog: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CATALOG ";
            } else {
                show_str = "SHOW CATALOG ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [meta_data_json]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CATALOG TO FILE ";
            } else {
                show_str = "SHOW CATALOG TO FILE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [operator_status]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECTS ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [file_name, object_name, offset, size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECTS ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [file_name, reference_count, size, deleted_size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECT ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [start_position, end_position]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kMemory: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ";
            } else {
                show_str = "SHOW MEMORY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY OBJECTS ";
            } else {
                show_str = "SHOW MEMORY OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ALLOCATION ";
            } else {
                show_str = "SHOW MEMORY ALLOCATION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, count, total_size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kListSnapshots: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SNAPSHOTS ";
            } else {
                show_str = "SHOW SNAPSHOTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, count, total_size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ALLOCATION ";
            } else {
                show_str = "SHOW MEMORY ALLOCATION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, count, total_size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kFunction: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW FUNCTION";
            } else {
                show_str = "SHOW FUNCTION";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kInvalid: {
            UnrecoverableError("Invalid show type");
        }
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalUnionAll *, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalDummyScan *, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalHashJoin *, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalSortMergeJoin *, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalIndexJoin *, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &, i64) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalDelete *delete_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string header;
    if (intent_size != 0) {
        header = std::string(intent_size - 2, ' ') + "-> DELETE FROM ";
    } else {
        header = "DELETE FROM ";
    }

    TableInfo *table_info = delete_node->table_info_.get();
    header += *table_info->db_name_ + "." + *table_info->table_name_;
    result->emplace_back(std::make_shared<std::string>(header));
}

void ExplainPhysicalPlan::Explain(const PhysicalUpdate *update_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string header;
    if (intent_size != 0) {
        header = std::string(intent_size - 2, ' ') + "-> UPDATE ";
    } else {
        header = "UPDATE ";
    }

    TableInfo *table_info = update_node->table_info_.get();
    header += *table_info->db_name_ + "." + *table_info->table_name_;
    result->emplace_back(std::make_shared<std::string>(header));
}

void ExplainPhysicalPlan::Explain(const PhysicalImport *import_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string import_header_str;
        if (intent_size != 0) {
            import_header_str = std::string(intent_size - 2, ' ') + "-> IMPORT ";
        } else {
            import_header_str = "IMPORT ";
        }

        import_header_str += "(" + std::to_string(import_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(import_header_str));
    }

    {
        std::shared_ptr<std::string> schema_name = std::make_shared<std::string>(std::string(intent_size, ' ') + " - database name: " + *(import_node->table_info()->db_name_));
        result->emplace_back(schema_name);
    }

    {
        std::shared_ptr<std::string> table_name = std::make_shared<std::string>(std::string(intent_size, ' ') + " - table name: " + *(import_node->table_info()->table_name_));
        result->emplace_back(table_name);
    }

    {
        std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(intent_size, ' ') + " - file: " + import_node->file_path());
        result->emplace_back(path);
    }

    switch (import_node->FileType()) {
        case CopyFileType::kCSV: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            std::shared_ptr<std::string> header = std::make_shared<std::string>(std::string(intent_size, ' ') + " - header: " + (import_node->header() ? "Yes" : "No"));
            result->emplace_back(header);

            std::shared_ptr<std::string> delimiter = std::make_shared<std::string>(std::string(intent_size, ' ') + " - delimiter: " + import_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: JSON");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: JSONL");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: FVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: CSR");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: BVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: PARQUET");
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid show type");
        }
    }

    if (import_node->left() != nullptr or import_node->right() != nullptr) {
        UnrecoverableError("IMPORT node have children nodes.");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalExport *export_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string export_header_str;
        if (intent_size != 0) {
            export_header_str = std::string(intent_size - 2, ' ') + "-> EXPORT ";
        } else {
            export_header_str = "EXPORT ";
        }

        export_header_str += "(" + std::to_string(export_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(export_header_str));
    }

    {
        std::shared_ptr<std::string> schema_name = std::make_shared<std::string>(std::string(intent_size, ' ') + " - database name: " + export_node->schema_name());
        result->emplace_back(schema_name);
    }

    {
        std::shared_ptr<std::string> table_name = std::make_shared<std::string>(std::string(intent_size, ' ') + " - table name: " + export_node->table_name());
        result->emplace_back(table_name);
    }

    {
        std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(intent_size, ' ') + " - file: " + export_node->file_path());
        result->emplace_back(path);
    }

    switch (export_node->FileType()) {
        case CopyFileType::kCSV: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: CSV");
            result->emplace_back(file_type);

            std::shared_ptr<std::string> header = std::make_shared<std::string>(std::string(intent_size, ' ') + " - header: " + (export_node->header() ? "Yes" : "No"));
            result->emplace_back(header);

            std::shared_ptr<std::string> delimiter = std::make_shared<std::string>(std::string(intent_size, ' ') + " - delimiter: " + export_node->delimiter());
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: JSON");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: JSONL");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: FVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: CSR");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: BVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + " - type: PARQUET");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }

    if (export_node->left() != nullptr or export_node->right() != nullptr) {
        UnrecoverableError("EXPORT node have children nodes.");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalAlter *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
}

void ExplainPhysicalPlan::Explain(const PhysicalCreateView *create_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE VIEW ";
        } else {
            create_header_str = "CREATE VIEW ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + create_node->create_view_info()->schema_name_;
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // View name
    {
        std::string view_name_str = std::string(intent_size, ' ') + " - view name: " + create_node->create_view_info()->view_name_;
        result->emplace_back(std::make_shared<std::string>(view_name_str));
    }

    // Column definition
    {
        size_t column_count = create_node->GetOutputNames()->size();
        if (column_count == 0) {
            UnrecoverableError("No column in the table");
        }
        std::string columns_str = std::string(intent_size, ' ') + " - columns: [";
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            columns_str += create_node->GetOutputNames()->at(idx) + " " + create_node->GetOutputTypes()->at(idx)->ToString() + ", ";
        }
        columns_str += create_node->GetOutputNames()->back() + " " + create_node->GetOutputTypes()->back()->ToString() + "]";
        result->emplace_back(std::make_shared<std::string>(columns_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->create_view_info()->conflict_type_);
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Text
    {
        std::string sql_text = std::string(intent_size, ' ') + " - text: Not implemented";
        result->emplace_back(std::make_shared<std::string>(sql_text));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalDropView *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = std::string(intent_size - 2, ' ') + "-> DROP VIEW ";
        } else {
            drop_header_str = "DROP VIEW ";
        }

        drop_header_str += "(" + std::to_string(drop_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *drop_node->schema_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Collection name
    {
        std::string table_name_str = std::string(intent_size, ' ') + " - view name: " + *drop_node->view_name();
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(drop_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFlush *flush_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string flush_header_str;
    if (intent_size != 0) {
        flush_header_str = std::string(intent_size - 2, ' ') + "-> FLUSH ";
    } else {
        flush_header_str = "FLUSH ";
    }

    switch (flush_node->flush_type()) {
        case FlushType::kCatalog:
            flush_header_str += "CATALOG (" + std::to_string(flush_node->node_id()) + ")";
            break;
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

    result->emplace_back(std::make_shared<std::string>(flush_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalSource *source_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> SOURCE ";
    } else {
        explain_header_str = "SOURCE ";
    }
    explain_header_str += "(" + std::to_string(source_node->node_id()) + ") " + ToString(source_node->source_type());
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalSink *sink_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> SINK ";
    } else {
        explain_header_str = "SINK ";
    }
    explain_header_str += "(" + std::to_string(sink_node->node_id()) + ") " + ToString(sink_node->sink_type());
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalParallelAggregate *parallel_aggregate_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "PARALLEL AGGREGATE ";
    }
    explain_header_str += "(" + std::to_string(parallel_aggregate_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeParallelAggregate *merge_parallel_aggregate_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE PARALLEL AGGREGATE ";
    } else {
        explain_header_str = "MERGE PARALLEL AGGREGATE ";
    }
    explain_header_str += "(" + std::to_string(merge_parallel_aggregate_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalIntersect *intersect_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> INTERSECT ";
    } else {
        explain_header_str = "INTERSECT ";
    }
    explain_header_str += "(" + std::to_string(intersect_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalExcept *except_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> EXCEPT ";
    } else {
        explain_header_str = "EXCEPT ";
    }
    explain_header_str += "(" + std::to_string(except_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalHash *hash_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> HASH ";
    } else {
        explain_header_str = "HASH ";
    }
    explain_header_str += "(" + std::to_string(hash_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeHash *merge_hash_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE HASH ";
    } else {
        explain_header_str = "MERGE HASH ";
    }
    explain_header_str += "(" + std::to_string(merge_hash_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeLimit *merge_limit_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE LIMIT ";
    } else {
        explain_header_str = "MERGE LIMIT ";
    }
    explain_header_str += "(" + std::to_string(merge_limit_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    {
        std::string limit_value_str = std::string(intent_size, ' ') + " - limit: ";
        ExplainLogicalPlan::Explain(merge_limit_node->limit_expr().get(), limit_value_str);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
    }

    if (merge_limit_node->offset_expr().get() != 0) {
        std::string offset_value_str = std::string(intent_size, ' ') + " - offset: ";
        ExplainLogicalPlan::Explain(merge_limit_node->offset_expr().get(), offset_value_str);
        result->emplace_back(std::make_shared<std::string>(offset_value_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = merge_limit_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeTop *merge_top_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE TOP ";
    } else {
        explain_header_str = "MERGE TOP ";
    }
    explain_header_str += "(" + std::to_string(merge_top_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    {
        std::string sort_expression_str = std::string(intent_size, ' ') + " - sort expressions: [";
        auto &sort_expressions = merge_top_node->GetSortExpressions();
        size_t order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            UnrecoverableError("MERGE TOP without any sort expression.");
        }
        auto &order_by_types = merge_top_node->GetOrderbyTypes();
        for (size_t idx = 0; idx < order_by_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        ExplainLogicalPlan::Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(std::make_shared<std::string>(sort_expression_str));
    }

    {
        auto limit = merge_top_node->GetLimit();
        static_assert(std::is_same_v<decltype(limit), u32>);
        auto offset = merge_top_node->GetOffset();
        static_assert(std::is_same_v<decltype(offset), u32>);
        if (limit < offset) {
            UnrecoverableError("MERGE TOP with limit < 0.");
        }
        auto limit_after_offset = limit - offset;
        std::string limit_value_str = std::string(intent_size, ' ') + " - limit: " + std::to_string(limit_after_offset);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
        if (offset) {
            std::string offset_value_str = std::string(intent_size, ' ') + " - offset: " + std::to_string(offset);
            result->emplace_back(std::make_shared<std::string>(offset_value_str));
        }
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = merge_top_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeSort *merge_sort_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE SORT ";
    } else {
        explain_header_str = "MERGE SORT ";
    }
    explain_header_str += "(" + std::to_string(merge_sort_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeKnn *merge_knn_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,

                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE KNN ";
    } else {
        explain_header_str = "MERGE KNN ";
    }
    explain_header_str += "(" + std::to_string(merge_knn_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(merge_knn_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = merge_knn_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in merge knn node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_knn_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_knn_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalMatch *match_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MATCH ";
    } else {
        explain_header_str = "MATCH ";
    }
    explain_header_str += "(" + std::to_string(match_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + match_node->TableAlias() + "(";

    table_name += *match_node->table_info()->db_name_ + ".";
    table_name += *match_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(match_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    std::string match_expression = std::string(intent_size, ' ') + " - match expression: " + match_node->match_expr()->ToString();
    result->emplace_back(std::make_shared<std::string>(match_expression));

    // filter expression
    if (const CommonQueryFilter *filter = match_node->common_query_filter(); filter) {
        {
            std::string filter_str = std::string(intent_size, ' ') + " - index filter: ";
            if (const auto *filter_expr = filter->index_filter_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(std::make_shared<std::string>(filter_str));
        }
        {
            std::string filter_str = std::string(intent_size, ' ') + " - leftover filter: ";
            if (const auto *filter_expr = filter->leftover_filter_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(std::make_shared<std::string>(filter_str));
        }
    }

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = match_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in Match node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += match_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += match_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    if (match_node->left() != nullptr) {
        UnrecoverableError("Match node have children nodes.");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMatchSparseScan *match_sparse_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MatchSparseScan ";
    } else {
        explain_header_str = "MatchSparseScan ";
    }
    explain_header_str += "(" + std::to_string(match_sparse_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(match_sparse_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = match_sparse_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in match sparse node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += match_sparse_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += match_sparse_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalMatchTensorScan *match_tensor_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MatchTensorScan ";
    } else {
        explain_header_str = "MatchTensorScan ";
    }
    explain_header_str += "(" + std::to_string(match_tensor_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + match_tensor_node->TableAlias() + "(";

    table_name += *match_tensor_node->table_info()->db_name_ + ".";
    table_name += *match_tensor_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(match_tensor_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    std::string match_tensor_expression = std::string(intent_size, ' ') + " - MatchTensor expression: " + match_tensor_node->match_tensor_expr()->ToString();
    result->emplace_back(std::make_shared<std::string>(std::move(match_tensor_expression)));

    std::string top_n_expression = std::string(intent_size, ' ') + " - Top N: " + std::to_string(match_tensor_node->GetTopN());
    result->emplace_back(std::make_shared<std::string>(std::move(top_n_expression)));

    // filter expression
    if (const CommonQueryFilter *filter = match_tensor_node->common_query_filter(); filter) {
        {
            std::string filter_str = std::string(intent_size, ' ') + " - index filter: ";
            if (const auto *filter_expr = filter->index_filter_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(std::make_shared<std::string>(filter_str));
        }
        {
            std::string filter_str = std::string(intent_size, ' ') + " - leftover filter: ";
            if (const auto *filter_expr = filter->leftover_filter_.get(); filter_expr) {
                ExplainLogicalPlan::Explain(filter_expr, filter_str);
            } else {
                filter_str += "None";
            }
            result->emplace_back(std::make_shared<std::string>(filter_str));
        }
    }

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = match_tensor_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in PhysicalMatchTensorScan node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += match_tensor_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += match_tensor_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    if (match_tensor_node->left() != nullptr) {
        UnrecoverableError("PhysicalMatchTensorScan node should not have children nodes.");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeMatchTensor *merge_match_tensor_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE MatchTensor ";
    } else {
        explain_header_str = "MERGE MatchTensor ";
    }
    explain_header_str += "(" + std::to_string(merge_match_tensor_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + merge_match_tensor_node->TableAlias() + "(";

    table_name += *merge_match_tensor_node->table_info()->db_name_ + ".";
    table_name += *merge_match_tensor_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(merge_match_tensor_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    std::string match_tensor_expression =
        std::string(intent_size, ' ') + " - MatchTensor expression: " + merge_match_tensor_node->match_tensor_expr()->ToString();
    result->emplace_back(std::make_shared<std::string>(std::move(match_tensor_expression)));

    std::string top_n_expression = std::string(intent_size, ' ') + " - Top N: " + std::to_string(merge_match_tensor_node->GetTopN());
    result->emplace_back(std::make_shared<std::string>(std::move(top_n_expression)));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = merge_match_tensor_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in PhysicalMergeMatchTensor node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_match_tensor_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_match_tensor_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    if (merge_match_tensor_node->left() == nullptr) {
        UnrecoverableError("PhysicalMergeMatchTensor should have child node!");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeMatchSparse *merge_match_sparse_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                  i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> MERGE MatchSparse ";
    } else {
        explain_header_str = "MERGE MatchSparse ";
    }
    explain_header_str += "(" + std::to_string(merge_match_sparse_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: " + merge_match_sparse_node->TableAlias() + "(";

    table_name += *merge_match_sparse_node->table_info()->db_name_ + ".";
    table_name += *merge_match_sparse_node->table_info()->table_name_ + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ') + " - table index: #" + std::to_string(merge_match_sparse_node->table_index());
    result->emplace_back(std::make_shared<std::string>(table_index));

    std::string match_sparse_expression =
        std::string(intent_size, ' ') + " - MatchSparse expression: " + merge_match_sparse_node->match_sparse_expr()->ToString();
    result->emplace_back(std::make_shared<std::string>(std::move(match_sparse_expression)));

    std::string top_n_expression = std::string(intent_size, ' ') + " - Top N: " + std::to_string(merge_match_sparse_node->GetTopN());
    result->emplace_back(std::make_shared<std::string>(std::move(top_n_expression)));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = merge_match_sparse_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in PhysicalMergeMatchSparse node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_match_sparse_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_match_sparse_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    if (merge_match_sparse_node->left() == nullptr) {
        UnrecoverableError("PhysicalMergeMatchSparse should have child node!");
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalFusion *fusion_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> FUSION ";
    } else {
        explain_header_str = "FUSION ";
    }
    explain_header_str += "(" + std::to_string(fusion_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Fusion expression
    std::string table_index = std::string(intent_size, ' ') + " - fusion: #" + fusion_node->fusion_expr_->ToString();
    result->emplace_back(std::make_shared<std::string>(table_index));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = fusion_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in fusion node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += fusion_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += fusion_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalMergeAggregate *merge_aggregate_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> Merge aggregate ";
    } else {
        explain_header_str = "Merge aggregate ";
    }
    explain_header_str += "(" + std::to_string(merge_aggregate_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = merge_aggregate_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in merge aggregate node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += merge_aggregate_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += merge_aggregate_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalReadCache *read_cache_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string explain_header_str;
    if (intent_size != 0) {
        explain_header_str = std::string(intent_size - 2, ' ') + "-> Read cache ";
    } else {
        explain_header_str = "Read cache ";
    }
    explain_header_str += "(" + std::to_string(read_cache_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(explain_header_str));

    const BaseTableRef *base_table_ref = read_cache_node->base_table_ref();
    // Table alias and name
    std::string table_name = std::string(intent_size, ' ') + " - table name: (";
    table_name += *base_table_ref->db_name() + ".";
    table_name += *base_table_ref->table_name() + ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ') + " - output columns: [";
    size_t column_count = read_cache_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError("No column in read cache node.");
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += read_cache_node->GetOutputNames()->at(idx) + ", ";
    }
    output_columns += read_cache_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
}

void ExplainPhysicalPlan::Explain(const PhysicalUnnest *unnest_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string unnest_node_header_str;
    if (intent_size != 0) {
        unnest_node_header_str = std::string(intent_size - 2, ' ') + "-> Unnest ";
    } else {
        unnest_node_header_str = "\"-> Unnest \" ";
    }

    unnest_node_header_str += "(" + std::to_string(unnest_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(unnest_node_header_str));

    // Unnest expression
    {
        std::string unnest_expression_str = std::string(intent_size, ' ') + " - unnest expression: [";
        std::vector<std::shared_ptr<BaseExpression>> expression_list = unnest_node->expression_list();
        size_t expression_count = expression_list.size();
        for (size_t idx = 0; idx < expression_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(expression_list[idx].get(), unnest_expression_str);
            unnest_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(expression_list.back().get(), unnest_expression_str);
        unnest_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(unnest_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = unnest_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalUnnestAggregate *unnest_aggregate_node,
                                  std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                  i64 intent_size) {
    std::string unnest_node_header_str;
    if (intent_size != 0) {
        unnest_node_header_str = std::string(intent_size - 2, ' ') + "-> Unnest ";
    } else {
        unnest_node_header_str = "\"-> Unnest \" ";
    }

    unnest_node_header_str += "(" + std::to_string(unnest_aggregate_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(unnest_node_header_str));

    // Unnest expression
    {
        std::string unnest_expression_str = std::string(intent_size, ' ') + " - unnest expression: [";
        std::vector<std::shared_ptr<BaseExpression>> expression_list = unnest_aggregate_node->expression_list();
        size_t expression_count = expression_list.size();
        for (size_t idx = 0; idx < expression_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(expression_list[idx].get(), unnest_expression_str);
            unnest_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(expression_list.back().get(), unnest_expression_str);
        unnest_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(unnest_expression_str));
    }

    size_t groups_count = unnest_aggregate_node->groups_.size();
    size_t aggregates_count = unnest_aggregate_node->aggregates_.size();

    //    // Aggregate Table index
    //    {
    //        std::string aggregate_table_index =
    //            std::string(intent_size, ' ') + " - aggregate table index: #" + std::to_string(unnest_aggregate_node->AggregateTableIndex());
    //        result->emplace_back(std::make_shared<std::string>(aggregate_table_index));
    //    }

    // Aggregate expressions
    {
        std::string aggregate_expression_str = std::string(intent_size, ' ') + " - aggregate: [";
        if (aggregates_count != 0) {
            for (size_t idx = 0; idx < aggregates_count - 1; ++idx) {
                ExplainLogicalPlan::Explain(unnest_aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            ExplainLogicalPlan::Explain(unnest_aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        //        std::string group_table_index = std::string(intent_size, ' ') + " - group by table index: #" +
        //        std::to_string(unnest_aggregate_node->GroupTableIndex()); result->emplace_back(std::make_shared<std::string>(group_table_index));

        std::string group_by_expression_str = std::string(intent_size, ' ') + " - group by: [";
        for (size_t idx = 0; idx < groups_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(unnest_aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(unnest_aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(group_by_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = unnest_aggregate_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
}

void ExplainPhysicalPlan::Explain(const PhysicalCheck *check_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (check_node->check_type()) {
        case CheckStmtType::kSystem: {
            std::string check_str;
            if (intent_size != 0) {
                check_str = std::string(intent_size - 2, ' ');
                check_str += "-> CHECK SYSTEM ";
            } else {
                check_str = "CHECK SYSTEM ";
            }
            check_str += "(";
            check_str += std::to_string(check_node->node_id());
            check_str += ")";
            result->emplace_back(std::make_shared<std::string>(check_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case CheckStmtType::kTable: {
            std::string check_str;
            if (intent_size != 0) {
                check_str = std::string(intent_size - 2, ' ');
                check_str += "-> CHECK TABLE ";
            } else {
                check_str = "CHECK TABLE ";
            }
            check_str += "(";
            check_str += std::to_string(check_node->node_id());
            check_str += ")";
            result->emplace_back(std::make_shared<std::string>(check_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid check type");
        }
    }
}

} // namespace infinity
