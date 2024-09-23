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
import index_filter_evaluators;
import index_filter_expression_info_tree;
import filter_expression_push_down_helper;
import internal_types;
import data_type;
import logical_type;
import query_context;
import infinity_exception;
import expression_type;
import base_expression;
import function_expression;
import cast_expression;
import column_expression;
import value_expression;
import filter_fulltext_expression;
import base_table_ref;
import logger;
import third_party;
import scalar_function;
import scalar_function_set;
import index_base;
import table_index_entry;
import catalog;
import value;
import table_index_meta;
import table_entry;
import txn;
import roaring_bitmap;
import query_node;
import column_index_reader;
import secondary_index_data;
import create_index_info;
import search_options;
import doc_iterator;
import search_driver;
import status;

namespace infinity {

struct ExpressionIndexScanInfo {
    enum class Enum {
        // mysterious expr
        kUnknownExpr,

        // constant value
        kValueExpr,

        // secondary index column
        kSecondaryIndexColumnExprOrAfterCast,
        kVarcharSecondaryIndexColumnExprOrAfterCast,

        // secondary index filter
        kSecondaryIndexValueCompareExpr,
        kValueSecondaryIndexCompareExpr,

        // fulltext filter
        kFilterFulltextExpr,

        // logical expr ("not" is treated as unknown)
        kAndExpr,
        kOrExpr,
    };

    // for index scan
    HashMap<ColumnID, TableIndexEntry *> candidate_column_index_map_;

    inline void InitColumnIndexEntries(TableEntry *table_entry, Txn *txn) {
        const TransactionID txn_id = txn->TxnID();
        const TxnTimeStamp begin_ts = txn->BeginTS();
        for (auto map_guard = table_entry->IndexMetaMap(); auto &[index_name, table_index_meta] : *map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                // skip invalid entry, for example, the index is deleted
                continue;
            }
            const IndexBase *index_base = table_index_entry->index_base();
            if (index_base->index_type_ != IndexType::kSecondary) {
                continue;
            }
            auto column_name = index_base->column_name();
            const ColumnID column_id = table_entry->GetColumnIdByName(column_name);
            if (candidate_column_index_map_.contains(column_id)) {
                LOG_TRACE(fmt::format("InitColumnIndexEntries(): Column {} has multiple secondary indexes. Skipping one.", column_id));
            } else {
                candidate_column_index_map_.emplace(column_id, table_index_entry);
            }
        }
    }

