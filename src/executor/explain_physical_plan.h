//
// Created by jinhai on 23-3-17.
//

#pragma once

#include "physical_operator.h"
#include "executor/operator/physical_create_schema.h"
#include "executor/operator/physical_create_table.h"
#include "executor/operator/physical_create_collection.h"
#include "executor/operator/physical_drop_schema.h"
#include "executor/operator/physical_drop_table.h"
#include "executor/operator/physical_drop_collection.h"
#include "executor/operator/physical_insert.h"
#include "executor/operator/physical_project.h"
#include "executor/operator/physical_filter.h"
#include "executor/operator/physical_table_scan.h"
#include "executor/operator/physical_aggregate.h"
#include "executor/operator/physical_sort.h"
#include "executor/operator/physical_limit.h"
#include "executor/operator/physical_cross_product.h"
#include "executor/operator/physical_nested_loop_join.h"
#include "executor/operator/physical_chunk_scan.h"
#include "executor/operator/physical_union_all.h"
#include "executor/operator/physical_index_scan.h"
#include "executor/operator/physical_dummy_scan.h"
#include "executor/operator/physical_hash_join.h"
#include "executor/operator/physical_sort_merge_join.h"
#include "executor/operator/physical_index_join.h"
#include "executor/operator/physical_top.h"
#include "executor/operator/physical_delete.h"
#include "executor/operator/physical_update.h"
#include "executor/operator/physical_import.h"
#include "executor/operator/physical_export.h"
#include "executor/operator/physical_alter.h"
#include "executor/operator/physical_create_view.h"
#include "executor/operator/physcial_drop_view.h"

namespace infinity {

class ExplainPhysicalPlan {
public:
    static void
    Explain(const PhysicalOperator* op,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalUnionAll* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalIndexScan* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDummyScan* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalHashJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSortMergeJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalIndexJoin* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalTop* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDelete* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalUpdate* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalImport* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalExport* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalAlter* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateView* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropView* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateSchema* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateTable* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCreateCollection* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropSchema* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropTable* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalDropCollection* drop_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalInsert* insert_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalProject* project_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalFilter* filter_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalTableScan* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalAggregate* aggregate_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalSort* sort_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalLimit* limit_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalCrossProduct* cross_product_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalNestedLoopJoin* join_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const PhysicalChunkScan* chunk_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

};

}
