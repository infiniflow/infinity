//
// Created by jinhai on 23-3-14.
//

module;

import stl;

export module explain_logical_plan;

namespace infinity {

class LogicalNode;
class LogicalCreateSchema;
class LogicalCreateTable;
class LogicalCreateCollection;
class LogicalCreateView;
class LogicalDropSchema;
class LogicalDropTable;
class LogicalDropCollection;
class LogicalDropView;
class LogicalInsert;
class LogicalProject;
class LogicalFilter;
class LogicalTableScan;
class LogicalKnnScan;
class LogicalAggregate;
class LogicalSort;
class LogicalLimit;
class LogicalCrossProduct;
class LogicalJoin;
class LogicalShow;
class LogicalImport;
class LogicalExport;
class LogicalFlush;
class BaseExpression;

export class ExplainLogicalPlan {
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
