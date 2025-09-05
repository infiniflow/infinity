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

module infinity_core:physical_planner.impl;

import :physical_planner;
import :physical_operator;
import :physical_operator_type;
import :physical_aggregate;
import :physical_alter;
import :physical_create_collection;
import :physical_create_schema;
import :physical_create_table;
import :physical_create_view;
import :physical_cross_product;
import :physical_delete;
import :physical_drop_collection;
import :physical_drop_schema;
import :physical_drop_table;
import :physical_drop_view;
import :physical_dummy_operator;
import :physical_dummy_scan;
import :physical_except;
import :physical_explain;
import :physical_export;
import :physical_filter;
import :physical_flush;
import :physical_optimize;
import :physical_hash;
import :physical_hash_join;
import :physical_index_join;
import :physical_import;
import :physical_index_scan;
import :physical_insert;
import :physical_intersect;
import :physical_knn_scan;
import :physical_limit;
import :physical_merge_hash;
import :physical_merge_knn;
import :physical_merge_limit;
import :physical_aggregate;
import :physical_merge_aggregate;
import :physical_merge_parallel_aggregate;
import :physical_merge_sort;
import :physical_merge_top;
import :physical_merge_match_tensor;
import :physical_merge_match_sparse;
import :physical_merge_match_sparse;
import :physical_nested_loop_join;
import :physical_parallel_aggregate;
import :physical_prepared_plan;
import :physical_project;
import :physical_show;
import :physical_sink;
import :physical_sort;
import :physical_source;
import :physical_table_scan;
import :physical_index_scan;
import :physical_top;
import :physical_union_all;
import :physical_update;
import :physical_drop_index;
import :physical_command;
import :physical_compact;
import :physical_match;
import :physical_match_tensor_scan;
import :physical_match_sparse_scan;
import :physical_fusion;
import :physical_create_index_prepare;
import :physical_read_cache;
import :physical_unnest;
import :physical_unnest_aggregate;
import :physical_check;
import :logical_node;
import :logical_node_type;
import :logical_alter;
import :logical_create_schema;
import :logical_create_view;
import :logical_create_table;
import :logical_create_collection;
import :logical_create_index;
import :logical_drop_schema;
import :logical_drop_table;
import :logical_drop_collection;
import :logical_drop_view;
import :logical_flush;
import :logical_optimize;
import :logical_insert;
import :logical_delete;
import :logical_update;
import :logical_project;
import :logical_filter;
import :logical_table_scan;
import :logical_index_scan;
import :logical_knn_scan;
import :logical_aggregate;
import :logical_sort;
import :logical_limit;
import :logical_top;
import :logical_cross_product;
import :logical_join;
import :logical_show;
import :logical_export;
import :logical_import;
import :logical_dummy_scan;
import :logical_explain;
import :logical_drop_index;
import :logical_command;
import :logical_compact;
import :logical_match;
import :logical_match_tensor_scan;
import :logical_match_sparse_scan;
import :logical_fusion;
import :logical_read_cache;
import :logical_unnest;
import :logical_unnest_aggregate;
import :logical_check;
import :value;
import :value_expression;
import :match_tensor_expression;
import :match_sparse_expression;
import :explain_physical_plan;
import :status;
import :infinity_exception;
import :load_meta;
import :block_index;
import :logger;

import third_party;

import create_index_info;
import command_statement;
import explain_statement;
import alter_statement;

