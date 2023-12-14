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

#include <memory>

import stl;
import physical_operator;
import physical_operator_type;
import physical_aggregate;
import physical_alter;
import physical_create_collection;
import physical_create_index;
import physical_create_schema;
import physical_create_table;
import physical_create_view;
import physical_cross_product;
import physical_delete;
import physical_drop_collection;
import physical_drop_schema;
import physical_drop_table;
import physical_drop_view;
import physical_dummy_operator;
import physical_dummy_scan;
import physical_except;
import physical_explain;
import physical_export;
import physical_filter;
import physical_flush;
import physical_optimize;
import physical_hash;
import physical_hash_join;
import physical_index_join;
import physical_import;
import physical_index_scan;
import physical_insert;
import physical_intersect;
import physical_knn_scan;
import physical_limit;
import physical_merge_hash;
import physical_merge_join;
import physical_merge_knn;
import physical_merge_limit;
import physical_merge_parallel_aggregate;
import physical_merge_sort;
import physical_merge_top;
import physical_nested_loop_join;
import physical_parallel_aggregate;
import physical_planner;
import physical_prepared_plan;
import physical_project;
import physical_show;
import physical_sink;
import physical_sort;
import physical_source;
import physical_table_scan;
import physical_top;
import physical_union_all;
import physical_update;
import physical_drop_index;
import physical_command;
import physical_match;
import physical_fusion;

import logical_node;
import logical_node_type;
import logical_create_schema;
import logical_create_view;
import logical_create_table;
import logical_create_collection;
import logical_create_index;
import logical_drop_schema;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_view;
import logical_flush;
import logical_optimize;
import logical_insert;
import logical_delete;
import logical_update;
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
import logical_dummy_scan;
import logical_explain;
import logical_drop_index;
import logical_command;
import logical_match;
import logical_fusion;

import parser;
import explain_physical_plan;

import infinity_exception;

module physical_planner;

