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

#include <algorithm>

import stl;
import logical_node;
import logical_node_type;
import base_expression;
import column_expression;
import function_expression;
import expression_type;
import logical_join;
import logical_aggregate;
import logical_project;
import logical_filter;
import logical_table_scan;
import logical_match;
import parser;
import base_table_ref;
import function;
import column_binding;
import logger;
import third_party;
import infinity_exception;

module column_pruner;

namespace infinity {

void RemoveUnusedColumns::VisitNode(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kInvalid:
            return;
        case LogicalNodeType::kAggregate: {
            if (!all_referenced_) {
                auto &aggr = op.Cast<LogicalAggregate>();
                auto filtered_aggregates = ClearUnusedExpressions(aggr.aggregates_, aggr.aggregate_index_);

                aggr.aggregates_ = Move(filtered_aggregates);
                if (aggr.aggregates_.empty() && aggr.groups_.empty()) {
                    // TODO: CountStar -> Count(*) is not supported yet
                }
            }
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion: {
            // TODO: Union is not supported yet
            break;
        }
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin: {
            break;
        }
        case LogicalNodeType::kCrossProduct:
            break;
        case LogicalNodeType::kLimit:
            break;
        case LogicalNodeType::kFilter: {
            //            auto &filter = op.Cast<LogicalFilter>();
            // TODO: Scan does not currently support Filter Prune
            break;
        }
        case LogicalNodeType::kProjection: {
            if (!all_referenced_) {
                auto &proj = op.Cast<LogicalProject>();
                auto filtered_expressions = ClearUnusedExpressions(proj.expressions_, proj.table_index_);

                proj.expressions_ = Move(filtered_expressions);
                if (proj.expressions_.empty()) {
                    // nothing references the projected expressions
                    // this happens in the case of e.g. EXISTS(SELECT * FROM ...)
                    // in this case we only need to project a single constant
                    // TODO: EXISTS is not supported yet
                    //                    ValueExpression value_expr(Value::MakeInt(0));
                    //                    proj.expressions_.push_back(MakeShared<ValueExpression>(value_expr));
                }
            }
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
        case LogicalNodeType::kSort: {
            if (!all_referenced_) {
                //                auto &sort = op.Cast<LogicalSort>();
                //                PlannerAssert(sort.expressions_.empty(), "'Order by' clause object should not yet be set");
                // TODO: Sort's projections is not supported yet
            }
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
        case LogicalNodeType::kDelete: {
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
        case LogicalNodeType::kUpdate:
            // Need reading all columns
            return;
        case LogicalNodeType::kInsert:
            break;
        case LogicalNodeType::kImport:
            break;
        case LogicalNodeType::kExport:
            break;
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable:
            break;
        case LogicalNodeType::kCreateIndex:
            break;
        case LogicalNodeType::kCreateCollection:
            break;
        case LogicalNodeType::kCreateSchema:
            break;
        case LogicalNodeType::kCreateView:
            break;
        case LogicalNodeType::kDropTable:
            break;
        case LogicalNodeType::kDropIndex:
            break;
        case LogicalNodeType::kDropCollection:
            break;
        case LogicalNodeType::kDropSchema:
            break;
        case LogicalNodeType::kDropView:
            break;
        case LogicalNodeType::kTableScan: {
            VisitNodeExpression(op);
            auto &scan = op.Cast<LogicalTableScan>();
            // FIXME: Add pushdown judgment when pushdown is supported e.g. (all_referenced_ || scan.projection_pushdown)
            if (all_referenced_) {
                return;
            }
            Vector<SizeT> project_indices = ClearUnusedExpressions(scan.base_table_ref_->column_ids_, scan.TableIndex());

            // TODO: Scan does not currently support Filter Pushdown
            // remove the original columns of scan with the filtered columns
            scan.base_table_ref_->RetainColumnByIndices(Move(project_indices));

            // TODO: Scan does not currently support Filter Prune

            // TODO: EXISTS is not supported yet
            return;
        }
        case LogicalNodeType::kMatch: {
            VisitNodeExpression(op);
            auto &scan = op.Cast<LogicalMatch>();
            if (all_referenced_) {
                return;
            }
            Vector<SizeT> project_indices = ClearUnusedExpressions(scan.base_table_ref_->column_ids_, scan.TableIndex());
            scan.base_table_ref_->RetainColumnByIndices(Move(project_indices));
            return;
        }
        case LogicalNodeType::kViewScan:
            // TODO
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kKnnScan:
            // TODO
            break;
        case LogicalNodeType::kShow:
            break;
        case LogicalNodeType::kExplain:
            break;
        case LogicalNodeType::kPrepare:
            break;
        case LogicalNodeType::kFlush:
            break;
        case LogicalNodeType::kOptimize:
            break;
        case LogicalNodeType::kCommand:
            break;
        case LogicalNodeType::kFusion:
            break;
    }
    LogicalNodeVisitor::VisitNodeExpression(op);
    LogicalNodeVisitor::VisitNodeChildren(op);
}

SharedPtr<BaseExpression> RemoveUnusedColumns::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    column_references_.insert(expression->binding());
    return expression;
}

template <class T>
Vector<T> RemoveUnusedColumns::ClearUnusedExpressions(const Vector<T> &list, idx_t table_idx) {
    Vector<T> items;
    items.reserve(list.size());
    for (idx_t col_idx = 0; col_idx < list.size(); col_idx++) {
        auto current_binding = ColumnBinding(table_idx, col_idx);

        if (column_references_.contains(current_binding)) {
            items.push_back(list[col_idx]);
        }
    }
    return items;
}

} // namespace infinity
