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

module column_pruner;

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
import logical_index_scan;
import logical_match;
import logical_match_scan_base;
import logical_knn_scan;
import logical_match_tensor_scan;
import base_table_ref;
import function;
import column_binding;
import logger;
import third_party;
import infinity_exception;

namespace infinity {

void RemoveUnusedColumns::VisitNode(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kUpdate:
        case LogicalNodeType::kInsert:
        case LogicalNodeType::kImport:
        case LogicalNodeType::kExport:
        case LogicalNodeType::kCreateTable:
        case LogicalNodeType::kCreateIndex:
        case LogicalNodeType::kDropTable:
        case LogicalNodeType::kDropIndex:
        case LogicalNodeType::kCreateSchema:
        case LogicalNodeType::kDropSchema:
        case LogicalNodeType::kShow:
        case LogicalNodeType::kCommand:
        case LogicalNodeType::kPrepare: {
            // skip
            return;
        }
        case LogicalNodeType::kAggregate: {
            if (!all_referenced_) {
                auto &aggr = op.Cast<LogicalAggregate>();
                auto filtered_aggregates = ClearUnusedExpressions(aggr.aggregates_, aggr.aggregate_index_);

                aggr.aggregates_ = std::move(filtered_aggregates);
                if (aggr.aggregates_.empty() && aggr.groups_.empty()) {
                    // TODO: CountStar -> Count(*) is not supported yet
                }
            }
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
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

                proj.expressions_ = std::move(filtered_expressions);
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
        case LogicalNodeType::kDelete: {
            RemoveUnusedColumns remove;
            remove.VisitNodeExpression(op);
            remove.VisitNodeChildren(op);
            return;
        }
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kTableScan: {
            VisitNodeExpression(op);
            auto &scan = op.Cast<LogicalTableScan>();
            // FIXME: Add pushdown judgment when pushdown is supported e.g. (all_referenced_ || scan.projection_pushdown)
            if (all_referenced_) {
                return;
            }
            Vector<SizeT> project_indices = ClearUnusedBaseTableColumns(scan.base_table_ref_->column_ids_, scan.TableIndex());

            // TODO: Scan does not currently support Filter Pushdown
            // remove the original columns of scan with the filtered columns
            scan.base_table_ref_->RetainColumnByIndices(project_indices);

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
            Vector<SizeT> project_indices = ClearUnusedBaseTableColumns(scan.base_table_ref_->column_ids_, scan.TableIndex());
            scan.base_table_ref_->RetainColumnByIndices(project_indices);
            return;
        }
        case LogicalNodeType::kKnnScan:
        case LogicalNodeType::kMatchSparseScan:
        case LogicalNodeType::kMatchTensorScan: {
            VisitNodeExpression(op);
            auto &match_base = op.Cast<LogicalMatchScanBase>();
            if (all_referenced_) {
                return;
            }
            Vector<SizeT> project_indices = ClearUnusedBaseTableColumns(match_base.base_table_ref_->column_ids_, match_base.TableIndex());
            match_base.base_table_ref_->RetainColumnByIndices(project_indices);
            return;
        }
        case LogicalNodeType::kViewScan:
            // TODO
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kIndexScan: {
            // do not visit node expression
            auto &scan = op.Cast<LogicalIndexScan>();
            Vector<SizeT> project_indices = ClearUnusedBaseTableColumns(scan.base_table_ref_->column_ids_, scan.TableIndex());
            scan.base_table_ref_->RetainColumnByIndices(project_indices);
            return;
        }
        case LogicalNodeType::kExplain:
            break;
        case LogicalNodeType::kOptimize:
            break;
        case LogicalNodeType::kFusion:
            break;
        case LogicalNodeType::kSort:
            break;
        case LogicalNodeType::kTop:
            break;
        default:
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

template <class T>
Vector<T> RemoveUnusedColumns::ClearUnusedBaseTableColumns(const Vector<T> &col_list, const idx_t table_idx) {
    Vector<T> items;
    items.reserve(col_list.size());
    for (idx_t idx = 0; idx < col_list.size(); ++idx) {
        if (const auto current_binding = ColumnBinding(table_idx, col_list[idx]); column_references_.contains(current_binding)) {
            items.push_back(idx);
        }
    }
    return items;
}

} // namespace infinity
