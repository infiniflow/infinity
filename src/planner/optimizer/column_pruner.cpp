//
// Created by kould on 23-10-27.
//

module;

#include <bits/stl_algo.h>

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
import logical_table_scan;
import parser;
import base_table_ref;
import function;
import column_binding;
import infinity_assert;
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
                ClearUnusedExpressions(aggr.aggregates_, aggr.aggregate_index_);
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
            auto &join = op.Cast<LogicalJoin>();

            if (all_referenced_ || join.join_type_ != JoinType::kInner) {
                break;
            }
            for (const auto& cond : join.conditions_) {
                FunctionExpression *func_expr = static_cast<FunctionExpression *>(cond.get());
                auto args = func_expr->arguments();
                bool has_not_column_expr = std::any_of(args.cbegin(), args.cend(), [](SharedPtr<BaseExpression> expr) {
                    return expr->type() != ExpressionType::kColumn;
                });
                // FIXME: It would be more elegant if the operators were generalized using enums e.g. DuckDB: cond.comparison == ExpressionType::COMPARE_EQUAL
                if (IsEqual(func_expr->func_.name(), "=") || has_not_column_expr) {
                    continue;
                }
                Assert<PlannerException>(args.size() == 2, "BinaryFunction can only have two arguments.", __FILE_NAME__, __LINE__);
                auto lhs_binding = static_cast<ColumnExpression *>(args[0].get())->binding();
                auto rhs_binding = static_cast<ColumnExpression *>(args[1].get())->binding();
                auto col_refs = column_references_.find(rhs_binding);

                if (col_refs == column_references_.end()) {
                    continue;
                }
                for (auto &entry : col_refs->second) {
                    entry->binding() = lhs_binding;
                    column_references_[lhs_binding].push_back(entry);
                }
                column_references_.erase(rhs_binding);
            }
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
                ClearUnusedExpressions(proj.expressions_, proj.table_index_);

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
            break;
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
            Vector<int> proj_idxs;
            auto &scan_column_ids = scan.base_table_ref_->column_ids_;
            for (int col_idx = 0; col_idx < scan_column_ids.size(); col_idx++) {
                proj_idxs.push_back(col_idx);
            }
            auto filtered_idxs = proj_idxs;
            ClearUnusedExpressions(filtered_idxs, scan.TableIndex(), false);

            // TODO: Scan does not currently support Filter Pushdown

            Vector<int> differences;
            std::set_difference(proj_idxs.cbegin(), proj_idxs.cend(), filtered_idxs.cbegin(), filtered_idxs.cend(), std::back_inserter(differences));
            // remove the original columns of scan with the filtered columns
            scan.base_table_ref_->EraseColumnByIdxs(std::move(differences));

            // TODO: Scan does not currently support Filter Prune

            // TODO: EXISTS is not supported yet
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
    }
    LogicalNodeVisitor::VisitNodeExpression(op);
    LogicalNodeVisitor::VisitNodeChildren(op);

}

SharedPtr<BaseExpression> RemoveUnusedColumns::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    column_references_[expression->binding()].push_back(expression);
    return expression;
}

void RemoveUnusedColumns::ReplaceBinding(ColumnBinding current_binding, ColumnBinding new_binding) {
    auto col_refs = column_references_.find(current_binding);

    if (col_refs == column_references_.end()) {
        return;
    }
    for (auto &colref : col_refs->second) {
        colref->SetBinding(new_binding.table_idx, new_binding.column_idx);
    }
}

template <class T>
void RemoveUnusedColumns::ClearUnusedExpressions(Vector<T> &list, idx_t table_idx, bool replace) {
    idx_t offset = 0;
    for (idx_t col_idx = 0; col_idx < list.size(); col_idx++) {
        auto current_binding = ColumnBinding(table_idx, col_idx + offset);
        auto entry = column_references_.find(current_binding);
        if (entry == column_references_.end()) {
            list.erase(list.begin() + col_idx);
            offset++;
            col_idx--;
        } else if (offset > 0 && replace) {
            // column is used but the ColumnBinding has changed because of removed columns
            ReplaceBinding(current_binding, ColumnBinding(table_idx, col_idx));
        }
    }
}

} // namespace infinity