    ExpressionInfoTree<ExpressionIndexScanInfo> BuildTree(const SharedPtr<BaseExpression> &expression, const u32 depth = 0) const {
        ExpressionInfoTree<ExpressionIndexScanInfo> tree;
        tree.src_ptr = &expression;
        switch (expression->type()) {
            case ExpressionType::kValue: {
                tree.info = Enum::kValueExpr;
                break;
            }
            case ExpressionType::kColumn: {
                auto *column_expression = static_cast<const ColumnExpression *>(expression.get());
                const ColumnID column_id = column_expression->binding().column_idx;
                if (candidate_column_index_map_.contains(column_id)) {
                    tree.info = column_expression->Type().type() == LogicalType::kVarchar ? Enum::kVarcharSecondaryIndexColumnExprOrAfterCast
                                                                                          : Enum::kSecondaryIndexColumnExprOrAfterCast;
                }
                break;
            }
            case ExpressionType::kCast: {
                CommonCheckCast(this, tree, expression, depth);
                break;
            }
            case ExpressionType::kFunction: {
                tree.children.reserve(expression->arguments().size());
                for (const auto &child_expression : expression->arguments()) {
                    tree.children.push_back(BuildTree(child_expression, depth + 1));
                }
                // 1. all value
                // 2. and, or
                // 3. col_index, val, >, <, >=, <=, =
                { // 1.
                    bool all_value = true;
                    for (const auto &child : tree.children) {
                        if (child.info != Enum::kValueExpr) {
                            all_value = false;
                            break;
                        }
                    }
                    if (all_value) {
                        tree.info = Enum::kValueExpr;
                        break;
                    }
                }
                { // 2. 3. now not all value
                    auto *function_expression = static_cast<const FunctionExpression *>(expression.get());
                    if (const auto &f_name = function_expression->ScalarFunctionName();
                        std::find(LogicalFunctionNames.begin(), LogicalFunctionNames.end(), f_name) != LogicalFunctionNames.end()) {
                        if (tree.children.size() != 2) {
                            UnrecoverableError("Function argument num != 2");
                        }
                        // 1. all column / unknown children: unknown
                        // 2. other: and / or
                        bool all_column_or_unknown = true;
                        for (const auto &child : tree.children) {
                            switch (child.info) {
                                case Enum::kUnknownExpr:
                                case Enum::kVarcharSecondaryIndexColumnExprOrAfterCast:
                                case Enum::kSecondaryIndexColumnExprOrAfterCast: {
                                    break;
                                }
                                case Enum::kAndExpr:
                                case Enum::kOrExpr:
                                case Enum::kValueExpr:
                                case Enum::kFilterFulltextExpr:
                                case Enum::kSecondaryIndexValueCompareExpr:
                                case Enum::kValueSecondaryIndexCompareExpr: {
                                    all_column_or_unknown = false;
                                    break;
                                }
                            }
                        }
                        if (!all_column_or_unknown) {
                            tree.info = f_name == "AND" ? Enum::kAndExpr : Enum::kOrExpr;
                        }
                    } else if (std::find(CompareFunctionNames.begin(), CompareFunctionNames.end(), f_name) != CompareFunctionNames.end()) {
                        if (tree.children.size() != 2) {
                            UnrecoverableError("Function argument num != 2");
                        }
                        auto check_column_value = [](const Enum col, const Enum val, const bool is_equal_func) -> bool {
                            if (val != Enum::kValueExpr) {
                                return false;
                            }
                            switch (col) {
                                case Enum::kSecondaryIndexColumnExprOrAfterCast: {
                                    return true;
                                }
                                case Enum::kVarcharSecondaryIndexColumnExprOrAfterCast: {
                                    return is_equal_func;
                                }
                                default: {
                                    return false;
                                }
                            }
                        };
                        if (const bool is_equal_func = (f_name == "=");
                            check_column_value(tree.children[0].info, tree.children[1].info, is_equal_func)) {
                            tree.info = Enum::kSecondaryIndexValueCompareExpr;
                        } else if (check_column_value(tree.children[1].info, tree.children[0].info, is_equal_func)) {
                            tree.info = Enum::kValueSecondaryIndexCompareExpr;
                        }
                    }
                }
                break;
            }
            case ExpressionType::kFilterFullText: {
                tree.info = Enum::kFilterFulltextExpr;
                break;
            }
            default: {
                break;
            }
        }
        return tree;
    }
};

class IndexScanFilterExpressionPushDownMethod {
    using TreeT = ExpressionInfoTree<ExpressionIndexScanInfo>;
    using Enum = ExpressionIndexScanInfo::Enum;
    // for index
    QueryContext *query_context_;
    const BaseTableRef *base_table_ref_ptr_;
    TableEntry *table_entry_ptr_ = nullptr;
    ScalarFunctionSet *and_scalar_function_set_ptr_ = nullptr;
    ExpressionIndexScanInfo tree_info_;

public:
    IndexScanFilterExpressionPushDownMethod(QueryContext *query_context, const BaseTableRef *base_table_ref_ptr)
        : query_context_(query_context), base_table_ref_ptr_(base_table_ref_ptr) {
        const auto and_function_set_ptr = Catalog::GetFunctionSetByName(query_context_->storage()->catalog(), "AND");
        and_scalar_function_set_ptr_ = static_cast<ScalarFunctionSet *>(and_function_set_ptr.get());
        // prepare secondary index info
        if (base_table_ref_ptr_) {
            table_entry_ptr_ = base_table_ref_ptr_->table_entry_ptr_;
            tree_info_.InitColumnIndexEntries(table_entry_ptr_, query_context_->GetTxn());
        }
    }

