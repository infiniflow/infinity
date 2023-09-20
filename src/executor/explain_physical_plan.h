//
// Created by jinhai on 23-3-17.
//

#pragma once

#include "physical_operator.h"

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
class PhysicalCreateCollection;
class PhysicalDropSchema;
class PhysicalDropTable;
class PhysicalDropCollection;
class PhysicalInsert;
class PhysicalProject;
class PhysicalFilter;
class PhysicalTableScan;
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
class ExplainPhysicalPlan {
public:
    static void
    Explain(const PhysicalOperator* op,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive=false,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalUnionAll* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalIndexScan* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDummyScan* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalHashJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSortMergeJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalIndexJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalTop* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDelete* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalUpdate* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalImport* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalExport* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalAlter* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateView* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropView* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateSchema* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateTable* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateCollection* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropSchema* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropTable* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropCollection* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalInsert* insert_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalProject* project_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalFilter* filter_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalTableScan* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalAggregate* aggregate_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSort* sort_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalLimit* limit_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCrossProduct* cross_product_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalNestedLoopJoin* join_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalShow* show_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalFlush* flush_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSource* source_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSink* flush_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalParallelAggregate* parallel_aggregate_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalMergeParallelAggregate* merge_parallel_aggregate_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalIntersect* intersect_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalExcept* except_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalHash* hash_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalMergeHash* merge_hash_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalMergeLimit* merge_limit_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalMergeTop* merge_top_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalMergeSort* merge_sort_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            bool is_recursive,
            i64 intent_size = 0);
};

}
