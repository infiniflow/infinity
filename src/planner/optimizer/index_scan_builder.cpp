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

#include <vector>
module index_scan_builder;

import stl;
import logical_node;
import logical_node_type;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_match;
import logical_match_tensor_scan;
import logical_match_scan_base;
import logical_fusion;
import query_context;
import logical_node_visitor;
import infinity_exception;
import logger;
import third_party;
import filter_expression_push_down;

namespace infinity {

// Different from LogicalNodeVisitor, this visitor accepts shared_ptr<LogicalNode> as input.
class BuildIndexScan {
public:
    explicit BuildIndexScan(QueryContext *query_context_ptr) : query_context_(query_context_ptr) {}

    void VisitNode(SharedPtr<LogicalNode> &op) {
        if (!op) {
            return;
        }
        switch (op->operator_type()) {
            case LogicalNodeType::kFilter: {
                if (op->right_node().get() != nullptr) {
                    String error_message = "BuildSecondaryIndexScan: Logical filter node shouldn't have right child.";
                    UnrecoverableError(error_message);
                } else if (op->left_node()->operator_type() != LogicalNodeType::kTableScan) {
                    LOG_INFO("BuildSecondaryIndexScan: The left child of Logical filter is not table scan. Cannot push down filter. Need to fix.");
                } else {
                    auto &filter = static_cast<LogicalFilter &>(*op);
                    auto &filter_expression = filter.expression();
                    auto &table_scan = static_cast<LogicalTableScan &>(*(op->left_node()));
                    auto &base_table_ref_ptr = table_scan.base_table_ref_;
                    auto &fast_rough_filter_evaluator = table_scan.fast_rough_filter_evaluator_;
                    // check if the filter can be pushed down to the table scan
                    auto [index_filter, leftover_filter, index_filter_evaluator] =
                        FilterExpressionPushDown::PushDownToIndexScan(query_context_, *base_table_ref_ptr, filter_expression);
                    // 1. check if the filter can be pushed down to the table scan
                    if (!index_filter) {
                        // no qualified index filter condition, keep the table scan
                        LOG_TRACE("BuildSecondaryIndexScan: No qualified index scan filter. Keep the table scan.");
                    } else {
                        // try to push down the qualified index filter condition to the scan
                        // replace logical table scan with logical index scan
                        auto index_scan = MakeShared<LogicalIndexScan>(query_context_->GetNextNodeID(),
                                                                       std::move(base_table_ref_ptr),
                                                                       std::move(index_filter),
                                                                       std::move(index_filter_evaluator),
                                                                       std::move(fast_rough_filter_evaluator),
                                                                       true);
                        op->set_left_node(std::move(index_scan));
                        LOG_TRACE("BuildSecondaryIndexScan: Push down the qualified index scan filter. Replace table scan with index scan.");
                    }
                    // 2. check the remaining filter expression
                    if (leftover_filter) {
                        // Keep the filter node.
                        filter_expression = std::move(leftover_filter);
                    } else {
                        // Remove the filter node. Need to get parent node
                        SharedPtr<LogicalNode> scan = std::move(op->left_node());
                        op = std::move(scan);
                    }
                }
                break;
            }
            case LogicalNodeType::kKnnScan:
            case LogicalNodeType::kMatchSparseScan:
            case LogicalNodeType::kMatchTensorScan: {
                auto &match_base = static_cast<LogicalMatchScanBase &>(*op);
                match_base.common_query_filter_->TryApplyIndexFilterOptimizer(query_context_);
                break;
            }
            case LogicalNodeType::kMatch: {
                auto &match = static_cast<LogicalMatch &>(*op);
                match.common_query_filter_->TryApplyIndexFilterOptimizer(query_context_);
                break;
            }
            default: {
                break;
            }
        }
        // visit children after handling current node
        VisitNode(op->left_node());
        VisitNode(op->right_node());
        if (op->operator_type() == LogicalNodeType::kFusion) {
            for (auto &fusion = static_cast<LogicalFusion &>(*op); auto &child : fusion.other_children_) {
                VisitNode(child);
            }
        }
    }

private:
    QueryContext *query_context_ = nullptr;
};

void IndexScanBuilder::ApplyToPlan(QueryContext *query_context_ptr, SharedPtr<LogicalNode> &logical_plan) {
    BuildIndexScan visitor(query_context_ptr);
    visitor.VisitNode(logical_plan);
}

} // namespace infinity