    IndexScanFilterExpressionPushDownResult SolveForIndexScan(const SharedPtr<BaseExpression> &expression) const {
        IndexScanFilterExpressionPushDownResult result;
        if (expression) {
            // build info tree
            const auto original_tree = tree_info_.BuildTree(expression);
            // try split AND children that only use index
            // For index:
            //    1. flatten simple "AND" and "OR", build children first, check children result type
            //    2. classify children by 3 types: column range (secondary index), filter fulltext, and complex logic
            //    3. try to compact column range of same column into one range
            std::tie(result.index_filter_, result.leftover_filter_) = GetIndexFilterAndLeftoverFilterFromTreeNode(original_tree);
        }
        if (result.index_filter_) {
            // build info tree
            const auto index_filter_tree = tree_info_.BuildTree(result.index_filter_);
            result.index_filter_evaluator_ = BuildIndexFilterEvaluator(index_filter_tree);
            // optimize fulltext nodes
            OptimizeFulltextTree(result.index_filter_evaluator_.get());
        } else {
            result.index_filter_evaluator_ = MakeUnique<IndexFilterEvaluatorAllTrue>();
        }
        if (result.leftover_filter_) {
            // build IndexFilterEvaluator for FilterFulltextExpression
            BuildIndexFilterEvaluatorForLeftoverFilterFulltextExpression(result.leftover_filter_);
        }
        return result;
    }

    inline void BuildIndexFilterEvaluatorForLeftoverFilterFulltextExpression(const SharedPtr<BaseExpression> &leftover_filter) const {
        if (!leftover_filter) {
            return;
        }
        switch (leftover_filter->type()) {
            case ExpressionType::kFilterFullText: {
                auto *filter_fulltext_expression = static_cast<FilterFulltextExpression *>(leftover_filter.get());
                filter_fulltext_expression->txn_ = query_context_->GetTxn();
                filter_fulltext_expression->block_index_ = base_table_ref_ptr_->block_index_;
                TreeT fake_tree;
                fake_tree.src_ptr = &leftover_filter;
                fake_tree.info = Enum::kFilterFulltextExpr;
                filter_fulltext_expression->filter_fulltext_evaluator_ = BuildIndexFilterEvaluator(fake_tree);
                OptimizeFulltextTree(filter_fulltext_expression->filter_fulltext_evaluator_.get());
                break;
            }
            default: {
                for (const auto &child_expression : leftover_filter->arguments()) {
                    BuildIndexFilterEvaluatorForLeftoverFilterFulltextExpression(child_expression);
                }
                break;
            }
        }
    }

private:
    //                    index_filter               leftover_filter
    inline Pair<SharedPtr<BaseExpression>, SharedPtr<BaseExpression>> GetIndexFilterAndLeftoverFilterFromTreeNode(const TreeT &tree_node) const {
        Pair<SharedPtr<BaseExpression>, SharedPtr<BaseExpression>> result;
        switch (tree_node.info) {
            case Enum::kVarcharSecondaryIndexColumnExprOrAfterCast:
            case Enum::kSecondaryIndexColumnExprOrAfterCast:
            case Enum::kUnknownExpr: {
                result.second = *tree_node.src_ptr;
                break;
            }
            case Enum::kValueExpr:
            case Enum::kValueSecondaryIndexCompareExpr:
            case Enum::kSecondaryIndexValueCompareExpr:
            case Enum::kFilterFulltextExpr: {
                result.first = *tree_node.src_ptr;
                break;
            }
            case Enum::kAndExpr: {
                if (tree_node.children.size() != 2) {
                    UnrecoverableError("Wrong number of children");
                }
                Vector<SharedPtr<BaseExpression>> index_filter_expressions;
                Vector<SharedPtr<BaseExpression>> leftover_filter_expressions;
                for (const auto &child : tree_node.children) {
                    auto [child_index, child_leftover] = GetIndexFilterAndLeftoverFilterFromTreeNode(child);
                    if (child_index) {
                        index_filter_expressions.push_back(std::move(child_index));
                    }
                    if (child_leftover) {
                        leftover_filter_expressions.push_back(std::move(child_leftover));
                    }
                }
                auto GetAndExpr = [&](Vector<SharedPtr<BaseExpression>> expressions) -> SharedPtr<BaseExpression> {
                    if (expressions.empty()) {
                        return {};
                    }
                    if (expressions.size() == 1) {
                        return std::move(expressions[0]);
                    }
                    if (expressions.size() > 2) {
                        UnrecoverableError("Wrong number of expressions");
                    }
                    // build "and" function
                    ScalarFunction and_func = and_scalar_function_set_ptr_->GetMostMatchFunction(expressions);
                    return MakeShared<FunctionExpression>(std::move(and_func), std::move(expressions));
                };
                result.first = GetAndExpr(std::move(index_filter_expressions));
                result.second = GetAndExpr(std::move(leftover_filter_expressions));
                break;
            }
            case Enum::kOrExpr: {
                if (tree_node.children.size() != 2) {
                    UnrecoverableError("Wrong number of children");
                }
                bool all_index_filter = true;
                for (const auto &child : tree_node.children) {
                    if (const auto [fst, snd] = GetIndexFilterAndLeftoverFilterFromTreeNode(child); snd) {
                        all_index_filter = false;
                        break;
                    }
                }
                if (all_index_filter) {
                    result.first = *tree_node.src_ptr;
                } else {
                    result.second = *tree_node.src_ptr;
                }
                break;
            }
        }
        return result;
    }

