//
// Created by jinhai on 23-3-14.
//

#pragma once

#include "logical_node.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_create_collection.h"
#include "planner/node/logical_create_schema.h"
#include "planner/node/logical_drop_schema.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_drop_collection.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_table_scan.h"
#include "planner/node/logical_sort.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_aggregate.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"

namespace infinity {

class ExplainLogicalPlan {
public:
    static void
    Explain(const LogicalNode* statement,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalCreateSchema* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalCreateTable* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalCreateCollection* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalDropSchema* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalDropTable* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalDropCollection* create_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalProject* project_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalFilter* filter_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalTableScan* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalAggregate* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalSort* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalLimit* table_scan_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalCrossProduct* cross_product_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

    static void
    Explain(const LogicalJoin* cross_product_node,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size = 0);

};

}