namespace infinity {

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildPhysicalOperator(const SharedPtr<LogicalNode> &logical_operator) const {

    UniquePtr<PhysicalOperator> result{nullptr};
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
            result = BuildCreateSchema(logical_operator);
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
        case LogicalNodeType::kFusion: {
            result = BuildFusion(logical_operator);
            break;
        }
        case LogicalNodeType::kTableScan: {
            result = BuildTableScan(logical_operator);
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
        case LogicalNodeType::kExplain: {
            result = BuildExplain(logical_operator);
            break;
        }
        default: {
            Error<PlannerException>("Unknown logical node type: " + logical_operator->name());
            //            result = MakeShared<PhysicalDummyOperator>(numeric_limits<uint64_t>::max());
        }
    }

    //    if(logical_operator->node_id() > query_context_ptr_->max_node_id()) {
    //        query_context_ptr_->set_max_node_id(logical_operator->node_id());
    //    }
    // Initialize the physical plan node
    result->Init();

    return result;
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCreateTable(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateTable> logical_create_table = static_pointer_cast<LogicalCreateTable>(logical_operator);
    return MakeUnique<PhysicalCreateTable>(logical_create_table->schema_name(),
                                           logical_create_table->table_definitions(),
                                           logical_create_table->GetOutputNames(),
                                           logical_create_table->GetOutputTypes(),
                                           logical_create_table->conflict_type(),
                                           logical_create_table->table_index(),
                                           logical_operator->node_id(),
                                           logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCreateIndex(const SharedPtr<LogicalNode> &logical_operator) const {
    auto logical_create_index = static_pointer_cast<LogicalCreateIndex>(logical_operator);
    return PhysicalCreateIndex::Make(logical_create_index->schema_name(),
                                     logical_create_index->table_name(),
                                     logical_create_index->index_definition(),
                                     logical_create_index->conflict_type(),
                                     logical_create_index->GetOutputNames(),
                                     logical_create_index->GetOutputTypes(),
                                     logical_create_index->node_id(),
                                     logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCreateCollection(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateCollection> logical_create_collection = static_pointer_cast<LogicalCreateCollection>(logical_operator);
    return MakeUnique<PhysicalCreateCollection>(logical_create_collection->schema_name(),
                                                logical_create_collection->collection_name(),
                                                logical_create_collection->conflict_type(),
                                                logical_create_collection->GetOutputNames(),
                                                logical_create_collection->GetOutputTypes(),
                                                logical_create_collection->table_index(),
                                                logical_operator->node_id(),
                                                logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCreateSchema(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateSchema> logical_create_schema = static_pointer_cast<LogicalCreateSchema>(logical_operator);
    return MakeUnique<PhysicalCreateSchema>(logical_create_schema->schema_name(),
                                            logical_create_schema->conflict_type(),
                                            logical_create_schema->GetOutputNames(),
                                            logical_create_schema->GetOutputTypes(),
                                            logical_create_schema->node_id(),
                                            logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildPreparedPlan(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeUnique<PhysicalPreparedPlan>(logical_operator->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCreateView(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalCreateView> logical_create_view = static_pointer_cast<LogicalCreateView>(logical_operator);
    return MakeUnique<PhysicalCreateView>(logical_operator->node_id(),
                                          logical_create_view->names_ptr(),
                                          logical_create_view->types_ptr(),
                                          logical_create_view->create_view_info(),
                                          logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDropTable(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropTable> logical_drop_table = static_pointer_cast<LogicalDropTable>(logical_operator);

    return MakeUnique<PhysicalDropTable>(logical_drop_table->schema_name(),
                                         logical_drop_table->table_name(),
                                         logical_drop_table->conflict_type(),
                                         logical_drop_table->GetOutputNames(),
                                         logical_drop_table->GetOutputTypes(),
                                         logical_drop_table->node_id(),
                                         logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDropIndex(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropIndex> logical_drop_index = static_pointer_cast<LogicalDropIndex>(logical_operator);
    return MakeUnique<PhysicalDropIndex>(logical_drop_index->schema_name(),
                                         logical_drop_index->table_name(),
                                         logical_drop_index->index_name(),
                                         logical_drop_index->conflict_type(),
                                         logical_drop_index->GetOutputNames(),
                                         logical_drop_index->GetOutputTypes(),
                                         logical_drop_index->node_id(),
                                         logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDropCollection(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropCollection> logical_drop_collection = static_pointer_cast<LogicalDropCollection>(logical_operator);
    return MakeUnique<PhysicalDropCollection>(logical_drop_collection->schema_name(),
                                              logical_drop_collection->collection_name(),
                                              logical_drop_collection->conflict_type(),
                                              logical_drop_collection->node_id(),
                                              logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDropSchema(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropSchema> logical_drop_schema = static_pointer_cast<LogicalDropSchema>(logical_operator);
    return MakeUnique<PhysicalDropSchema>(logical_drop_schema->schema_name(),
                                          logical_drop_schema->conflict_type(),
                                          logical_drop_schema->GetOutputNames(),
                                          logical_drop_schema->GetOutputTypes(),
                                          logical_drop_schema->node_id(),
                                          logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDropView(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDropView> logical_drop_view = static_pointer_cast<LogicalDropView>(logical_operator);
    return MakeUnique<PhysicalDropView>(logical_drop_view->schema_name(),
                                        logical_drop_view->view_name(),
                                        logical_drop_view->conflict_type(),
                                        logical_drop_view->GetOutputNames(),
                                        logical_drop_view->GetOutputTypes(),
                                        logical_drop_view->node_id(),
                                        logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildInsert(const SharedPtr<LogicalNode> &logical_operator) const {

    SharedPtr<LogicalInsert> logical_insert_ptr = dynamic_pointer_cast<LogicalInsert>(logical_operator);
    return MakeUnique<PhysicalInsert>(logical_operator->node_id(),
                                      logical_insert_ptr->table_collection_entry(),
                                      logical_insert_ptr->table_index(),
                                      logical_insert_ptr->value_list(),
                                      logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDelete(const SharedPtr<LogicalNode> &logical_operator) const {
    if (logical_operator->left_node() == nullptr) {
        Error<PlannerException>("Logical delete node has no input node.");
    }
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Logical delete node shouldn't have right child.");
    }
    SharedPtr<LogicalDelete> logical_delete = dynamic_pointer_cast<LogicalDelete>(logical_operator);
    auto input_logical_node = logical_operator->left_node();
    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);
    auto physical_delete = MakeUnique<PhysicalDelete>(logical_operator->node_id(),
                                                      Move(input_physical_operator),
                                                      logical_delete->table_entry_ptr_,
                                                      logical_operator->load_metas());
    return physical_delete;
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildUpdate(const SharedPtr<LogicalNode> &logical_operator) const {
    if (logical_operator->left_node() == nullptr) {
        Error<PlannerException>("Logical update node has no input node.");
    }
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Logical update node shouldn't have right child.");
    }
    SharedPtr<LogicalUpdate> logical_update = dynamic_pointer_cast<LogicalUpdate>(logical_operator);
    auto input_logical_node = logical_operator->left_node();
    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);
    auto physical_update = MakeUnique<PhysicalUpdate>(logical_operator->node_id(),
                                                      Move(input_physical_operator),
                                                      logical_update->table_entry_ptr_,
                                                      logical_update->update_columns_,
                                                      logical_operator->load_metas());
    return physical_update;
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildImport(const SharedPtr<LogicalNode> &logical_operator) const {
    LogicalImport *logical_import = (LogicalImport *)(logical_operator.get());
    return MakeUnique<PhysicalImport>(logical_operator->node_id(),
                                      logical_import->table_collection_entry(),
                                      logical_import->file_path(),
                                      logical_import->header(),
                                      logical_import->delimiter(),
                                      logical_import->FileType(),
                                      logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildExport(const SharedPtr<LogicalNode> &logical_operator) const {
    LogicalExport *logical_export = (LogicalExport *)(logical_operator.get());
    return MakeUnique<PhysicalExport>(logical_export->node_id(),
                                      logical_export->schema_name(),
                                      logical_export->table_name(),
                                      logical_export->file_path(),
                                      logical_export->header(),
                                      logical_export->delimiter(),
                                      logical_export->FileType(),
                                      logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildAlter(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeUnique<PhysicalAlter>(logical_operator->GetOutputNames(),
                                     logical_operator->GetOutputTypes(),
                                     logical_operator->node_id(),
                                     logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildAggregate(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Aggregate project node shouldn't have right child.");
    }
    SharedPtr<LogicalAggregate> logical_aggregate = static_pointer_cast<LogicalAggregate>(logical_operator);
    UniquePtr<PhysicalOperator> input_physical_operator{};
    if (input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    return MakeUnique<PhysicalAggregate>(logical_aggregate->node_id(),
                                         Move(input_physical_operator),
                                         logical_aggregate->groups_,
                                         logical_aggregate->groupby_index_,
                                         logical_aggregate->aggregates_,
                                         logical_aggregate->aggregate_index_,
                                         logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();

    if (left_node == nullptr) {
        Error<PlannerException>("Join node has no left child.");
    }
    if (right_node == nullptr) {
        Error<PlannerException>("Join node has no right child.");
    }

    SharedPtr<LogicalJoin> logical_join = static_pointer_cast<LogicalJoin>(logical_operator);

    UniquePtr<PhysicalOperator> left_physical_operator{};
    UniquePtr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return MakeUnique<PhysicalNestedLoopJoin>(logical_operator->node_id(),
                                              logical_join->join_type_,
                                              logical_join->conditions_,
                                              Move(left_physical_operator),
                                              Move(right_physical_operator),
                                              logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCrossProduct(const SharedPtr<LogicalNode> &logical_operator) const {

    auto left_node = logical_operator->left_node();
    auto right_node = logical_operator->right_node();

    if (left_node == nullptr) {
        Error<PlannerException>("Cross product node has no left child.");
    }
    if (right_node == nullptr) {
        Error<PlannerException>("Cross product node has no right child.");
    }

    SharedPtr<LogicalCrossProduct> logical_cross_product = static_pointer_cast<LogicalCrossProduct>(logical_operator);

    UniquePtr<PhysicalOperator> left_physical_operator{};
    UniquePtr<PhysicalOperator> right_physical_operator{};

    left_physical_operator = BuildPhysicalOperator(left_node);
    right_physical_operator = BuildPhysicalOperator(right_node);

    return MakeUnique<PhysicalCrossProduct>(logical_operator->node_id(),
                                            Move(left_physical_operator),
                                            Move(right_physical_operator),
                                            logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildSort(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();

    if (input_logical_node == nullptr) {
        Error<PlannerException>("Sort node has no input node.");
    }
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Sort node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    SharedPtr<LogicalSort> logical_sort = static_pointer_cast<LogicalSort>(logical_operator);

    return MakeUnique<PhysicalSort>(logical_operator->node_id(),
                                    Move(input_physical_operator),
                                    logical_sort->expressions_,
                                    logical_sort->order_by_types_,
                                    logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildLimit(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();

    if (input_logical_node == nullptr) {
        Error<PlannerException>("Limit node has no input node.");
    }
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Limit node shouldn't have right child.");
    }

    SharedPtr<LogicalLimit> logical_limit = static_pointer_cast<LogicalLimit>(logical_operator);
    UniquePtr<PhysicalOperator> input_physical_operator = BuildPhysicalOperator(input_logical_node);
    return MakeUnique<PhysicalLimit>(logical_operator->node_id(),
                                     Move(input_physical_operator),
                                     logical_limit->limit_expression_,
                                     logical_limit->offset_expression_,
                                     logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Logical project node shouldn't have right child.");
    }
    SharedPtr<LogicalProject> logical_project = static_pointer_cast<LogicalProject>(logical_operator);
    UniquePtr<PhysicalOperator> input_physical_operator{};
    if (input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }
    return MakeUnique<PhysicalProject>(logical_operator->node_id(),
                                       logical_project->table_index_,
                                       Move(input_physical_operator),
                                       logical_project->expressions_,
                                       logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildFilter(const SharedPtr<LogicalNode> &logical_operator) const {
    auto input_logical_node = logical_operator->left_node();
    if (input_logical_node == nullptr) {
        Error<PlannerException>("Logical filter node has no input node.");
    }
    if (logical_operator->right_node() != nullptr) {
        Error<PlannerException>("Logical filter node shouldn't have right child.");
    }

    auto input_physical_operator = BuildPhysicalOperator(input_logical_node);

    SharedPtr<LogicalFilter> logical_filter = static_pointer_cast<LogicalFilter>(logical_operator);

    return MakeUnique<PhysicalFilter>(logical_operator->node_id(),
                                      Move(input_physical_operator),
                                      logical_filter->expression(),
                                      logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildIntersect(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeUnique<PhysicalHashJoin>(logical_operator->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildUnion(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeUnique<PhysicalUnionAll>(logical_operator->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildExcept(const SharedPtr<LogicalNode> &logical_operator) const {
    return MakeUnique<PhysicalHashJoin>(logical_operator->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildShow(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalShow> logical_show = static_pointer_cast<LogicalShow>(logical_operator);
    return MakeUnique<PhysicalShow>(logical_show->node_id(),
                                    logical_show->scan_type(),
                                    logical_show->schema_name(),
                                    logical_show->object_name(),
                                    logical_show->table_index(),
                                    logical_show->segment_id(),
                                    logical_show->block_id(),
                                    logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalTableScan> logical_table_scan = static_pointer_cast<LogicalTableScan>(logical_operator);
    return MakeUnique<PhysicalTableScan>(logical_operator->node_id(),
                                         logical_table_scan->base_table_ref_,
                                         logical_operator->load_metas(),
                                         logical_table_scan->add_row_id_);
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildViewScan(const SharedPtr<LogicalNode> &logical_operator) const {
    Error<NotImplementException>("BuildViewScan");
    return nullptr;
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalDummyScan> logical_show = static_pointer_cast<LogicalDummyScan>(logical_operator);
    return MakeUnique<PhysicalDummyScan>(logical_show->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildFlush(const SharedPtr<LogicalNode> &logical_operator) const {
    LogicalFlush *logical_flush = (LogicalFlush *)(logical_operator.get());
    return MakeUnique<PhysicalFlush>(logical_flush->flush_type(), logical_flush->node_id(), logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildOptimize(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalOptimize> logical_optimize = static_pointer_cast<LogicalOptimize>(logical_operator);
    return MakeUnique<PhysicalOptimize>(logical_optimize->node_id(),
                                        logical_optimize->schema_name(),
                                        logical_optimize->object_name(),
                                        logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildMatch(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalMatch> logical_match = static_pointer_cast<LogicalMatch>(logical_operator);
    return MakeUnique<PhysicalMatch>(logical_match->node_id(),
                                     logical_match->base_table_ref_,
                                     logical_match->match_expr_,
                                     logical_match->TableIndex(),
                                     logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildFusion(const SharedPtr<LogicalNode> &logical_operator) const {
    SharedPtr<LogicalFusion> logical_fusion = static_pointer_cast<LogicalFusion>(logical_operator);
    UniquePtr<PhysicalOperator> left_phy = nullptr, right_phy = nullptr;
    auto left_logical_node = logical_operator->left_node();
    if (left_logical_node != nullptr) {
        left_phy = BuildPhysicalOperator(left_logical_node);
    }
    auto right_logical_node = logical_operator->right_node();
    if (right_logical_node != nullptr) {
        right_phy = BuildPhysicalOperator(right_logical_node);
    }
    return MakeUnique<PhysicalFusion>(logical_fusion->node_id(),
                                      Move(left_phy),
                                      Move(right_phy),
                                      logical_fusion->fusion_expr_,
                                      logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildKnn(const SharedPtr<LogicalNode> &logical_operator) const {
    auto *logical_knn_scan = (LogicalKnnScan *)(logical_operator.get());
//    logical_knn_scan->
    UniquePtr<PhysicalKnnScan> knn_scan_op = MakeUnique<PhysicalKnnScan>(logical_knn_scan->node_id(),
                                                                          logical_knn_scan->base_table_ref_,
                                                                          logical_knn_scan->knn_expression_,
                                                                          logical_knn_scan->filter_expression_,
                                                                          logical_knn_scan->GetOutputNames(),
                                                                          logical_knn_scan->GetOutputTypes(),
                                                                          logical_knn_scan->knn_table_index_,
                                                                          logical_operator->load_metas());

    knn_scan_op->PlanWithIndex(query_context_ptr_);
    if(knn_scan_op->TaskCount() == 1) {
        return knn_scan_op;
    } else {
        return MakeUnique<PhysicalMergeKnn>(query_context_ptr_->GetNextNodeID(),
                                            logical_knn_scan->base_table_ref_,
                                            Move(knn_scan_op),
                                            logical_knn_scan->GetOutputNames(),
                                            logical_knn_scan->GetOutputTypes(),
                                            logical_knn_scan->knn_expression_,
                                            logical_knn_scan->knn_table_index_,
                                            logical_operator->load_metas());
    }
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildCommand(const SharedPtr<LogicalNode> &logical_operator) const {
    auto *logical_command = (LogicalCommand *)(logical_operator.get());
    return MakeUnique<PhysicalCommand>(logical_command->node_id(),
                                       logical_command->command_info(),
                                       logical_command->GetOutputNames(),
                                       logical_command->GetOutputTypes(),
                                       logical_operator->load_metas());
}

UniquePtr<PhysicalOperator> PhysicalPlanner::BuildExplain(const SharedPtr<LogicalNode> &logical_operator) const {

    auto input_logical_node = logical_operator->left_node();
    UniquePtr<PhysicalOperator> input_physical_operator{nullptr};
    if (input_logical_node != nullptr) {
        input_physical_operator = BuildPhysicalOperator(input_logical_node);
    }

    SharedPtr<LogicalExplain> logical_explain = static_pointer_cast<LogicalExplain>(logical_operator);

    UniquePtr<PhysicalExplain> explain_node{nullptr};
    switch (logical_explain->explain_type()) {
        case ExplainType::kAnalyze: {
            Error<NotImplementException>("Explain analyze");
            break;
        }
        case ExplainType::kAst:
        case ExplainType::kUnOpt:
        case ExplainType::kOpt: {
            explain_node = MakeUnique<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       logical_explain->TextArray(),
                                                       nullptr,
                                                       logical_operator->load_metas());
            break;
        }
        case ExplainType::kPhysical: {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainPhysicalPlan::Explain(input_physical_operator.get(), texts_ptr);
            explain_node = MakeUnique<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       texts_ptr,
                                                       nullptr,
                                                       logical_operator->load_metas());
            break;
        }
        case ExplainType::kFragment:
        case ExplainType::kPipeline: {
            explain_node = MakeUnique<PhysicalExplain>(logical_explain->node_id(),
                                                       logical_explain->explain_type(),
                                                       nullptr,
                                                       Move(input_physical_operator),
                                                       logical_operator->load_metas());
            break;
        }
    }

    return explain_node;
}

} // namespace infinity