namespace infinity {

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildPhysicalOperator(const std::shared_ptr<LogicalNode> &logical_operator) const {

    std::unique_ptr<PhysicalOperator> result{nullptr};
    switch (logical_operator->operator_type()) {
        // DDL
        case LogicalNodeType::kCreateTable: {
            result = BuildCreateTable(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateIndex: {
            result = BuildCreateIndex(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            result = BuildCreateCollection(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            result = BuildCreateDatabase(logical_operator);
            break;
        }
        case LogicalNodeType::kDropTable: {
            result = BuildDropTable(logical_operator);
            break;
        }
        case LogicalNodeType::kDropIndex: {
            result = BuildDropIndex(logical_operator);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            result = BuildDropCollection(logical_operator);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            result = BuildDropSchema(logical_operator);
            break;
        }
        case LogicalNodeType::kCreateView: {
            result = BuildCreateView(logical_operator);
            break;
        }
        case LogicalNodeType::kDropView: {
            result = BuildDropView(logical_operator);
            break;
        }
        case LogicalNodeType::kAlter: {
            result = BuildAlter(logical_operator);
            break;
        }

        // DML
        case LogicalNodeType::kInsert: {
            result = BuildInsert(logical_operator);
            break;
        }
        case LogicalNodeType::kDelete: {
            result = BuildDelete(logical_operator);
            break;
        }
        case LogicalNodeType::kUpdate: {
            result = BuildUpdate(logical_operator);
            break;
        }
        case LogicalNodeType::kImport: {
            result = BuildImport(logical_operator);
            break;
        }
        case LogicalNodeType::kExport: {
            result = BuildExport(logical_operator);
            break;
        }

        // Scan
        case LogicalNodeType::kShow: {
            result = BuildShow(logical_operator);
            break;
        }
        case LogicalNodeType::kMatch: {
            result = BuildMatch(logical_operator);
            break;
        }
        case LogicalNodeType::kMatchTensorScan: {
            result = BuildMatchTensorScan(logical_operator);
            break;
        }
        case LogicalNodeType::kMatchSparseScan: {
            result = BuildMatchSparseScan(logical_operator);
            break;
        }
        case LogicalNodeType::kFusion: {
            result = BuildFusion(logical_operator);
            break;
        }
        case LogicalNodeType::kTableScan: {
            result = BuildTableScan(logical_operator);
            break;
        }
        case LogicalNodeType::kIndexScan: {
            result = BuildIndexScan(logical_operator);
            break;
        }
        case LogicalNodeType::kViewScan: {
            break;
        }
        case LogicalNodeType::kDummyScan: {
            result = BuildDummyScan(logical_operator);
            break;
        }

        // SELECT
        case LogicalNodeType::kAggregate: {
            result = BuildAggregate(logical_operator);
            break;
        }
        case LogicalNodeType::kJoin: {
            result = BuildJoin(logical_operator);
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            result = BuildCrossProduct(logical_operator);
            break;
        }
        case LogicalNodeType::kSort: {
            result = BuildSort(logical_operator);
            break;
        }
        case LogicalNodeType::kLimit: {
            result = BuildLimit(logical_operator);
            break;
        }
        case LogicalNodeType::kTop: {
            result = BuildTop(logical_operator);
            break;
        }
        case LogicalNodeType::kFilter: {
            result = BuildFilter(logical_operator);
            break;
        }
        case LogicalNodeType::kProjection: {
            result = BuildProjection(logical_operator);
            break;
        }
        case LogicalNodeType::kUnion: {
            result = BuildUnion(logical_operator);
            break;
        }
        case LogicalNodeType::kExcept: {
            result = BuildExcept(logical_operator);
            break;
        }
        case LogicalNodeType::kIntersect: {
            result = BuildIntersect(logical_operator);
            break;
        }
        case LogicalNodeType::kFlush: {
            result = BuildFlush(logical_operator);
            break;
        }
        case LogicalNodeType::kOptimize: {
            result = BuildOptimize(logical_operator);
            break;
        }
        case LogicalNodeType::kKnnScan: {
            result = BuildKnn(logical_operator);
            break;
        }
        case LogicalNodeType::kCommand: {
            result = BuildCommand(logical_operator);
            break;
        }
        case LogicalNodeType::kCompact: {
            result = BuildCompact(logical_operator);
            break;
        }
        case LogicalNodeType::kExplain: {
            result = BuildExplain(logical_operator);
            break;
        }
        case LogicalNodeType::kReadCache: {
            result = BuildReadCache(logical_operator);
            break;
        }
        case LogicalNodeType::kUnnest: {
            result = BuildUnnest(logical_operator);
            break;
        }
        case LogicalNodeType::kUnnestAggregate: {
            result = BuildUnnestAggregate(logical_operator);
            break;
        }
        case LogicalNodeType::kCheck: {
            result = BuildCheck(logical_operator);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unknown logical node type: {}", logical_operator->name()));
            //            result = std::make_shared<PhysicalDummyOperator>(numeric_limits<uint64_t>::max());
        }
    }

    //    if(logical_operator->node_id() > query_context_ptr_->max_node_id()) {
    //        query_context_ptr_->set_max_node_id(logical_operator->node_id());
    //    }
    // Initialize the physical plan node
    result->Init(query_context_ptr_);

    return result;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCreateTable(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCreateTable> logical_create_table = static_pointer_cast<LogicalCreateTable>(logical_operator);
    return std::make_unique<PhysicalCreateTable>(logical_create_table->schema_name(),
                                                 logical_create_table->table_definitions(),
                                                 logical_create_table->GetOutputNames(),
                                                 logical_create_table->GetOutputTypes(),
                                                 logical_create_table->conflict_type(),
                                                 logical_create_table->table_index(),
                                                 logical_operator->node_id(),
                                                 logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCreateIndex(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto logical_create_index = static_pointer_cast<LogicalCreateIndex>(logical_operator);
    std::shared_ptr<std::string> schema_name = logical_create_index->base_table_ref()->table_info_->db_name_;
    std::shared_ptr<std::string> table_name = logical_create_index->base_table_ref()->table_info_->table_name_;
    const auto &index_def_ptr = logical_create_index->index_definition();

    bool parallel = false;
    if (index_def_ptr->index_type_ == IndexType::kHnsw) {
        parallel = true;
    }

    return std::make_unique<PhysicalCreateIndexPrepare>(logical_create_index->node_id(),
                                                        logical_create_index->base_table_ref(),
                                                        logical_create_index->index_definition(),
                                                        logical_create_index->conflict_type(),
                                                        logical_create_index->GetOutputNames(),
                                                        logical_create_index->GetOutputTypes(),
                                                        logical_create_index->load_metas(),
                                                        parallel);
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCreateCollection(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCreateCollection> logical_create_collection = static_pointer_cast<LogicalCreateCollection>(logical_operator);
    return std::make_unique<PhysicalCreateCollection>(logical_create_collection->schema_name(),
                                                      logical_create_collection->collection_name(),
                                                      logical_create_collection->conflict_type(),
                                                      logical_create_collection->GetOutputNames(),
                                                      logical_create_collection->GetOutputTypes(),
                                                      logical_create_collection->table_index(),
                                                      logical_operator->node_id(),
                                                      logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCreateDatabase(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCreateSchema> logical_create_schema = static_pointer_cast<LogicalCreateSchema>(logical_operator);
    return std::make_unique<PhysicalCreateSchema>(logical_create_schema->schema_name(),
                                                  logical_create_schema->conflict_type(),
                                                  logical_create_schema->comment(),
                                                  logical_create_schema->GetOutputNames(),
                                                  logical_create_schema->GetOutputTypes(),
                                                  logical_create_schema->node_id(),
                                                  logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildPreparedPlan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_unique<PhysicalPreparedPlan>(logical_operator->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCreateView(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCreateView> logical_create_view = static_pointer_cast<LogicalCreateView>(logical_operator);
    return std::make_unique<PhysicalCreateView>(logical_operator->node_id(),
                                                logical_create_view->names_ptr(),
                                                logical_create_view->types_ptr(),
                                                logical_create_view->create_view_info(),
                                                logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDropTable(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropTable> logical_drop_table = static_pointer_cast<LogicalDropTable>(logical_operator);

    return std::make_unique<PhysicalDropTable>(logical_drop_table->schema_name(),
                                               logical_drop_table->table_name(),
                                               logical_drop_table->conflict_type(),
                                               logical_drop_table->GetOutputNames(),
                                               logical_drop_table->GetOutputTypes(),
                                               logical_drop_table->node_id(),
                                               logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDropIndex(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropIndex> logical_drop_index = static_pointer_cast<LogicalDropIndex>(logical_operator);
    return std::make_unique<PhysicalDropIndex>(logical_drop_index->schema_name(),
                                               logical_drop_index->table_name(),
                                               logical_drop_index->index_name(),
                                               logical_drop_index->conflict_type(),
                                               logical_drop_index->GetOutputNames(),
                                               logical_drop_index->GetOutputTypes(),
                                               logical_drop_index->node_id(),
                                               logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDropCollection(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropCollection> logical_drop_collection = static_pointer_cast<LogicalDropCollection>(logical_operator);
    return std::make_unique<PhysicalDropCollection>(logical_drop_collection->schema_name(),
                                                    logical_drop_collection->collection_name(),
                                                    logical_drop_collection->conflict_type(),
                                                    logical_drop_collection->node_id(),
                                                    logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDropSchema(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropSchema> logical_drop_schema = static_pointer_cast<LogicalDropSchema>(logical_operator);
    return std::make_unique<PhysicalDropSchema>(logical_drop_schema->schema_name(),
                                                logical_drop_schema->conflict_type(),
                                                logical_drop_schema->GetOutputNames(),
                                                logical_drop_schema->GetOutputTypes(),
                                                logical_drop_schema->node_id(),
                                                logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDropView(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDropView> logical_drop_view = static_pointer_cast<LogicalDropView>(logical_operator);
    return std::make_unique<PhysicalDropView>(logical_drop_view->schema_name(),
                                              logical_drop_view->view_name(),
                                              logical_drop_view->conflict_type(),
                                              logical_drop_view->GetOutputNames(),
                                              logical_drop_view->GetOutputTypes(),
                                              logical_drop_view->node_id(),
                                              logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildInsert(const std::shared_ptr<LogicalNode> &logical_operator) const {

    std::shared_ptr<LogicalInsert> logical_insert_ptr = dynamic_pointer_cast<LogicalInsert>(logical_operator);

    // Check if this is INSERT SELECT (has a child) or INSERT VALUES (no child)
    auto input_logical_node = logical_operator->left_node();
    std::unique_ptr<PhysicalOperator> input_physical_operator = nullptr;

    if (input_logical_node.get() != nullptr) {
        // INSERT SELECT case: build the child operator (SELECT)
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    return std::make_unique<PhysicalInsert>(logical_operator->node_id(),
                                            logical_insert_ptr->table_info(),
                                            logical_insert_ptr->table_index(),
                                            logical_insert_ptr->value_list(),
                                            std::move(input_physical_operator),
                                            logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDelete(const std::shared_ptr<LogicalNode> &logical_operator) const {
    if (logical_operator->left_node().get() == nullptr) {
        UnrecoverableError("Logical delete node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical delete node shouldn't have right child.");
    }
    std::shared_ptr<LogicalDelete> logical_delete = dynamic_pointer_cast<LogicalDelete>(logical_operator);
    auto input_logical_node = logical_operator->left_node();
    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);
    auto physical_delete = std::make_unique<PhysicalDelete>(logical_operator->node_id(),
                                                            std::move(input_physical_operator),
                                                            logical_delete->table_info_,
                                                            logical_operator->load_metas());
    return physical_delete;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildUpdate(const std::shared_ptr<LogicalNode> &logical_operator) const {
    if (logical_operator->left_node().get() == nullptr) {
        UnrecoverableError("Logical update node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical update node shouldn't have right child.");
    }
    std::shared_ptr<LogicalUpdate> logical_update = dynamic_pointer_cast<LogicalUpdate>(logical_operator);
    auto input_logical_node = logical_operator->left_node();
    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);
    auto physical_update = std::make_unique<PhysicalUpdate>(logical_operator->node_id(),
                                                            std::move(input_physical_operator),
                                                            logical_update->table_info_,
                                                            logical_update->update_columns_,
                                                            logical_update->all_columns_in_table_,
                                                            logical_update->final_result_columns_,
                                                            logical_operator->load_metas());
    return physical_update;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildImport(const std::shared_ptr<LogicalNode> &logical_operator) const {
    LogicalImport *logical_import = (LogicalImport *)(logical_operator.get());
    return std::make_unique<PhysicalImport>(logical_operator->node_id(),
                                            logical_import->table_info(),
                                            logical_import->file_path(),
                                            logical_import->header(),
                                            logical_import->delimiter(),
                                            logical_import->FileType(),
                                            logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildExport(const std::shared_ptr<LogicalNode> &logical_operator) const {
    LogicalExport *logical_export = (LogicalExport *)(logical_operator.get());
    return std::make_unique<PhysicalExport>(logical_export->node_id(),
                                            logical_export->table_info(),
                                            logical_export->schema_name(),
                                            logical_export->table_name(),
                                            logical_export->file_path(),
                                            logical_export->header(),
                                            logical_export->delimiter(),
                                            logical_export->FileType(),
                                            logical_export->offset(),
                                            logical_export->limit(),
                                            logical_export->row_limit(),
                                            logical_export->column_idx_array(),
                                            logical_export->block_index(),
                                            logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildAlter(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto *logical_alter = const_cast<LogicalAlter *>(static_cast<const LogicalAlter *>(logical_operator.get()));
    switch (logical_alter->type_) {
        case AlterStatementType::kRenameTable: {
            auto *logical_rename_table = static_cast<LogicalRenameTable *>(logical_alter);
            return std::make_unique<PhysicalRenameTable>(logical_rename_table->table_info_,
                                                         std::move(logical_rename_table->new_table_name_),
                                                         logical_operator->GetOutputNames(),
                                                         logical_operator->GetOutputTypes(),
                                                         logical_operator->node_id(),
                                                         logical_operator->load_metas());
        }
        case AlterStatementType::kAddColumns: {
            auto *logical_add_columns = static_cast<LogicalAddColumns *>(logical_alter);
            return std::make_unique<PhysicalAddColumns>(logical_add_columns->table_info_,
                                                        logical_add_columns->column_defs_,
                                                        logical_operator->GetOutputNames(),
                                                        logical_operator->GetOutputTypes(),
                                                        logical_operator->node_id(),
                                                        logical_operator->load_metas());
        }
        case AlterStatementType::kDropColumns: {
            auto *logical_drop_columns = static_cast<LogicalDropColumns *>(logical_alter);
            return std::make_unique<PhysicalDropColumns>(logical_drop_columns->table_info_,
                                                         logical_drop_columns->column_names_,
                                                         logical_operator->GetOutputNames(),
                                                         logical_operator->GetOutputTypes(),
                                                         logical_operator->node_id(),
                                                         logical_operator->load_metas());
        }
        default: {
            RecoverableError(Status::NotSupport("Alter statement isn't supported."));
        }
    }
    return {};
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildAggregate(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Aggregate project node shouldn't have right child.");
    }
    std::shared_ptr<LogicalAggregate> logical_aggregate = static_pointer_cast<LogicalAggregate>(logical_operator);
    std::unique_ptr<PhysicalOperator> input_physical_operator{};
    if (input_logical_node.get() != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    size_t tasklet_count = input_physical_operator->TaskletCount();

    auto physical_agg_op = std::make_unique<PhysicalAggregate>(logical_aggregate->node_id(),
                                                               std::move(input_physical_operator),
                                                               logical_aggregate->groups_,
                                                               logical_aggregate->groupby_index_,
                                                               logical_aggregate->aggregates_,
                                                               logical_aggregate->aggregate_index_,
                                                               logical_operator->load_metas());

    if (tasklet_count <= 1) {
        return physical_agg_op;
    } else {
        return std::make_unique<PhysicalMergeAggregate>(query_context_ptr_->GetNextNodeID(),
                                                        logical_aggregate->base_table_ref_,
                                                        std::move(physical_agg_op),
                                                        logical_aggregate->GetOutputNames(),
                                                        logical_aggregate->GetOutputTypes(),
                                                        std::make_shared<std::vector<LoadMeta>>());
    }
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildJoin(const std::shared_ptr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();

    if (left_node.get() == nullptr) {
        UnrecoverableError("Join node has no left child.");
    }
    if (right_node.get() == nullptr) {
        UnrecoverableError("Join node has no right child.");
    }

    std::shared_ptr<LogicalJoin> logical_join = static_pointer_cast<LogicalJoin>(logical_operator);

    std::unique_ptr<PhysicalOperator> left_physical_operator{};
    std::unique_ptr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return std::make_unique<PhysicalNestedLoopJoin>(logical_operator->node_id(),
                                                    logical_join->join_type_,
                                                    logical_join->conditions_,
                                                    std::move(left_physical_operator),
                                                    std::move(right_physical_operator),
                                                    logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCrossProduct(const std::shared_ptr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();

    if (left_node.get() == nullptr) {
        UnrecoverableError("Cross product node has no left child.");
    }
    if (right_node.get() == nullptr) {
        UnrecoverableError("Cross product node has no right child.");
    }

    std::shared_ptr<LogicalCrossProduct> logical_cross_product = static_pointer_cast<LogicalCrossProduct>(logical_operator);

    std::unique_ptr<PhysicalOperator> left_physical_operator{};
    std::unique_ptr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return std::make_unique<PhysicalCrossProduct>(logical_operator->node_id(),
                                                  std::move(left_physical_operator),
                                                  std::move(right_physical_operator),
                                                  logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildSort(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();

    if (input_logical_node.get() == nullptr) {
        UnrecoverableError("Sort node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Sort node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    std::shared_ptr<LogicalSort> logical_sort = static_pointer_cast<LogicalSort>(logical_operator);

    return std::make_unique<PhysicalSort>(logical_operator->node_id(),
                                          std::move(input_physical_operator),
                                          logical_sort->expressions_,
                                          logical_sort->order_by_types_,
                                          logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildLimit(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();

    if (input_logical_node.get() == nullptr) {
        UnrecoverableError("Limit node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Limit node shouldn't have right child.");
    }

    std::shared_ptr<LogicalLimit> logical_limit = static_pointer_cast<LogicalLimit>(logical_operator);
    std::unique_ptr<PhysicalOperator> input_physical_operator = BuildPhysicalOperator(input_logical_node);

    i64 offset = 0;
    if (logical_limit->offset_expression_.get() != nullptr) {
        offset = (static_pointer_cast<ValueExpression>(logical_limit->offset_expression_))->GetValue().value_.big_int;
    }

    if (input_physical_operator->TaskletCount() <= 1 or offset != 0) {
        return std::make_unique<PhysicalLimit>(logical_operator->node_id(),
                                               std::move(input_physical_operator),
                                               logical_limit->limit_expression_,
                                               logical_limit->offset_expression_,
                                               logical_operator->load_metas(),
                                               logical_limit->total_hits_count_flag_);
    } else {
        auto child_limit_op = std::make_unique<PhysicalLimit>(logical_operator->node_id(),
                                                              std::move(input_physical_operator),
                                                              logical_limit->limit_expression_,
                                                              nullptr,
                                                              logical_operator->load_metas(),
                                                              logical_limit->total_hits_count_flag_);
        return std::make_unique<PhysicalMergeLimit>(query_context_ptr_->GetNextNodeID(),
                                                    std::move(child_limit_op),
                                                    logical_limit->base_table_ref_,
                                                    logical_limit->limit_expression_,
                                                    nullptr,
                                                    std::make_shared<std::vector<LoadMeta>>());
    }
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildTop(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto logical_operator_top = static_cast<LogicalTop *>(logical_operator.get());
    if (logical_operator_top->right_node()) {
        UnrecoverableError("Top node shouldn't have right child.");
    }
    auto &input_logical_node = logical_operator_top->left_node();
    if (!input_logical_node) {
        UnrecoverableError("Top node has no input node.");
    }
    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);
    i64 merge_offset{};
    i64 merge_limit = (static_pointer_cast<ValueExpression>(logical_operator_top->limit_expression_))->GetValue().value_.big_int;
    if (auto &offset = logical_operator_top->offset_expression_; offset.get() != nullptr) {
        merge_offset = (static_pointer_cast<ValueExpression>(offset))->GetValue().value_.big_int;
        merge_limit += merge_offset;
    }
    if (merge_offset < 0 or merge_limit <= merge_offset) {
        Status status = Status::SyntaxError("Limit <= 0 or offset < 0");
        RecoverableError(status);
    }
    if (merge_offset >= std::numeric_limits<u32>::max()) {
        Status status = Status::SyntaxError("Offset is too large");
        RecoverableError(status);
    }
    if (merge_limit >= std::numeric_limits<u32>::max()) {
        Status status = Status::SyntaxError("Limit is too large");
        RecoverableError(status);
    }
    if (input_physical_operator->TaskletCount() <= 1) {
        // only Top
        return std::make_unique<PhysicalTop>(logical_operator_top->node_id(),
                                             std::move(input_physical_operator),
                                             merge_limit,
                                             merge_offset, // start from offset
                                             logical_operator_top->sort_expressions_,
                                             logical_operator_top->order_by_types_,
                                             logical_operator_top->load_metas(),
                                             logical_operator_top->total_hits_count_flag_);
    } else {
        // need MergeTop
        auto child_top_op = std::make_unique<PhysicalTop>(logical_operator_top->node_id(),
                                                          std::move(input_physical_operator),
                                                          merge_limit,
                                                          u32{}, // start from 0
                                                          logical_operator_top->sort_expressions_,
                                                          logical_operator_top->order_by_types_,
                                                          logical_operator_top->load_metas(),
                                                          logical_operator_top->total_hits_count_flag_);
        return std::make_unique<PhysicalMergeTop>(query_context_ptr_->GetNextNodeID(),
                                                  logical_operator_top->base_table_ref_,
                                                  std::move(child_top_op),
                                                  merge_limit,
                                                  merge_offset, // start from offset
                                                  logical_operator_top->sort_expressions_,
                                                  logical_operator_top->order_by_types_,
                                                  std::make_shared<std::vector<LoadMeta>>());
    }
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildProjection(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical project node shouldn't have right child.");
    }
    std::shared_ptr<LogicalProject> logical_project = static_pointer_cast<LogicalProject>(logical_operator);
    std::unique_ptr<PhysicalOperator> input_physical_operator{};
    if (input_logical_node.get() != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }
    return std::make_unique<PhysicalProject>(logical_operator->node_id(),
                                             logical_project->table_index_,
                                             std::move(input_physical_operator),
                                             logical_project->expressions_,
                                             logical_operator->load_metas(),
                                             std::move(logical_project->highlight_columns_));
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildFilter(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (input_logical_node.get() == nullptr) {
        UnrecoverableError("Logical filter node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical filter node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    std::shared_ptr<LogicalFilter> logical_filter = static_pointer_cast<LogicalFilter>(logical_operator);

    return std::make_unique<PhysicalFilter>(logical_operator->node_id(),
                                            std::move(input_physical_operator),
                                            logical_filter->expression(),
                                            logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildIntersect(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_unique<PhysicalHashJoin>(logical_operator->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildUnion(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_unique<PhysicalUnionAll>(logical_operator->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildExcept(const std::shared_ptr<LogicalNode> &logical_operator) const {
    return std::make_unique<PhysicalHashJoin>(logical_operator->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildShow(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalShow> logical_show = static_pointer_cast<LogicalShow>(logical_operator);
    return std::make_unique<PhysicalShow>(logical_show->node_id(),
                                          logical_show->show_type(),
                                          logical_show->schema_name(),
                                          logical_show->object_name(),
                                          logical_show->table_index(),
                                          logical_show->file_path(),
                                          logical_show->segment_id(),
                                          logical_show->block_id(),
                                          logical_show->chunk_id(),
                                          logical_show->column_id(),
                                          logical_show->index_name(),
                                          logical_show->session_id(),
                                          logical_show->transaction_id(),
                                          logical_show->function_name(),
                                          logical_show->show_nullable(),
                                          logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildTableScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalTableScan> logical_table_scan = static_pointer_cast<LogicalTableScan>(logical_operator);
    return std::make_unique<PhysicalTableScan>(logical_operator->node_id(),
                                               logical_table_scan->base_table_ref_,
                                               std::move(logical_table_scan->fast_rough_filter_evaluator_),
                                               logical_operator->load_metas(),
                                               logical_table_scan->add_row_id_);
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildIndexScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalIndexScan> logical_index_scan = static_pointer_cast<LogicalIndexScan>(logical_operator);
    return std::make_unique<PhysicalIndexScan>(logical_operator->node_id(),
                                               logical_index_scan->base_table_ref_,
                                               logical_index_scan->index_filter_,
                                               std::move(logical_index_scan->index_filter_evaluator_),
                                               std::move(logical_index_scan->fast_rough_filter_evaluator_),
                                               logical_operator->load_metas(),
                                               logical_operator->GetOutputNames(),
                                               logical_operator->GetOutputTypes(),
                                               logical_index_scan->add_row_id_,
                                               true);
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildViewScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    Status status = Status::NotSupport("BuildViewScan");
    RecoverableError(status);
    return nullptr;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildDummyScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalDummyScan> logical_show = static_pointer_cast<LogicalDummyScan>(logical_operator);
    return std::make_unique<PhysicalDummyScan>(logical_show->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildFlush(const std::shared_ptr<LogicalNode> &logical_operator) const {
    LogicalFlush *logical_flush = (LogicalFlush *)(logical_operator.get());
    return std::make_unique<PhysicalFlush>(logical_flush->flush_type(), logical_flush->node_id(), logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildOptimize(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalOptimize> logical_optimize = static_pointer_cast<LogicalOptimize>(logical_operator);
    return std::make_unique<PhysicalOptimize>(logical_optimize->node_id(),
                                              logical_optimize->schema_name(),
                                              logical_optimize->object_name(),
                                              logical_optimize->index_name_,
                                              std::move(logical_optimize->opt_params_),
                                              logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildMatch(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalMatch> logical_match = static_pointer_cast<LogicalMatch>(logical_operator);
    return std::make_unique<PhysicalMatch>(logical_match->node_id(),
                                           logical_match->base_table_ref_,
                                           logical_match->match_expr_,
                                           logical_match->index_reader_,
                                           std::move(logical_match->query_tree_),
                                           logical_match->begin_threshold_,
                                           logical_match->early_term_algo_,
                                           logical_match->top_n_,
                                           logical_match->common_query_filter_,
                                           std::move(logical_match->minimum_should_match_option_),
                                           std::move(logical_match->rank_features_option_),
                                           logical_match->score_threshold_,
                                           logical_match->ft_similarity_,
                                           logical_match->bm25_params_,
                                           logical_match->TableIndex(),
                                           logical_operator->load_metas(),
                                           true /*cache_result*/);
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildMatchTensorScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    const auto logical_match_tensor = static_pointer_cast<LogicalMatchTensorScan>(logical_operator);
    auto match_tensor_scan_op =
        std::make_unique<PhysicalMatchTensorScan>(logical_match_tensor->node_id(),
                                                  logical_match_tensor->TableIndex(),
                                                  logical_match_tensor->base_table_ref_,
                                                  std::static_pointer_cast<MatchTensorExpression>(logical_match_tensor->query_expression_),
                                                  logical_match_tensor->common_query_filter_,
                                                  logical_match_tensor->topn_,
                                                  logical_match_tensor->knn_threshold_,
                                                  logical_match_tensor->index_options_,
                                                  logical_operator->load_metas());
    match_tensor_scan_op->CheckColumn();
    match_tensor_scan_op->PlanWithIndex(query_context_ptr_);
    if (match_tensor_scan_op->TaskletCount() <= 1) {
        match_tensor_scan_op->SetCacheResult(true);
        return match_tensor_scan_op;
    } else {
        return std::make_unique<PhysicalMergeMatchTensor>(query_context_ptr_->GetNextNodeID(),
                                                          std::move(match_tensor_scan_op),
                                                          logical_match_tensor->TableIndex(),
                                                          logical_match_tensor->base_table_ref_,
                                                          std::static_pointer_cast<MatchTensorExpression>(logical_match_tensor->query_expression_),
                                                          logical_match_tensor->common_query_filter_->original_filter_,
                                                          logical_match_tensor->topn_,
                                                          logical_match_tensor->index_options_,
                                                          std::make_shared<std::vector<LoadMeta>>(),
                                                          true);
    }
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildMatchSparseScan(const std::shared_ptr<LogicalNode> &logical_operator) const {
    const auto logical_match_sparse = std::static_pointer_cast<LogicalMatchSparseScan>(logical_operator);
    auto match_sparse_scan_op =
        std::make_unique<PhysicalMatchSparseScan>(logical_match_sparse->node_id(),
                                                  logical_match_sparse->TableIndex(),
                                                  logical_match_sparse->base_table_ref_,
                                                  std::static_pointer_cast<MatchSparseExpression>(logical_match_sparse->query_expression_),
                                                  logical_match_sparse->common_query_filter_,
                                                  logical_operator->load_metas());
    match_sparse_scan_op->PlanWithIndex(query_context_ptr_);
    if (match_sparse_scan_op->TaskletCount() <= 1) {
        match_sparse_scan_op->SetCacheResult(true);
        return match_sparse_scan_op;
    }
    auto merge_match_sparse_op =
        std::make_unique<PhysicalMergeMatchSparse>(query_context_ptr_->GetNextNodeID(),
                                                   std::move(match_sparse_scan_op),
                                                   logical_match_sparse->TableIndex(),
                                                   logical_match_sparse->base_table_ref_,
                                                   std::static_pointer_cast<MatchSparseExpression>(logical_match_sparse->query_expression_),
                                                   logical_match_sparse->common_query_filter_->original_filter_,
                                                   std::make_shared<std::vector<LoadMeta>>(),
                                                   true);
    return merge_match_sparse_op;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildFusion(const std::shared_ptr<LogicalNode> &logical_operator) const {
    const auto logical_fusion = static_pointer_cast<LogicalFusion>(logical_operator);
    std::unique_ptr<PhysicalOperator> left_phy = nullptr, right_phy = nullptr;
    std::vector<std::unique_ptr<PhysicalOperator>> other_children;
    if (const auto &left_logical_node = logical_operator->left_node(); left_logical_node.get() != nullptr) {
        left_phy = BuildPhysicalOperator(left_logical_node);
    }
    if (const auto right_logical_node = logical_operator->right_node(); right_logical_node.get() != nullptr) {
        right_phy = BuildPhysicalOperator(right_logical_node);
    }
    size_t num_other_children = logical_fusion->other_children_.size();
    for (size_t i = 0; i < num_other_children; i++) {
        std::unique_ptr<PhysicalOperator> child_phy = BuildPhysicalOperator(logical_fusion->other_children_[i]);
        other_children.push_back(std::move(child_phy));
    }
    return std::make_unique<PhysicalFusion>(logical_fusion->node_id(),
                                            logical_fusion->base_table_ref_,
                                            std::move(left_phy),
                                            std::move(right_phy),
                                            std::move(other_children),
                                            logical_fusion->fusion_expr_,
                                            logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildKnn(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto *logical_knn_scan = (LogicalKnnScan *)(logical_operator.get());
    std::unique_ptr<PhysicalKnnScan> knn_scan_op = std::make_unique<PhysicalKnnScan>(logical_knn_scan->node_id(),
                                                                                     logical_knn_scan->base_table_ref_,
                                                                                     logical_knn_scan->knn_expression(),
                                                                                     logical_knn_scan->common_query_filter_,
                                                                                     logical_knn_scan->GetOutputNames(),
                                                                                     logical_knn_scan->GetOutputTypes(),
                                                                                     logical_knn_scan->knn_table_index_,
                                                                                     logical_operator->load_metas());

    knn_scan_op->PlanWithIndex(query_context_ptr_);
    if (knn_scan_op->TaskletCount() == 1) {
        knn_scan_op->SetCacheResult(true);
        return knn_scan_op;
    } else {
        return std::make_unique<PhysicalMergeKnn>(query_context_ptr_->GetNextNodeID(),
                                                  logical_knn_scan->base_table_ref_,
                                                  std::move(knn_scan_op),
                                                  logical_knn_scan->GetOutputNames(),
                                                  logical_knn_scan->GetOutputTypes(),
                                                  logical_knn_scan->knn_expression(),
                                                  logical_knn_scan->common_query_filter_->original_filter_,
                                                  logical_knn_scan->knn_table_index_,
                                                  std::make_shared<std::vector<LoadMeta>>(),
                                                  true);
    }
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCommand(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto *logical_command = (LogicalCommand *)(logical_operator.get());
    auto command_info = logical_command->command_info();
    return std::make_unique<PhysicalCommand>(logical_command->node_id(),
                                             command_info,
                                             logical_command->GetOutputNames(),
                                             logical_command->GetOutputTypes(),
                                             logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCompact(const std::shared_ptr<LogicalNode> &logical_operator) const {
    const auto *logical_compact = static_cast<const LogicalCompact *>(logical_operator.get());
    if (logical_compact->left_node().get() != nullptr || logical_compact->right_node().get() != nullptr) {
        UnrecoverableError("Compact node shouldn't have child.");
    }
    return std::make_unique<PhysicalCompact>(logical_compact->node_id(),
                                             logical_compact->base_table_ref_,
                                             logical_compact->compact_type_,
                                             logical_compact->GetOutputNames(),
                                             logical_compact->GetOutputTypes(),
                                             logical_operator->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildReadCache(const std::shared_ptr<LogicalNode> &logical_operator) const {
    const auto *logical_read_cache = static_cast<LogicalReadCache *>(logical_operator.get());
    return std::make_unique<PhysicalReadCache>(logical_read_cache->node_id(),
                                               logical_read_cache->origin_type_,
                                               logical_read_cache->base_table_ref_,
                                               logical_read_cache->cache_content_,
                                               logical_read_cache->column_map_,
                                               logical_read_cache->load_metas(),
                                               logical_read_cache->is_min_heap_);
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildUnnest(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (input_logical_node.get() == nullptr) {
        UnrecoverableError("Logical filter node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical filter node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    auto *logical_unnest = static_cast<LogicalUnnest *>(logical_operator.get());
    return std::make_unique<PhysicalUnnest>(logical_unnest->node_id(),
                                            std::move(input_physical_operator),
                                            logical_unnest->expression_list(),
                                            logical_unnest->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildUnnestAggregate(const std::shared_ptr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (input_logical_node.get() == nullptr) {
        UnrecoverableError("Logical filter node has no input node.");
    }
    if (logical_operator->right_node().get() != nullptr) {
        UnrecoverableError("Logical filter node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    auto *logical_unnest_aggregate = static_cast<LogicalUnnestAggregate *>(logical_operator.get());
    return std::make_unique<PhysicalUnnestAggregate>(logical_unnest_aggregate->node_id(),
                                                     std::move(input_physical_operator),
                                                     logical_unnest_aggregate->groups(),
                                                     logical_unnest_aggregate->group_by_index(),
                                                     logical_unnest_aggregate->aggregates(),
                                                     logical_unnest_aggregate->aggregate_index(),
                                                     logical_unnest_aggregate->unnest_expression_list(),
                                                     logical_unnest_aggregate->load_metas());
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildExplain(const std::shared_ptr<LogicalNode> &logical_operator) const {

    auto input_logical_node = logical_operator->left_node();
    std::unique_ptr<PhysicalOperator> input_physical_operator{nullptr};
    if (input_logical_node.get() != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    std::shared_ptr<LogicalExplain> logical_explain = static_pointer_cast<LogicalExplain>(logical_operator);

    std::unique_ptr<PhysicalExplain> explain_node{nullptr};
    switch (logical_explain->explain_type()) {
        case ExplainType::kAst:
        case ExplainType::kUnOpt:
        case ExplainType::kOpt: {
            explain_node = std::make_unique<PhysicalExplain>(logical_explain->node_id(),
                                                             logical_explain->explain_type(),
                                                             logical_explain->TextArray(),
                                                             nullptr,
                                                             logical_operator->load_metas());
            break;
        }
        case ExplainType::kPhysical: {
            std::shared_ptr<std::vector<std::shared_ptr<std::string>>> texts_ptr = std::make_shared<std::vector<std::shared_ptr<std::string>>>();
            ExplainPhysicalPlan::Explain(input_physical_operator.get(), texts_ptr);
            explain_node = std::make_unique<PhysicalExplain>(logical_explain->node_id(),
                                                             logical_explain->explain_type(),
                                                             texts_ptr,
                                                             nullptr,
                                                             logical_operator->load_metas());
            break;
        }
        case ExplainType::kAnalyze:
        case ExplainType::kFragment:
        case ExplainType::kPipeline: {
            explain_node = std::make_unique<PhysicalExplain>(logical_explain->node_id(),
                                                             logical_explain->explain_type(),
                                                             nullptr,
                                                             std::move(input_physical_operator),
                                                             logical_operator->load_metas());
            break;
        }
        case ExplainType::kInvalid: {
            UnrecoverableError("Invalid explain type");
            break;
        }
    }

    return explain_node;
}

std::unique_ptr<PhysicalOperator> PhysicalPlanner::BuildCheck(const std::shared_ptr<LogicalNode> &logical_operator) const {
    std::shared_ptr<LogicalCheck> logical_check = static_pointer_cast<LogicalCheck>(logical_operator);
    return std::make_unique<PhysicalCheck>(logical_check->node_id(),
                                           logical_check->check_type(),
                                           logical_check->schema_name(),
                                           logical_check->table_name(),
                                           logical_operator->load_metas());
}

} // namespace infinity
