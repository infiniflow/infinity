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

#include <memory>
#include <string>
#include <vector>

module secondary_index_scan_builder;

import stl;
import logical_node;
import logical_node_type;
import logical_filter;
import logical_table_scan;
import logical_index_scan;
import query_context;
import logical_node_visitor;
import infinity_exception;
import base_expression;
import expression_type;
import conjunction_expression;
import function_expression;
import base_table_ref;
import logger;
import third_party;
import scalar_function;
import scalar_function_set;
import catalog;
import cast_expression;
import column_expression;
import secondary_index_scan_execute_expression;
import column_index_entry;
import create_index_info;
import table_index_meta;

namespace infinity {

// TODO: equivalent expression rewrite optimization

// TODO: now do not support "not" expression in index scan

// step 1. flatten the "and" expression into a vector of subexpressions
// step 2. find the index filter candidates
//         step 2 includes:
//             case 1. the subexpression is in the form of "[cast] x compare value_expr" and the column x has a secondary index.
//             case 2. the subexpression is constructed by "and" or "or" expression, and each fundamental child expression satisfies case 1.
// step 3. push down the qualified index filter candidates to the index scan (otherwise, keep the table scan)
class FilterExpressionPushDown {
private:
    QueryContext *query_context_ = nullptr;
    const BaseTableRef &base_table_ref_;
    HashMap<ColumnID, SharedPtr<ColumnIndexEntry>> candidate_column_index_map_;
    Vector<SharedPtr<BaseExpression>> flatten_and_subexpressions_;
    Vector<SharedPtr<BaseExpression>> index_filter_candidates_;
    Vector<SharedPtr<BaseExpression>> index_filter_leftover_;
    HashMap<ColumnID, SharedPtr<ColumnIndexEntry>> column_index_map_;
    SharedPtr<BaseExpression> index_filter_qualified_;
    SharedPtr<BaseExpression> extra_leftover_filter;
    Vector<FilterExecuteElem> filter_execute_command;

public:
    FilterExpressionPushDown(QueryContext *query_context, const BaseTableRef &base_table_ref)
        : query_context_(query_context), base_table_ref_(base_table_ref) {}

    ~FilterExpressionPushDown() = default;

    void Solve(SharedPtr<BaseExpression> &&expression) {
        // flatten expressions combined by "and" into a vector of subexpressions
        FlattenAndExpression(std::move(expression));
        // collect column index information
        InitColumnIndexEntries();
        // classification of subexpressions
        FindIndexFilterCandidates();
        // build two filter expressions: one for index scan, one for normal filter
        PrepareResult();
        // generate filter execute command
        if (index_filter_qualified_) {
            filter_execute_command = BuildSecondaryIndexScanCommand(index_filter_qualified_);
        }
    }

    auto &ColumnIndexMap() { return column_index_map_; }

    auto &QualifiedIndexFilterConditions() { return index_filter_qualified_; }

    auto &LeftoverIndexFilterConditions() { return extra_leftover_filter; }

    auto &FilterExecuteCommand() { return filter_execute_command; }

private:
    inline void FlattenAndExpression(SharedPtr<BaseExpression> &&expression) {
        if (expression->type() == ExpressionType::kFunction) {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            if (function_expression->ScalarFunctionName() == "AND") {
                // flatten the arguments of and expression
                AddSubexpressions(std::move(expression));
                return;
            }
        }
        // the expression is not "and" expression, cannot be flattened
        flatten_and_subexpressions_.emplace_back(std::move(expression));
    }
    inline void AddSubexpressions(SharedPtr<BaseExpression> &&expression) {
        // flatten the arguments of "and" expression
        for (auto &child_expression : expression->arguments()) {
            FlattenAndExpression(std::move(child_expression));
        }
    }

    inline void InitColumnIndexEntries() {
        TransactionID txn_id = query_context_->GetTxn()->TxnID();
        TxnTimeStamp begin_ts = query_context_->GetTxn()->BeginTS();

        auto &table_entry = base_table_ref_.table_entry_ptr_;
        {
            auto map_guard = table_entry->IndexMetaMap();
            for (auto &[index_name, table_index_meta] : *map_guard) {
                auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
                if (!status.ok()) {
                    // Table index entry isn't found
                    RecoverableError(status);
                }
                auto &index_map = table_index_entry->column_index_map();
                for (auto &[column_id, column_index_entry] : index_map) {
                    if (column_index_entry->index_base_ptr()->index_type_ == IndexType::kSecondary) {
                        if (candidate_column_index_map_.contains(column_id)) {
                            LOG_TRACE(fmt::format("InitColumnIndexEntries(): Column {} has multiple secondary indexes. Skipping one.", column_id));
                        } else {
                            candidate_column_index_map_.emplace(column_id, column_index_entry);
                        }
                    }
                }
            }
        }
    }

