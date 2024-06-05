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

module apply_fast_rough_filter;

import stl;
import logical_node;
import logical_node_type;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_match_tensor_scan;
import logical_match;
import query_context;
import logical_node_visitor;
import infinity_exception;
import logger;
import third_party;
import filter_expression_push_down;
import fast_rough_filter;

namespace infinity {

class ApplyFastRoughFilterMethod {
public:
    inline static void VisitNode(SharedPtr<LogicalNode> &op) {
        if (!op) {
            return;
        }
        if (op->operator_type() == LogicalNodeType::kFilter) {
            auto &filter = static_cast<LogicalFilter &>(*op);
            auto &filter_expression = filter.expression();
            if (op->right_node().get() != nullptr) {
                String error_message = "BuildSecondaryIndexScan: Logical filter node shouldn't have right child.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            } else if (op->left_node()->operator_type() == LogicalNodeType::kTableScan) {
                auto &table_scan = static_cast<LogicalTableScan &>(*(op->left_node()));
                table_scan.fast_rough_filter_evaluator_ = FilterExpressionPushDown::PushDownToFastRoughFilter(filter_expression);
            } else if (op->left_node()->operator_type() == LogicalNodeType::kIndexScan) {
                // warn
                LOG_WARN("ApplyFastRoughFilterMethod: IndexScan exist after Filter. A part of filter condition has been removed.");
                // still build from remaining filter condition
                auto &index_scan = static_cast<LogicalIndexScan &>(*(op->left_node()));
                index_scan.fast_rough_filter_evaluator_ = FilterExpressionPushDown::PushDownToFastRoughFilter(filter_expression);
            } else {
                LOG_WARN("ApplyFastRoughFilterMethod: Filter node should be followed by TableScan or IndexScan.");
            }
        } else if (op->operator_type() == LogicalNodeType::kKnnScan) {
            // also need to apply filter
            auto &knn_scan = static_cast<LogicalKnnScan &>(*op);
            knn_scan.common_query_filter_->TryApplyFastRoughFilterOptimizer();
        } else if (op->operator_type() == LogicalNodeType::kMatch) {
            // also need to apply filter
            auto &match = static_cast<LogicalMatch &>(*op);
            match.common_query_filter_->TryApplyFastRoughFilterOptimizer();
        } else if (op->operator_type() == LogicalNodeType::kMatchTensorScan) {
            // also need to apply filter
            auto &matchtensor = static_cast<LogicalMatchTensorScan &>(*op);
            matchtensor.common_query_filter_->TryApplyFastRoughFilterOptimizer();
        } else if (op->operator_type() == LogicalNodeType::kIndexScan) {
            String error_message = "ApplyFastRoughFilterMethod: IndexScan optimizer should not happen before ApplyFastRoughFilter optimizer.";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        // visit children after handling current node
        VisitNode(op->left_node());
        VisitNode(op->right_node());
    }
};

void ApplyFastRoughFilter::ApplyToPlan(QueryContext *, SharedPtr<LogicalNode> &logical_plan) { ApplyFastRoughFilterMethod::VisitNode(logical_plan); }

} // namespace infinity
