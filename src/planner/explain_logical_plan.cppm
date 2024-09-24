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

export module explain_logical_plan;

import stl;
import logical_node;
import logical_create_schema;
import logical_create_table;
import logical_create_index;
import logical_create_collection;
import logical_create_view;
import logical_drop_schema;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_view;
import logical_insert;
import logical_delete;
import logical_update;
import logical_project;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_aggregate;
import logical_sort;
import logical_limit;
import logical_top;
import logical_cross_product;
import logical_join;
import logical_show;
import logical_import;
import logical_export;
import logical_flush;
import logical_optimize;
import logical_match;
import logical_match_sparse_scan;
import logical_match_tensor_scan;
import logical_fusion;
import base_expression;

namespace infinity {

export class ExplainLogicalPlan {
public:
    static void Explain(const LogicalNode *logical_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateSchema *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateTable *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateIndex *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateCollection *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalCreateView *create_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropSchema *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropTable *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropCollection *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDropView *drop_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalInsert *insert_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalDelete *delete_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalUpdate *update_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalProject *project_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalFilter *filter_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalTableScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalIndexScan *table_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalKnnScan *knn_scan_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalAggregate *aggregate_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalSort *sort_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalLimit *limit_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalTop *top_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size);

    static void Explain(const LogicalCrossProduct *cross_product_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalJoin *join_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalShow *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalImport *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalExport *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalFlush *show_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalOptimize *optimize_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalMatch *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalMatchSparseScan *match_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalMatchTensorScan *match_tensor_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const LogicalFusion *fusion_node, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size = 0);

    static void Explain(const BaseExpression *base_expression, String &expr_str, bool consider_add_parentheses = false);
};

} // namespace infinity