    inline void FindIndexFilterCandidates() {
        for (auto &expression : flatten_and_subexpressions_) {
            if (CanApplyIndexScan(expression)) {
                index_filter_candidates_.emplace_back(std::move(expression));
            } else {
                index_filter_leftover_.emplace_back(std::move(expression));
            }
        }
        flatten_and_subexpressions_.clear();
    }

    // classification of subexpressions
    inline bool CanApplyIndexScan(const SharedPtr<BaseExpression> &expression) {
        // case 1. expression is a scalar expression containing only one column and the column has a secondary index
        // case 2. expression is an "and" or "or" expression, and each child expression can be applied to the index scan (recursive check)
        // now we do not support "not" expression in index scan
        if (expression->type() == ExpressionType::kFunction) {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            if (auto const &f_name = function_expression->ScalarFunctionName(); f_name == "AND" or f_name == "OR") {
                // case 2.
                if ((expression->arguments()).size() != 2) {
                    UnrecoverableError(
                        fmt::format("Unsupported expression type: In CanApplyIndexScan(), the expression {} is not a binary function expression.",
                                    expression->Name()));
                }
                // recursive check
                return CanApplyIndexScan(expression->arguments()[0]) and CanApplyIndexScan(expression->arguments()[1]);
            } else if (f_name == "NOT") {
                LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), the expression {} is a \"not\" function expression. "
                                      "Now we do not support not expression in index scan.",
                                      expression->Name()));
                return false;
            } else {
                // case 1.
                return CheckExprIndexState(expression, 0);
            }
        } else if (expression->type() == ExpressionType::kValue) {
            LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), the expression \"{}\" is a value expression. "
                                  "Need to apply the expression rewrite optimizer first.",
                                  expression->Name()));
            return false;
        } else {
            LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), unknown expression: {}.", expression->Name()));
            return false;
        }
    }

    // case 1. expression needs to be in the form of "[cast] x compare value_expression" and the column x should have a secondary index.
    inline bool CheckExprIndexState(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        // TODO: now do not support "!=" in index scan
        // expected: depth 0: <, >, <=, >=, = function
        //           depth >= 1: left: column_expr. right: +, -, *, / function (ABS? POW? MOD?), or value, or cast.
        switch (expression->type()) {
            case ExpressionType::kFunction: {
                auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
                auto const &f_name = function_expression->ScalarFunctionName();
                static constexpr std::array<const char *, 5> IndexScanSupportedCompareFunctionNames = {"<", ">", "<=", ">=", "="};
                // depth 0: <, >, <=, >=, = function
                if (std::find(IndexScanSupportedCompareFunctionNames.begin(), IndexScanSupportedCompareFunctionNames.end(), f_name) !=
                    IndexScanSupportedCompareFunctionNames.end()) {
                    // supported compare function
                    if (expression->arguments().size() != 2) {
                        UnrecoverableError(fmt::format("Expression depth: {}. Unsupported expression type: In CheckExprIndexState(), the compare "
                                                       "expression {} argument size != 2.",
                                                       sub_expr_depth,
                                                       expression->Name()));
                        return false;
                    }
                    bool valid_compare = true;
                    // left expression should be column
                    if (!IsColumnExpression(expression->arguments()[0], sub_expr_depth + 1)) {
                        LOG_TRACE(fmt::format("Expression depth: {}. Left expression is not a column with secondary index: {}.",
                                              sub_expr_depth,
                                              function_expression->Name()));
                        valid_compare = false;
                    }
                    if (!IsValueResultExpression(expression->arguments()[1], sub_expr_depth + 1)) {
                        //  right expression should be value
                        LOG_TRACE(fmt::format("Expression depth: {}. Invalid filter function: {}. Right expression is not value type.",
                                              sub_expr_depth,
                                              function_expression->Name()));
                        valid_compare = false;
                    }
                    if (valid_compare) {
                        LOG_TRACE(fmt::format("Expression depth: {}. Valid filter function: {}.", sub_expr_depth, function_expression->Name()));
                    }
                    return valid_compare;
                } else {
                    // unsupported compare function
                    LOG_TRACE(fmt::format("Expression depth: {}. Unsupported filter function for index scan: {}.",
                                          sub_expr_depth,
                                          function_expression->Name()));
                    return false;
                }
            }
            case ExpressionType::kValue: {
                LOG_TRACE(fmt::format("Expression depth: {}. Value {} appears in depth 0. Maybe need the expression rewrite optimizer.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
            case ExpressionType::kCast: {
                LOG_TRACE(fmt::format("Expression depth: {}. Cast expression {} appears in depth 0. Maybe need the expression rewrite optimizer.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
            default: {
                LOG_TRACE(fmt::format("Expression depth: {}. Unsupported expression type: In CheckExprIndexState(), unsupported expression {}.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
        }
    }
    inline bool IsColumnExpression(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        switch (expression->type()) {
            case ExpressionType::kCast: {
                // check cast
                if (expression->arguments().size() != 1) {
                    UnrecoverableError(fmt::format("Expression depth: {}. Unsupported expression type: In IsColumnExpression(), the cast expression "
                                                   "{} has more than 1 argument.",
                                                   sub_expr_depth,
                                                   expression->Name()));
                    return false;
                }
                return IsColumnExpression(expression->arguments()[0], sub_expr_depth + 1);
            }
            case ExpressionType::kColumn: {
                if (!(expression->Type().CanBuildSecondaryIndex())) {
                    // Unsupported type
                    LOG_TRACE(fmt::format("Expression depth: {}. In IsColumnExpression(), unsupported column value type {}. Expression: {}.",
                                          sub_expr_depth,
                                          expression->Type().ToString(),
                                          expression->Name()));
                    return false;
                }
                auto column_expression = std::static_pointer_cast<ColumnExpression>(expression);
                auto column_id = column_expression->binding().column_idx;
                if (candidate_column_index_map_.contains(column_id)) {
                    LOG_TRACE(fmt::format("Expression depth: {}. Column {} has index.", sub_expr_depth, column_expression->Name()));
                    return true;
                } else {
                    LOG_TRACE(fmt::format("Expression depth: {}. Column {} does not have a secondary index. Cannot apply index scan.",
                                          sub_expr_depth,
                                          column_expression->Name()));
                    return false;
                }
            }
            default: {
                LOG_TRACE(fmt::format("Expression depth: {}. In IsColumnExpression(), unsupported expression {}. Expecting column expression.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
        }
    }
    inline bool IsValueResultExpression(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        // expected: function / value / cast
        switch (expression->type()) {
            case ExpressionType::kValue: {
                return true;
            }
            case ExpressionType::kCast: {
                if (expression->arguments().size() != 1) {
                    UnrecoverableError(fmt::format("Expression depth: {}. Unsupported expression type: In IsValueResultExpression(), the cast "
                                                   "expression {} has more than 1 argument.",
                                                   sub_expr_depth,
                                                   expression->Name()));
                    return false;
                }
                return IsValueResultExpression(expression->arguments()[0], sub_expr_depth + 1);
            }
            case ExpressionType::kFunction: {
                for (auto &child_expression : expression->arguments()) {
                    if (!IsValueResultExpression(child_expression, sub_expr_depth + 1)) {
                        LOG_TRACE(
                            fmt::format("Expression depth: {}. Function {} contains non-value subexpression.", sub_expr_depth, expression->Name()));
                        return false;
                    }
                }
                LOG_TRACE(fmt::format("Expression depth: {}. valid numerical function: {}.", sub_expr_depth, expression->Name()));
                return true;
            }
            default: {
                LOG_TRACE(fmt::format("Expression depth: {}. Unsupported expression type: In IsValueResultExpression(), unsupported expression {}.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
        }
    }

    inline void PrepareResult() {
        auto and_function_set_ptr = Catalog::GetFunctionSetByName(query_context_->storage()->catalog(), "AND");
        auto and_scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(and_function_set_ptr);
        // 1. build column_index_map_
        for (auto &expression : index_filter_candidates_) {
            if (!AddIndexForBooleanExpression(expression)) {
                // cannot apply index scan
                UnrecoverableError(fmt::format("Error when trying to apply index scan on expression {}.", expression->Name()));
            }
        }
        // 2. build index_filter_qualified_
        // should be in the form of :
        //   expression constructed with
        //     1. fundamental expression ([cast] x compare value_expr)
        //     2. conjunction "and", "or" and "not".
        for (auto &expression : index_filter_candidates_) {
            if (!index_filter_qualified_) {
                index_filter_qualified_ = std::move(expression);
            } else {
                // build "and" function
                Vector<SharedPtr<BaseExpression>> arguments;
                arguments.emplace_back(std::move(index_filter_qualified_));
                arguments.emplace_back(std::move(expression));
                ScalarFunction and_func = and_scalar_function_set_ptr->GetMostMatchFunction(arguments);
                index_filter_qualified_ = MakeShared<FunctionExpression>(std::move(and_func), std::move(arguments));
            }
        }
        index_filter_candidates_.clear();
        // 3. build extra_leftover_filter
        for (auto &expression : index_filter_leftover_) {
            if (!extra_leftover_filter) {
                extra_leftover_filter = std::move(expression);
            } else {
                // build "and" function
                Vector<SharedPtr<BaseExpression>> arguments;
                arguments.emplace_back(std::move(extra_leftover_filter));
                arguments.emplace_back(std::move(expression));
                ScalarFunction and_func = and_scalar_function_set_ptr->GetMostMatchFunction(arguments);
                extra_leftover_filter = MakeShared<FunctionExpression>(std::move(and_func), std::move(arguments));
            }
        }
        index_filter_leftover_.clear();
    }
    inline bool AddIndexForBooleanExpression(SharedPtr<BaseExpression> &expression) {
        if (expression->type() == ExpressionType::kFunction) {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            if (auto const &f_name = function_expression->ScalarFunctionName(); f_name == "AND" or f_name == "OR") {
                // recursive check
                return AddIndexForBooleanExpression(expression->arguments()[0]) and AddIndexForBooleanExpression(expression->arguments()[1]);
            } else if (f_name == "NOT") {
                // recursive check
                UnrecoverableError("Error when trying to apply index scan on \"NOT\" expression: need to be excluded early.");
                return false;
            } else {
                // compare function "[cast] x compare value_expr"
                // check left argument
                return AddIndexForColumnExpression(expression->arguments()[0]);
            }
        } else {
            return false;
        }
    }
    inline bool AddIndexForColumnExpression(SharedPtr<BaseExpression> &expression) {
        if (expression->type() == ExpressionType::kCast) {
            return AddIndexForColumnExpression(expression->arguments()[0]);
        } else if (expression->type() == ExpressionType::kColumn) {
            auto column_expression = std::static_pointer_cast<ColumnExpression>(expression);
            return AddColumnToResultMap(column_expression->binding().column_idx);
        } else {
            return false;
        }
    }
    inline bool AddColumnToResultMap(ColumnID column_id) {
        if (column_index_map_.contains(column_id)) {
            return true;
        } else if (candidate_column_index_map_.contains(column_id)) {
            column_index_map_.emplace(column_id, candidate_column_index_map_[column_id]);
            return true;
        } else {
            return false;
        }
    }
};

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
                // check if the filter can be pushed down to the table scan
                FilterExpressionPushDown filter_expression_push_down(query_context_, *base_table_ref_ptr);
                filter_expression_push_down.Solve(std::move(filter_expression));
                auto &column_index_map = filter_expression_push_down.ColumnIndexMap();
                auto &v_qualified = filter_expression_push_down.QualifiedIndexFilterConditions();
                auto &s_leftover = filter_expression_push_down.LeftoverIndexFilterConditions();
                auto &filter_execute_command = filter_expression_push_down.FilterExecuteCommand();
                // 1. check if the filter can be pushed down to the table scan
                if (!v_qualified) {
                    // no qualified index filter condition, keep the table scan
                    LOG_INFO("BuildSecondaryIndexScan: No qualified index scan filter. Keep the table scan.");
                } else {
                    // try to push down the qualified index filter condition to the scan
                    // replace logical table scan with logical index scan
                    auto index_scan = MakeShared<LogicalIndexScan>(query_context_->GetNextNodeID(),
                                                                   std::move(base_table_ref_ptr),
                                                                   std::move(v_qualified),
                                                                   std::move(column_index_map),
                                                                   std::move(filter_execute_command),
                                                                   true);
                    op->set_left_node(std::move(index_scan));
                    LOG_INFO("BuildSecondaryIndexScan: Push down the qualified index scan filter. Replace table scan with index scan.");
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