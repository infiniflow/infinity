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

export module explain_physical_plan;

namespace infinity {

class PhysicalUnionAll;

class PhysicalIndexScan;

class PhysicalDummyScan;

class PhysicalHashJoin;

class PhysicalSortMergeJoin;

class PhysicalIndexJoin;

class PhysicalTop;

class PhysicalDelete;

class PhysicalUpdate;

class PhysicalImport;

class PhysicalExport;

class PhysicalAlter;

class PhysicalCreateView;

class PhysicalDropView;

class PhysicalCreateSchema;

class PhysicalCreateTable;

class PhysicalCreateIndex;

class PhysicalCreateCollection;

class PhysicalDropSchema;

class PhysicalDropTable;

class PhysicalDropCollection;

class PhysicalInsert;

class PhysicalProject;

class PhysicalFilter;

class PhysicalTableScan;

class PhysicalKnnScan;

class PhysicalAggregate;

class PhysicalSort;

class PhysicalLimit;

class PhysicalCrossProduct;

class PhysicalNestedLoopJoin;

class PhysicalShow;

class PhysicalFlush;

class PhysicalSource;

class PhysicalSink;

class PhysicalParallelAggregate;

class PhysicalMergeParallelAggregate;

class PhysicalIntersect;

class PhysicalExcept;

class PhysicalHash;

class PhysicalMergeHash;

class PhysicalMergeLimit;

class PhysicalMergeTop;

class PhysicalMergeSort;

class PhysicalMergeKnn;

class PhysicalMatch;
class PhysicalFusion;

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

    static void Explain(const PhysicalCreateIndex *create_index,
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
    static void Explain(const PhysicalFusion *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);
};

} // namespace infinity