    inline UniquePtr<IndexFilterEvaluator> BuildIndexFilterEvaluator(const TreeT &index_filter_tree_node) const {
        switch (index_filter_tree_node.info) {
            case Enum::kVarcharSecondaryIndexColumnExprOrAfterCast:
            case Enum::kSecondaryIndexColumnExprOrAfterCast:
            case Enum::kUnknownExpr: {
                UnrecoverableError("Wrong status!");
                return {};
            }
            case Enum::kValueExpr: {
                const auto val = FilterExpressionPushDownHelper::CalcValueResult(*index_filter_tree_node.src_ptr);
                if (val.type().type() != LogicalType::kBoolean) {
                    UnrecoverableError("Filter Expression must be boolean type!");
                }
                if (val.GetValue<BooleanT>()) {
                    // all true
                    return MakeUnique<IndexFilterEvaluatorAllTrue>();
                }
                // all false
                return MakeUnique<IndexFilterEvaluatorAllFalse>();
            }
            case Enum::kValueSecondaryIndexCompareExpr:
            case Enum::kSecondaryIndexValueCompareExpr: {
                auto *function_expression = static_cast<FunctionExpression *>(index_filter_tree_node.src_ptr->get());
                auto const &f_name = function_expression->ScalarFunctionName();
                constexpr std::array PossibleFunctionNames{"<", ">", "<=", ">=", "="};
                constexpr std::array PossibleCompareTypes{FilterCompareType::kLess,
                                                          FilterCompareType::kGreater,
                                                          FilterCompareType::kLessEqual,
                                                          FilterCompareType::kGreaterEqual,
                                                          FilterCompareType::kEqual};
                constexpr std::array PossibleReverseCompareTypes{FilterCompareType::kGreater,
                                                                 FilterCompareType::kLess,
                                                                 FilterCompareType::kGreaterEqual,
                                                                 FilterCompareType::kLessEqual,
                                                                 FilterCompareType::kEqual};
                const auto it = std::find(PossibleFunctionNames.begin(), PossibleFunctionNames.end(), f_name);
                if (it == PossibleFunctionNames.end()) {
                    UnrecoverableError("Function name not found");
                }
                auto SolveForColVal = [&](SharedPtr<BaseExpression> &col_expr,
                                          SharedPtr<BaseExpression> &val_expr,
                                          FilterCompareType initial_compare_type) -> UniquePtr<IndexFilterEvaluator> {
                    auto val_right = FilterExpressionPushDownHelper::CalcValueResult(val_expr);
                    auto [column_id, value, compare_type] =
                        FilterExpressionPushDownHelper::UnwindCast(col_expr, std::move(val_right), initial_compare_type);
                    switch (compare_type) {
                        case FilterCompareType::kEqual:
                        case FilterCompareType::kLessEqual:
                        case FilterCompareType::kGreaterEqual: {
                            const auto *secondary_index = tree_info_.candidate_column_index_map_.at(column_id);
                            return IndexFilterEvaluatorSecondary::Make(function_expression, column_id, secondary_index, compare_type, value);
                        }
                        case FilterCompareType::kAlwaysTrue: {
                            return MakeUnique<IndexFilterEvaluatorAllTrue>();
                        }
                        case FilterCompareType::kAlwaysFalse: {
                            return MakeUnique<IndexFilterEvaluatorAllFalse>();
                        }
                        default: {
                            // error
                            UnrecoverableError("Wrong compare type!");
                            return {};
                        }
                    }
                };
                switch (index_filter_tree_node.info) {
                    case Enum::kSecondaryIndexValueCompareExpr: {
                        return SolveForColVal(function_expression->arguments()[0],
                                              function_expression->arguments()[1],
                                              PossibleCompareTypes[std::distance(PossibleFunctionNames.begin(), it)]);
                    }
                    case Enum::kValueSecondaryIndexCompareExpr: {
                        return SolveForColVal(function_expression->arguments()[1],
                                              function_expression->arguments()[0],
                                              PossibleReverseCompareTypes[std::distance(PossibleFunctionNames.begin(), it)]);
                    }
                    default: {
                        UnrecoverableError("Wrong expression type!");
                        return {};
                    }
                }
            }
            case Enum::kFilterFulltextExpr: {
                auto *filter_fulltext_expr = static_cast<const FilterFulltextExpression *>(index_filter_tree_node.src_ptr->get());
                auto index_reader = table_entry_ptr_->GetFullTextIndexReader(query_context_->GetTxn());
                EarlyTermAlgo early_term_algo = EarlyTermAlgo::kNaive;
                UniquePtr<QueryNode> query_tree;
                {
                    const Map<String, String> &column2analyzer = index_reader.GetColumn2Analyzer();
                    SearchOptions search_ops(filter_fulltext_expr->options_text_);

                    // option: threshold
                    if (search_ops.options_.contains("threshold")) {
                        RecoverableError(Status::SyntaxError("threshold option should not in filter_fulltext"));
                    }

                    // option: default field
                    auto iter = search_ops.options_.find("default_field");
                    String default_field;
                    if (iter != search_ops.options_.end()) {
                        default_field = iter->second;
                    }

                    // option: block max
                    iter = search_ops.options_.find("block_max");
                    if (iter == search_ops.options_.end() || iter->second == "false") {
                        early_term_algo = EarlyTermAlgo::kNaive;
                    } else if (iter->second == "true" || iter->second == "bmw") {
                        early_term_algo = EarlyTermAlgo::kBMW;
                    } else if (iter->second == "bmm") {
                        early_term_algo = EarlyTermAlgo::kBMM;
                    } else if (iter->second == "compare") {
                        early_term_algo = EarlyTermAlgo::kCompare;
                    } else {
                        RecoverableError(Status::SyntaxError("block_max option must be empty, true, false or compare"));
                    }

                    // option: top n
                    if (search_ops.options_.contains("topn")) {
                        RecoverableError(Status::SyntaxError("topn option should not in filter_fulltext"));
                    }

                    auto query_operator_option = FulltextQueryOperatorOption::kInfinitySyntax;
                    // option: operator
                    if (iter = search_ops.options_.find("operator"); iter != search_ops.options_.end()) {
                        ToLower(iter->second);
                        if (iter->second == "and") {
                            query_operator_option = FulltextQueryOperatorOption::kAnd;
                        } else if (iter->second == "or") {
                            query_operator_option = FulltextQueryOperatorOption::kOr;
                        } else {
                            RecoverableError(Status::SyntaxError(R"(operator option must be "and" or "or".)"));
                        }
                    }

                    SearchDriver search_driver(column2analyzer, default_field, query_operator_option);
                    query_tree = search_driver.ParseSingleWithFields(filter_fulltext_expr->fields_, filter_fulltext_expr->matching_text_);
                    if (!query_tree) {
                        RecoverableError(Status::ParseMatchExprFailed(filter_fulltext_expr->fields_, filter_fulltext_expr->matching_text_));
                    }
                }
                return MakeUnique<IndexFilterEvaluatorFulltext>(filter_fulltext_expr,
                                                                table_entry_ptr_,
                                                                early_term_algo,
                                                                std::move(index_reader),
                                                                std::move(query_tree));
            }
            case Enum::kAndExpr: {
                Vector<UniquePtr<IndexFilterEvaluator>> candidates;
                for (const auto &child : index_filter_tree_node.children) {
                    switch (auto res = BuildIndexFilterEvaluator(child); res->type()) {
                        case IndexFilterEvaluator::Type::kInvalid: {
                            UnrecoverableError("Invalid enum");
                            break;
                        }
                        case IndexFilterEvaluator::Type::kAllFalse: {
                            return MakeUnique<IndexFilterEvaluatorAllFalse>();
                        }
                        case IndexFilterEvaluator::Type::kAllTrue: {
                            // no-op
                            break;
                        }
                        case IndexFilterEvaluator::Type::kAnd:
                        case IndexFilterEvaluator::Type::kOr:
                        case IndexFilterEvaluator::Type::kFulltextIndex:
                        case IndexFilterEvaluator::Type::kSecondaryIndex: {
                            candidates.push_back(std::move(res));
                            break;
                        }
                    }
                }
                if (candidates.empty()) {
                    // all true
                    return MakeUnique<IndexFilterEvaluatorAllTrue>();
                }
                if (candidates.size() == 1) {
                    return std::move(candidates[0]);
                }
                if (candidates.size() > 2) {
                    UnrecoverableError("Too many candidates");
                }
                // merge 2
                return IndexFilterEvaluatorBuildFromAnd(std::move(candidates));
            }
            case Enum::kOrExpr: {
                Vector<UniquePtr<IndexFilterEvaluator>> candidates;
                for (const auto &child : index_filter_tree_node.children) {
                    switch (auto res = BuildIndexFilterEvaluator(child); res->type()) {
                        case IndexFilterEvaluator::Type::kInvalid: {
                            UnrecoverableError("Invalid enum");
                            break;
                        }
                        case IndexFilterEvaluator::Type::kAllFalse: {
                            // no-op
                            break;
                        }
                        case IndexFilterEvaluator::Type::kAllTrue: {
                            return MakeUnique<IndexFilterEvaluatorAllTrue>();
                        }
                        case IndexFilterEvaluator::Type::kAnd:
                        case IndexFilterEvaluator::Type::kOr:
                        case IndexFilterEvaluator::Type::kFulltextIndex:
                        case IndexFilterEvaluator::Type::kSecondaryIndex: {
                            candidates.push_back(std::move(res));
                            break;
                        }
                    }
                }
                if (candidates.empty()) {
                    // all false
                    return MakeUnique<IndexFilterEvaluatorAllFalse>();
                }
                if (candidates.size() == 1) {
                    return std::move(candidates[0]);
                }
                if (candidates.size() > 2) {
                    UnrecoverableError("Too many candidates");
                }
                // merge 2
                return IndexFilterEvaluatorBuildFromOr(std::move(candidates));
            }
        }
    }

