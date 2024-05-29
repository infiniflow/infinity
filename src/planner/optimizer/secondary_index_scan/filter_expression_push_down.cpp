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

#include <string>
module filter_expression_push_down;

import stl;
import internal_types;
import data_type;
import logical_type;
import query_context;
import infinity_exception;
import base_expression;
import expression_state;
import expression_evaluator;
import expression_type;
import function_expression;
import base_table_ref;
import logger;
import third_party;
import scalar_function;
import scalar_function_set;
import cast_expression;
import column_expression;
import value_expression;
import secondary_index_scan_execute_expression;
import index_base;
import table_index_entry;
import create_index_info;
import catalog;
import fast_rough_filter;
import value;
import column_vector;
import filter_expression_push_down_helper;
import table_index_meta;

namespace infinity {

class FilterExpressionPushDownMethodBase {
protected:
    QueryContext *query_context_ = nullptr;
    const BaseTableRef &base_table_ref_;
    Vector<SharedPtr<BaseExpression>> flatten_and_subexpressions_;

protected:
    FilterExpressionPushDownMethodBase(QueryContext *query_context, const BaseTableRef &base_table_ref)
        : query_context_(query_context), base_table_ref_(base_table_ref) {}

    // old "and" tree structure is destroyed
    inline void FlattenAndExpression(const SharedPtr<BaseExpression> &expression) {
        if (expression->type() == ExpressionType::kFunction) {
            auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
            if (function_expression->ScalarFunctionName() == "AND") {
                // flatten the arguments of and expression
                AddSubexpressions(expression);
                return;
            }
        }
        // the expression is not "and" expression, cannot be flattened
        flatten_and_subexpressions_.push_back(expression);
    }

