//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

module secondary_index_scan_builder;

import stl;
import logical_node;
import logical_node_type;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_match;
import query_context;
import logical_node_visitor;
import infinity_exception;
import logger;
import third_party;
import filter_expression_push_down;

namespace infinity {

// Different from LogicalNodeVisitor, this visitor accepts shared_ptr<LogicalNode> as input.
class BuildSecondaryIndexScan {
public:
    explicit BuildSecondaryIndexScan(QueryContext *query_context_ptr) : query_context_(query_context_ptr) {}

    void VisitNode(SharedPtr<LogicalNode> &op) {
        if (!op) {
            return;
        }
        if (op->operator_type() == LogicalNodeType::kFilter) {
            if (op->right_node().get() != nullptr) {
                UnrecoverableError("BuildSecondaryIndexScan: Logical filter node shouldn't have right child.");
            } else if (op->left_node()->operator_type() != LogicalNodeType::kTableScan) {
                LOG_INFO("BuildSecondaryIndexScan: The left child of Logical filter is not table scan. Cannot push down filter. Need to fix.");
            } else {
                auto &filter = static_cast<LogicalFilter &>(*op);
                auto &filter_expression = filter.expression();
                auto &table_scan = static_cast<LogicalTableScan &>(*(op->left_node()));
                auto &base_table_ref_ptr = table_scan.base_table_ref_;
                auto &fast_rough_filter_evaluator = table_scan.fast_rough_filter_evaluator_;
                // check if the filter can be pushed down to the table scan
                IndexScanFilterExpressionPushDownResult index_scan_solve_result =
                    FilterExpressionPushDown::PushDownToIndexScan(query_context_, *base_table_ref_ptr, filter_expression);
                auto &column_index_map = index_scan_solve_result.column_index_map_;
                auto &v_qualified = index_scan_solve_result.index_filter_qualified_;
                auto &s_leftover = index_scan_solve_result.extra_leftover_filter_;
                auto &filter_execute_command = index_scan_solve_result.filter_execute_command_;
                // 1. check if the filter can be pushed down to the table scan
                if (!v_qualified) {
                    // no qualified index filter condition, keep the table scan
                    LOG_TRACE("BuildSecondaryIndexScan: No qualified index scan filter. Keep the table scan.");
                } else {
                    // try to push down the qualified index filter condition to the scan
                    // replace logical table scan with logical index scan
                    auto index_scan = MakeShared<LogicalIndexScan>(query_context_->GetNextNodeID(),
                                                                   std::move(base_table_ref_ptr),
                                                                   std::move(v_qualified),
                                                                   std::move(column_index_map),
                                                                   std::move(filter_execute_command),
                                                                   std::move(fast_rough_filter_evaluator),
                                                                   true);
                    op->set_left_node(std::move(index_scan));
                    LOG_TRACE("BuildSecondaryIndexScan: Push down the qualified index scan filter. Replace table scan with index scan.");
                }
                // 2. check the remaining filter expression
                if (s_leftover) {
                    // Keep the filter node.
                    filter_expression = std::move(s_leftover);
                } else {
                    // Remove the filter node. Need to get parent node
                    SharedPtr<LogicalNode> scan = std::move(op->left_node());
                    op = std::move(scan);
                }
            }
        } else if (op->operator_type() == LogicalNodeType::kKnnScan) {
            auto &knn_scan = static_cast<LogicalKnnScan &>(*op);
            knn_scan.common_query_filter_->TryApplySecondaryIndexFilterOptimizer(query_context_);
        } else if (op->operator_type() == LogicalNodeType::kMatch) {
            auto &match = static_cast<LogicalMatch &>(*op);
            match.common_query_filter_->TryApplySecondaryIndexFilterOptimizer(query_context_);
        }
        // visit children after handling current node
        VisitNode(op->left_node());
        VisitNode(op->right_node());
    }

private:
    QueryContext *query_context_ = nullptr;
};

void SecondaryIndexScanBuilder::ApplyToPlan(QueryContext *query_context_ptr, SharedPtr<LogicalNode> &logical_plan) {
    BuildSecondaryIndexScan visitor(query_context_ptr);
    visitor.VisitNode(logical_plan);
}

} // namespace infinity