    inline static void OptimizeFulltextTree(IndexFilterEvaluator *evaluator) {
        if (!evaluator) {
            return;
        }
        using Type = IndexFilterEvaluator::Type;
        switch (evaluator->type()) {
            case Type::kAnd:
            case Type::kOr: {
                auto *mc = static_cast<const IndexFilterEvaluatorLogicalChildren *>(evaluator);
                OptimizeFulltextTree(mc->fulltext_evaluator_.get());
                for (auto &ptr : mc->other_children_evaluators_) {
                    OptimizeFulltextTree(ptr.get());
                }
                break;
            }
            case Type::kFulltextIndex: {
                auto *ft_filter = static_cast<IndexFilterEvaluatorFulltext *>(evaluator);
                ft_filter->OptimizeQueryTree();
                break;
            }
            case Type::kSecondaryIndex:
            case Type::kAllTrue:
            case Type::kAllFalse: {
                // no-op
                break;
            }
            case Type::kInvalid: {
                UnrecoverableError("Invalid enum");
                break;
            }
        }
    }
};

IndexScanFilterExpressionPushDownResult FilterExpressionPushDown::PushDownToIndexScan(QueryContext *query_context,
                                                                                      const BaseTableRef *base_table_ref_ptr,
                                                                                      const SharedPtr<BaseExpression> &expression) {
    IndexScanFilterExpressionPushDownMethod filter_expression_push_down_method(query_context, base_table_ref_ptr);
    return filter_expression_push_down_method.SolveForIndexScan(expression);
}

void FilterExpressionPushDown::BuildFilterFulltextExpression(QueryContext *query_context,
                                                             const BaseTableRef *base_table_ref_ptr,
                                                             const Vector<SharedPtr<BaseExpression>> &expressions) {
    IndexScanFilterExpressionPushDownMethod filter_expression_push_down_method(query_context, base_table_ref_ptr);
    for (const auto &expr : expressions) {
        filter_expression_push_down_method.BuildIndexFilterEvaluatorForLeftoverFilterFulltextExpression(expr);
    }
}

} // namespace infinity
