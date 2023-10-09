//
// Created by jinhai on 23-3-14.
//

#pragma once

#include "logical_node.h"
#include "planner/node/logical_aggregate.h"
#include "planner/node/logical_create_collection.h"
#include "planner/node/logical_create_schema.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_create_view.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_drop_collection.h"
#include "planner/node/logical_drop_schema.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_drop_view.h"
#include "planner/node/logical_export.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_flush.h"
#include "planner/node/logical_import.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_join.h"
#include "planner/node/logical_knn_scan.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_show.h"
#include "planner/node/logical_sort.h"
#include "planner/node/logical_table_scan.h"

namespace infinity {

class ExplainLogicalPlan {
public:
    static void Explain(const LogicalNode *logical_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateCollection *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropView *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalKnnScan *knn_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalSort *sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalLimit *limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCrossProduct *cross_product_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalImport *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalExport *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalFlush *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const BaseExpression *base_expression, String &expr_str);
};

} // namespace infinity