    inline void AddSubexpressions(const SharedPtr<BaseExpression> &expression) {
        // flatten the arguments of "and" expression
        for (const auto &child_expression : expression->arguments()) {
            FlattenAndExpression(child_expression);
        }
    }

public:
    static inline bool HaveLeftColumnAndRightValue(const SharedPtr<FunctionExpression> &expression, u32 sub_expr_depth, auto &is_valid_column) {
        if (expression->arguments().size() != 2) {
            String error_message = "HaveLeftColumnAndRightValue: argument size != 2";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        return IsValidColumnExpression(expression->arguments()[0], sub_expr_depth + 1, is_valid_column) and
               IsValueResultExpression(expression->arguments()[1], sub_expr_depth + 1);
    }

    static inline bool HaveRightColumnAndLeftValue(const SharedPtr<FunctionExpression> &expression, u32 sub_expr_depth, auto &is_valid_column) {
        if (expression->arguments().size() != 2) {
            String error_message = "HaveRightColumnAndRightValue: argument size != 2";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        return IsValidColumnExpression(expression->arguments()[1], sub_expr_depth + 1, is_valid_column) and
               IsValueResultExpression(expression->arguments()[0], sub_expr_depth + 1);
    }

    static inline bool IsValidColumnExpression(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth, auto &is_valid_column_expression) {
        switch (expression->type()) {
            case ExpressionType::kCast: {
                // check cast
                if (expression->arguments().size() != 1) {
                    UnrecoverableError(
                        fmt::format("Expression depth: {}. Unsupported expression type: In IsValidColumnExpression(), the cast expression "
                                    "{} has more than 1 argument.",
                                    sub_expr_depth,
                                    expression->Name()));
                    return false;
                }
                // check unwind compatibility
                // support:
                // case 1. cast "smaller ints" to BigIntT
                // case 2. cast "ints" or "floats" to DoubleT
                bool valid_cast = false;
                switch (expression->Type().type()) {
                    case LogicalType::kBigInt: {
                        switch (expression->arguments()[0]->Type().type()) {
                            case LogicalType::kTinyInt:
                            case LogicalType::kSmallInt:
                            case LogicalType::kInteger: {
                                valid_cast = true;
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    case LogicalType::kDouble: {
                        switch (expression->arguments()[0]->Type().type()) {
                            case LogicalType::kTinyInt:
                            case LogicalType::kSmallInt:
                            case LogicalType::kInteger:
                            case LogicalType::kBigInt:
                            case LogicalType::kFloat: {
                                valid_cast = true;
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
                if (!valid_cast) {
                    return false;
                }
                return IsValidColumnExpression(expression->arguments()[0], sub_expr_depth + 1, is_valid_column_expression);
            }
            case ExpressionType::kColumn: {
                return is_valid_column_expression(expression, sub_expr_depth);
            }
            default: {
                LOG_TRACE(fmt::format("Expression depth: {}. In IsValidColumnExpression(), unsupported expression {}. Expecting column expression.",
                                      sub_expr_depth,
                                      expression->Name()));
                return false;
            }
        }
    }

    static inline bool IsValueResultExpression(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        // expected: function / value / cast
        switch (expression->type()) {
            case ExpressionType::kValue: {
                return true;
            }
            case ExpressionType::kCast: {
                if (expression->arguments().size() != 1) {
                    String error_message = fmt::format("Expression depth: {}. Unsupported expression type: In IsValueResultExpression(), the cast "
                                                       "expression {} has more than 1 argument.",
                                                       sub_expr_depth,
                                                       expression->Name());
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
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
};

// step 1. flatten the "and" expression into a vector of subexpressions
// step 2. find the index filter candidates
//         step 2 includes:
//             case 1. the subexpression is in the form of "[cast] x compare value_expr" and the column x has a secondary index.
//             case 2. the subexpression is constructed by "and" or "or" expression, and each fundamental child expression satisfies case 1.
// step 3. push down the qualified index filter candidates to the index scan (otherwise, keep the table scan)
class IndexScanFilterExpressionPushDownMethod final : public FilterExpressionPushDownMethodBase {
private:
    // for index scan
    HashMap<ColumnID, TableIndexEntry *> candidate_column_index_map_;
    Vector<SharedPtr<BaseExpression>> index_filter_candidates_;
    Vector<SharedPtr<BaseExpression>> index_filter_leftover_;
    HashMap<ColumnID, TableIndexEntry *> column_index_map_;
    SharedPtr<BaseExpression> index_filter_qualified_;
    SharedPtr<BaseExpression> extra_leftover_filter_;
    Vector<FilterExecuteElem> filter_execute_command_;

public:
    IndexScanFilterExpressionPushDownMethod(QueryContext *query_context, const BaseTableRef &base_table_ref)
        : FilterExpressionPushDownMethodBase(query_context, base_table_ref) {}

    IndexScanFilterExpressionPushDownResult SolveForIndexScan(const SharedPtr<BaseExpression> &expression) {
        // flatten expressions combined by "and" into a vector of subexpressions
        FlattenAndExpression(expression);
        // collect column index information
        InitColumnIndexEntries();
        // classification of subexpressions
        FindIndexFilterCandidates();
        // build two filter expressions: one for index scan, one for normal filter
        PrepareResult();
        // generate filter execute command
        if (index_filter_qualified_) {
            filter_execute_command_ = BuildSecondaryIndexScanCommand(index_filter_qualified_);
        }
        return {std::move(column_index_map_),
                std::move(index_filter_qualified_),
                std::move(extra_leftover_filter_),
                std::move(filter_execute_command_)};
    }

private:
    inline void InitColumnIndexEntries() {
        TransactionID txn_id = query_context_->GetTxn()->TxnID();
        TxnTimeStamp begin_ts = query_context_->GetTxn()->BeginTS();

        auto &table_entry = base_table_ref_.table_entry_ptr_;
        {
            auto map_guard = table_entry->IndexMetaMap();
            for (auto &[index_name, table_index_meta] : *map_guard) {
                auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
                if (!status.ok()) {
                    // skip invalid entry, for example, the index is deleted
                    continue;
                }
                const IndexBase *index_base = table_index_entry->index_base();
                if (index_base->index_type_ != IndexType::kSecondary) {
                    continue;
                }
                String column_name = index_base->column_name();
                u64 column_id = table_entry->GetColumnIdByName(column_name);
                if (candidate_column_index_map_.contains(column_id)) {
                    LOG_TRACE(fmt::format("InitColumnIndexEntries(): Column {} has multiple secondary indexes. Skipping one.", column_id));
                } else {
                    candidate_column_index_map_.emplace(column_id, table_index_entry);
                }
            }
        }
    }

    inline void FindIndexFilterCandidates() {
        for (auto &expression : flatten_and_subexpressions_) {
            if (auto new_expr = RewriteForIndexScan(expression); new_expr) {
                index_filter_candidates_.emplace_back(std::move(new_expr));
            } else {
                index_filter_leftover_.emplace_back(std::move(expression));
            }
        }
        flatten_and_subexpressions_.clear();
    }

    // classification of subexpressions
    inline SharedPtr<BaseExpression> RewriteForIndexScan(const SharedPtr<BaseExpression> &expression) {
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
                auto left_arg = RewriteForIndexScan(expression->arguments()[0]);
                if (!left_arg) {
                    return nullptr;
                }
                auto right_arg = RewriteForIndexScan(expression->arguments()[1]);
                if (!right_arg) {
                    return nullptr;
                }
                // both left and right can be applied to index scan
                Vector<SharedPtr<BaseExpression>> arguments;
                arguments.emplace_back(std::move(left_arg));
                arguments.emplace_back(std::move(right_arg));
                return MakeShared<FunctionExpression>(function_expression->func_, std::move(arguments));
            } else if (f_name == "NOT") {
                LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), the expression {} is a \"not\" function expression. "
                                      "Now we do not support not expression in index scan.",
                                      expression->Name()));
                return nullptr;
            } else {
                // case 1.
                return CheckExprIndexStateAndRewrite(expression, 0);
            }
        } else if (expression->type() == ExpressionType::kValue) {
            LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), the expression \"{}\" is a value expression. "
                                  "Need to apply the expression rewrite optimizer first.",
                                  expression->Name()));
            return nullptr;
        } else {
            LOG_TRACE(fmt::format("Unsupported expression type: In CanApplyIndexScan(), unknown expression: {}.", expression->Name()));
            return nullptr;
        }
    }

    // case 1. expression needs to be in the form of "[cast] x compare value_expression" and the column x should have a secondary index.
    inline SharedPtr<BaseExpression> CheckExprIndexStateAndRewrite(const SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        // TODO: now do not support "!=" in index scan
        // expected: depth 0: <, >, <=, >=, = function
        //           depth >= 1: left: column_expr. right: +, -, *, / function (ABS? POW? MOD?), or value, or cast.
        switch (expression->type()) {
            case ExpressionType::kFunction: {
                auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
                auto const &f_name = function_expression->ScalarFunctionName();
                static constexpr std::array<const char *, 5> IndexScanSupportedCompareFunctionNames = {"<", ">", "<=", ">=", "="};
                static constexpr std::array<const char *, 5> IndexScanSupportedCompareFunctionNamesCorrespondingReverse = {">", "<", ">=", "<=", "="};
                // depth 0: <, >, <=, >=, = function
                if (auto name_iter = std::find(IndexScanSupportedCompareFunctionNames.begin(), IndexScanSupportedCompareFunctionNames.end(), f_name);
                    name_iter != IndexScanSupportedCompareFunctionNames.end()) {
                    // supported compare function
                    if (expression->arguments().size() != 2) {
                        String error_message = fmt::format("Expression depth: {}. Unsupported expression type: In CheckExprIndexState(), the compare "
                                                           "expression {} argument size != 2.",
                                                           sub_expr_depth,
                                                           expression->Name());
                        LOG_CRITICAL(error_message);
                        UnrecoverableError(error_message);
                        return nullptr;
                    }
                    auto is_column_index = [&m = candidate_column_index_map_](const SharedPtr<BaseExpression> &expr, u32 depth) -> bool {
                        if (!(expr->Type().CanBuildSecondaryIndex())) {
                            // Unsupported type
                            LOG_TRACE(fmt::format("Expression depth: {}. In is_column_index(), unsupported column value type {}. Expression: {}.",
                                                  depth,
                                                  expr->Type().ToString(),
                                                  expr->Name()));
                            return false;
                        }
                        auto column_expression = std::static_pointer_cast<ColumnExpression>(expr);
                        auto column_id = column_expression->binding().column_idx;
                        if (m.contains(column_id)) {
                            LOG_TRACE(fmt::format("Expression depth: {}. Column {} has index.", depth, expr->Name()));
                            return true;
                        } else {
                            LOG_TRACE(fmt::format("Expression depth: {}. Column {} does not have a secondary index. Cannot apply index scan.",
                                                  depth,
                                                  expr->Name()));
                            return false;
                        }
                    };
                    if (HaveLeftColumnAndRightValue(function_expression, sub_expr_depth + 1, is_column_index)) {
                        return expression;
                    } else if (HaveRightColumnAndLeftValue(function_expression, sub_expr_depth + 1, is_column_index)) {
                        Vector<SharedPtr<BaseExpression>> arguments = expression->arguments();
                        std::swap(arguments[0], arguments[1]);
                        auto name_idx = name_iter - IndexScanSupportedCompareFunctionNames.begin();
                        auto function_set_ptr = Catalog::GetFunctionSetByName(query_context_->storage()->catalog(),
                                                                              IndexScanSupportedCompareFunctionNamesCorrespondingReverse[name_idx]);
                        auto scalar_function_set_ptr = static_pointer_cast<ScalarFunctionSet>(function_set_ptr);
                        ScalarFunction func = scalar_function_set_ptr->GetMostMatchFunction(arguments);
                        return MakeShared<FunctionExpression>(std::move(func), std::move(arguments));
                    } else {
                        //  right expression should be value
                        LOG_TRACE(fmt::format("Expression depth: {}. Cannot apply to secondary index: Invalid filter function: {}.",
                                              sub_expr_depth,
                                              function_expression->Name()));
                        return nullptr;
                    }
                } else {
                    // unsupported compare function
                    LOG_TRACE(fmt::format("Expression depth: {}. Unsupported filter function for index scan: {}.",
                                          sub_expr_depth,
                                          function_expression->Name()));
                    return nullptr;
                }
            }
            case ExpressionType::kValue: {
                LOG_TRACE(fmt::format("Expression depth: {}. Value {} appears in depth 0. Maybe need the expression rewrite optimizer.",
                                      sub_expr_depth,
                                      expression->Name()));
                return nullptr;
            }
            case ExpressionType::kCast: {
                LOG_TRACE(fmt::format("Expression depth: {}. Cast expression {} appears in depth 0. Maybe need the expression rewrite optimizer.",
                                      sub_expr_depth,
                                      expression->Name()));
                return nullptr;
            }
            default: {
                LOG_TRACE(fmt::format("Expression depth: {}. Unsupported expression type: In CheckExprIndexState(), unsupported expression {}.",
                                      sub_expr_depth,
                                      expression->Name()));
                return nullptr;
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
                String error_message = fmt::format("Error when trying to apply index scan on expression {}.", expression->Name());
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
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
        // 3. build extra_leftover_filter_
        for (auto &expression : index_filter_leftover_) {
            if (!extra_leftover_filter_) {
                extra_leftover_filter_ = std::move(expression);
            } else {
                // build "and" function
                Vector<SharedPtr<BaseExpression>> arguments;
                arguments.emplace_back(std::move(extra_leftover_filter_));
                arguments.emplace_back(std::move(expression));
                ScalarFunction and_func = and_scalar_function_set_ptr->GetMostMatchFunction(arguments);
                extra_leftover_filter_ = MakeShared<FunctionExpression>(std::move(and_func), std::move(arguments));
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
                String error_message = "Error when trying to apply index scan on \"NOT\" expression: need to be excluded early.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
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

IndexScanFilterExpressionPushDownResult FilterExpressionPushDown::PushDownToIndexScan(QueryContext *query_context,
                                                                                      const BaseTableRef &base_table_ref,
                                                                                      const SharedPtr<BaseExpression> &expression) {
    IndexScanFilterExpressionPushDownMethod filter_expression_push_down_method(query_context, base_table_ref);
    return filter_expression_push_down_method.SolveForIndexScan(expression);
}

class FastRoughFilterEvaluatorTrue final : public FastRoughFilterEvaluator {
public:
    FastRoughFilterEvaluatorTrue() = default;
    ~FastRoughFilterEvaluatorTrue() final = default;
    bool EvaluateInner(TxnTimeStamp, const FastRoughFilter &) const final { return true; }
};

class FastRoughFilterEvaluatorFalse final : public FastRoughFilterEvaluator {
public:
    FastRoughFilterEvaluatorFalse() = default;
    ~FastRoughFilterEvaluatorFalse() final = default;
    bool EvaluateInner(TxnTimeStamp, const FastRoughFilter &) const final { return false; }
};

class FastRoughFilterEvaluatorCombineAnd final : public FastRoughFilterEvaluator {
    UniquePtr<FastRoughFilterEvaluator> left_;
    UniquePtr<FastRoughFilterEvaluator> right_;

public:
    FastRoughFilterEvaluatorCombineAnd(UniquePtr<FastRoughFilterEvaluator> left, UniquePtr<FastRoughFilterEvaluator> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    ~FastRoughFilterEvaluatorCombineAnd() final = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const final {
        return left_->EvaluateInner(query_ts, filter) and right_->EvaluateInner(query_ts, filter);
    }
};

class FastRoughFilterEvaluatorCombineOr final : public FastRoughFilterEvaluator {
    UniquePtr<FastRoughFilterEvaluator> left_;
    UniquePtr<FastRoughFilterEvaluator> right_;

public:
    FastRoughFilterEvaluatorCombineOr(UniquePtr<FastRoughFilterEvaluator> left, UniquePtr<FastRoughFilterEvaluator> right)
        : left_(std::move(left)), right_(std::move(right)) {}
    ~FastRoughFilterEvaluatorCombineOr() final = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const final {
        return left_->EvaluateInner(query_ts, filter) or right_->EvaluateInner(query_ts, filter);
    }
};

// fast "equal" filter
class FastRoughFilterEvaluatorProbabilisticDataFilter final : public FastRoughFilterEvaluator {
    ColumnID column_id_;
    Value value_;

public:
    FastRoughFilterEvaluatorProbabilisticDataFilter(ColumnID column_id, Value value) : column_id_(column_id), value_(std::move(value)) {}
    ~FastRoughFilterEvaluatorProbabilisticDataFilter() final = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const final { return filter.MayContain(query_ts, column_id_, value_); }
};

// fast "range" filter
class FastRoughFilterEvaluatorMinMaxFilter final : public FastRoughFilterEvaluator {
    ColumnID column_id_;
    Value value_;
    FilterCompareType compare_type_;

public:
    FastRoughFilterEvaluatorMinMaxFilter(ColumnID column_id, Value value, FilterCompareType compare_type)
        : column_id_(column_id), value_(std::move(value)), compare_type_(compare_type) {}
    ~FastRoughFilterEvaluatorMinMaxFilter() final = default;
    bool EvaluateInner(TxnTimeStamp query_ts, const FastRoughFilter &filter) const final {
        return filter.MayInRange(column_id_, value_, compare_type_);
    }
};

class FastRoughFilterExpressionPushDownMethod {
public:
    static inline UniquePtr<FastRoughFilterEvaluator> SolveForFastRoughFilter(SharedPtr<BaseExpression> &expression, u32 sub_expr_depth = 0) {
        if (!expression) {
            return ReturnAlwaysTrue();
        }
        // keep old tree structure unchanged
        // build boolean evaluator tree
        // evaluate result: false: can skip block or segment, true: cannot skip block or segment
        // replace some known expression with FastRoughFilterEvaluator
        // replace others as "true"
        // known expression 1: "[cast] x equal value_expr" for ProbabilisticDataFilter, also need to build a val <= x <= val filter for MinMaxFilter
        // known expression 2: "[cast] x compare (>, <, >=, <=) value_expr" for MinMaxFilter
        // known expression 3 : "and" or "or" expression
        switch (expression->type()) {
            case ExpressionType::kFunction: {
                static constexpr std::array<const char *, 4> Case2FunctionNames = {"<", ">", "<=", ">="};
                static constexpr std::array<FilterCompareType, 4> Case2CompareTypes = {FilterCompareType::kLess,
                                                                                       FilterCompareType::kGreater,
                                                                                       FilterCompareType::kLessEqual,
                                                                                       FilterCompareType::kGreaterEqual};
                static constexpr std::array<FilterCompareType, 4> Case2ReverseCompareTypes = {FilterCompareType::kGreater,
                                                                                              FilterCompareType::kLess,
                                                                                              FilterCompareType::kGreaterEqual,
                                                                                              FilterCompareType::kLessEqual};
                auto function_expression = std::static_pointer_cast<FunctionExpression>(expression);
                if (auto const &f_name = function_expression->ScalarFunctionName(); f_name == "AND") {
                    // known expression 3
                    return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(SolveForFastRoughFilter(expression->arguments()[0], sub_expr_depth + 1),
                                                                          SolveForFastRoughFilter(expression->arguments()[1], sub_expr_depth + 1));
                } else if (f_name == "OR") {
                    // known expression 3
                    return MakeUnique<FastRoughFilterEvaluatorCombineOr>(SolveForFastRoughFilter(expression->arguments()[0], sub_expr_depth + 1),
                                                                         SolveForFastRoughFilter(expression->arguments()[1], sub_expr_depth + 1));
                } else if (auto it = std::find(Case2FunctionNames.begin(), Case2FunctionNames.end(), f_name); it != Case2FunctionNames.end()) {
                    // maybe known expression 2
                    auto is_valid_column = [](const SharedPtr<BaseExpression> &expr, u32 depth) -> bool {
                        if (expr->Type().SupportMinMaxFilter()) {
                            return true;
                        } else {
                            // Unsupported type
                            LOG_TRACE(fmt::format("Expression depth: {}. column value type {} does not support MinMax filter. Expression: {}.",
                                                  depth,
                                                  expr->Type().ToString(),
                                                  expr->Name()));
                            return false;
                        }
                    };
                    auto SolveForExpr2 = [](SharedPtr<BaseExpression> &col_expr,
                                            SharedPtr<BaseExpression> &val_expr,
                                            FilterCompareType initial_compare_type) -> UniquePtr<FastRoughFilterEvaluator> {
                        auto val_right = FilterExpressionPushDownHelper::CalcValueResult(val_expr);
                        auto [column_id, value, compare_type] =
                            FilterExpressionPushDownHelper::UnwindCast(col_expr, std::move(val_right), initial_compare_type);
                        switch (compare_type) {
                            case FilterCompareType::kLessEqual:
                            case FilterCompareType::kGreaterEqual: {
                                return MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, std::move(value), compare_type);
                            }
                            case FilterCompareType::kInvalid: // special cast expression, e.g., cast varchar column to int and compare with int
                            case FilterCompareType::kAlwaysTrue: {
                                return ReturnAlwaysTrue();
                            }
                            case FilterCompareType::kAlwaysFalse: {
                                return ReturnAlwaysFalse();
                            }
                            default: {
                                // error
                                String error_message = "Wrong compare type!";
                                LOG_CRITICAL(error_message);
                                UnrecoverableError(error_message);
                                return ReturnAlwaysTrue();
                            }
                        }
                    };
                    if (FilterExpressionPushDownMethodBase::HaveLeftColumnAndRightValue(function_expression, sub_expr_depth + 1, is_valid_column)) {
                        // known expression 2
                        return SolveForExpr2(expression->arguments()[0],
                                             expression->arguments()[1],
                                             Case2CompareTypes[std::distance(Case2FunctionNames.begin(), it)]);
                    } else if (FilterExpressionPushDownMethodBase::HaveRightColumnAndLeftValue(function_expression,
                                                                                               sub_expr_depth + 1,
                                                                                               is_valid_column)) {
                        // known expression 2
                        return SolveForExpr2(expression->arguments()[1],
                                             expression->arguments()[0],
                                             Case2ReverseCompareTypes[std::distance(Case2FunctionNames.begin(), it)]);
                    } else {
                        // unknown expression
                        return ReturnAlwaysTrue();
                    }
                } else if (f_name == "=") {
                    // maybe known expression 1
                    auto is_valid_column = [](const SharedPtr<BaseExpression> &expr, u32 depth) -> bool {
                        if (expr->Type().SupportBloomFilter() or expr->Type().SupportMinMaxFilter()) {
                            return true;
                        } else {
                            // Unsupported type
                            LOG_TRACE(fmt::format("Expression depth: {}. column value type {} does not support BloomFilter. Expression: {}.",
                                                  depth,
                                                  expr->Type().ToString(),
                                                  expr->Name()));
                            return false;
                        }
                    };
                    auto SolveForExpr1 = [](SharedPtr<BaseExpression> &col_expr,
                                            SharedPtr<BaseExpression> &val_expr) -> UniquePtr<FastRoughFilterEvaluator> {
                        auto val_right = FilterExpressionPushDownHelper::CalcValueResult(val_expr);
                        auto [column_id, value, compare_type] =
                            FilterExpressionPushDownHelper::UnwindCast(col_expr, std::move(val_right), FilterCompareType::kEqual);
                        switch (compare_type) {
                            case FilterCompareType::kEqual: {
                                switch (value.type().type()) {
                                    case kBoolean:
                                    case kDecimal: {
                                        return MakeUnique<FastRoughFilterEvaluatorProbabilisticDataFilter>(column_id, std::move(value));
                                    }
                                    case kFloat:
                                    case kDouble: {
                                        auto minmax_filter_le =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kLessEqual);
                                        auto minmax_filter_ge =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kGreaterEqual);
                                        return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(minmax_filter_le),
                                                                                              std::move(minmax_filter_ge));
                                    }
                                    default: {
                                        auto minmax_filter_le =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kLessEqual);
                                        auto minmax_filter_ge =
                                            MakeUnique<FastRoughFilterEvaluatorMinMaxFilter>(column_id, value, FilterCompareType::kGreaterEqual);
                                        auto minmax_filter =
                                            MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(minmax_filter_le), std::move(minmax_filter_ge));
                                        auto bloom_filter = MakeUnique<FastRoughFilterEvaluatorProbabilisticDataFilter>(column_id, std::move(value));
                                        return MakeUnique<FastRoughFilterEvaluatorCombineAnd>(std::move(minmax_filter), std::move(bloom_filter));
                                    }
                                }
                            }
                            case FilterCompareType::kInvalid: // special cast expression, e.g., cast varchar column to int and compare with int
                            case FilterCompareType::kAlwaysTrue: {
                                return ReturnAlwaysTrue();
                            }
                            case FilterCompareType::kAlwaysFalse: {
                                return ReturnAlwaysFalse();
                            }
                            default: {
                                // error
                                String error_message = "Wrong compare type!";
                                LOG_CRITICAL(error_message);
                                UnrecoverableError(error_message);
                                return ReturnAlwaysTrue();
                            }
                        }
                    };
                    if (FilterExpressionPushDownMethodBase::HaveLeftColumnAndRightValue(function_expression, sub_expr_depth + 1, is_valid_column)) {
                        // known expression 1
                        return SolveForExpr1(expression->arguments()[0], expression->arguments()[1]);
                    } else if (FilterExpressionPushDownMethodBase::HaveRightColumnAndLeftValue(function_expression,
                                                                                               sub_expr_depth + 1,
                                                                                               is_valid_column)) {
                        // known expression 1
                        return SolveForExpr1(expression->arguments()[1], expression->arguments()[0]);
                    } else {
                        // unknown expression
                        return ReturnAlwaysTrue();
                    }
                } else if (FilterExpressionPushDownMethodBase::IsValueResultExpression(expression, sub_expr_depth + 1)) {
                    return ReturnValue(expression, sub_expr_depth);
                } else {
                    // unknown expression
                    return ReturnAlwaysTrue();
                }
            }
            case ExpressionType::kValue: {
                return ReturnValue(expression, sub_expr_depth);
            }
            default: {
                // non-value expression
                LOG_TRACE("SolveForFastRoughFilter(): Unknown expression. Return always true.");
                return ReturnAlwaysTrue();
            }
        }
    }

private:
    static inline UniquePtr<FastRoughFilterEvaluator> ReturnAlwaysTrue() { return MakeUnique<FastRoughFilterEvaluatorTrue>(); }

    static inline UniquePtr<FastRoughFilterEvaluator> ReturnAlwaysFalse() { return MakeUnique<FastRoughFilterEvaluatorFalse>(); }

    static inline UniquePtr<FastRoughFilterEvaluator> ReturnValue(SharedPtr<BaseExpression> &expression, u32 sub_expr_depth) {
        auto value = FilterExpressionPushDownHelper::CalcValueResult(expression);
        if (value.type().type() != kBoolean) {
            LOG_TRACE("ReturnValue(): Non-boolean value. Return always true.");
            return ReturnAlwaysTrue();
        } else {
            if (value.GetValue<BooleanT>()) {
                LOG_TRACE("ReturnValue(): Boolean value is true. Return always true.");
                return ReturnAlwaysTrue();
            } else {
                LOG_TRACE("ReturnValue(): Boolean value is false. Return always false.");
                return ReturnAlwaysFalse();
            }
        }
    }
};

UniquePtr<FastRoughFilterEvaluator> FilterExpressionPushDown::PushDownToFastRoughFilter(SharedPtr<BaseExpression> &expression) {
    return FastRoughFilterExpressionPushDownMethod::SolveForFastRoughFilter(expression);
}

} // namespace infinity