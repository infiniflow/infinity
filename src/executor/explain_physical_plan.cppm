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

import stl;
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

export module explain_physical_plan;

namespace infinity {

export class ExplainPhysicalPlan {
public:
    static void Explain(const PhysicalOperator *op, SharedPtr<Vector<SharedPtr<String>>> &result, bool is_recursive = true, i64 intent_size = 0);

    static void Explain(const PhysicalUnionAll *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalIndexScan *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDummyScan *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalHashJoin *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalSortMergeJoin *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalIndexJoin *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalTop *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDelete *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalUpdate *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalImport *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalExport *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalAlter *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDropView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalCreateIndexPrepare *create_index,
                        SharedPtr<Vector<SharedPtr<String>>> &result,

                        i64 intent_size = 0);

    static void Explain(const PhysicalCreateCollection *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalKnnScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalSort *sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalLimit *limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalCrossProduct *cross_product_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalNestedLoopJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalFlush *flush_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalSource *source_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalSink *flush_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalParallelAggregate *parallel_aggregate_node,
                        SharedPtr<Vector<SharedPtr<String>>> &result,

                        i64 intent_size = 0);

    static void Explain(const PhysicalMergeParallelAggregate *merge_parallel_aggregate_node,
                        SharedPtr<Vector<SharedPtr<String>>> &result,

                        i64 intent_size = 0);

    static void Explain(const PhysicalIntersect *intersect_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalExcept *except_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalHash *hash_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeHash *merge_hash_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeLimit *merge_limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeTop *merge_top_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeSort *merge_sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeKnn *merge_knn_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMatch *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMatchTensorScan *match_tensor_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalMergeMatchTensor *merge_match_tensor_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const PhysicalFusion *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);
 
    static void Explain(const PhysicalMergeAggregate *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);
};

} // namespace infinity
