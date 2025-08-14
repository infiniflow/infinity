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

module infinity_core:filter_expression_push_down_indexscanfilter.impl;

import :stl;
import :index_filter_evaluators;
import :index_filter_expression_info_tree;
import :filter_expression_push_down_helper;
import :query_context;
import :infinity_exception;
import :expression_type;
import :base_expression;
import :function_expression;
import :cast_expression;
import :column_expression;
import :value_expression;
import :filter_fulltext_expression;
import :base_table_ref;
import :scalar_function;
import :scalar_function_set;
import :index_base;
import :new_catalog;
import :value;
import :meta_info;
import :roaring_bitmap;
import :query_node;
import :column_index_reader;
import :secondary_index_data;
import :doc_iterator;
import :search_driver;
import :status;
import :parse_fulltext_options;
import :block_index;
import :new_txn;
import :table_index_meeta;
import :segment_index_meta;
import :table_meeta;
import :db_meeta;
import :kv_store;

import std;
import third_party;

import internal_types;
import data_type;
import logical_type;
import create_index_info;
import search_options;

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
    TableMeeta *table_meta_ = nullptr;
    HashMap<ColumnID, SharedPtr<TableIndexMeeta>> new_candidate_column_index_map_;

    inline void NewInitColumnIndexEntries(TableInfo *table_info, NewTxn *new_txn, BaseTableRef *base_table_ref) {
        Status status;
        if (!base_table_ref->block_index_->table_meta_) {
            base_table_ref->block_index_->table_meta_ = MakeUnique<TableMeeta>(table_info->db_id_, table_info->table_id_, new_txn);
        }
        table_meta_ = base_table_ref->block_index_->table_meta_.get();
        auto &table_index_meta_map = base_table_ref->block_index_->table_index_meta_map_;

        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta_->GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            RecoverableError(status);
        }
        if (!base_table_ref->index_index_) {
            base_table_ref->index_index_ = MakeShared<IndexIndex>();
        }
        for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
            const String &index_id_str = (*index_id_strs_ptr)[i];
            if (table_index_meta_map.size() <= i) {
                auto table_index_meta = MakeShared<TableIndexMeeta>(index_id_str, *table_meta_);
                table_index_meta_map.emplace_back(std::move(table_index_meta));
            }
            SharedPtr<TableIndexMeeta> &it = table_index_meta_map[i];

            SharedPtr<IndexBase> index_base;
            std::tie(index_base, status) = it->GetIndexBase();
            if (!status.ok()) {
                RecoverableError(status);
            }
            if (index_base->index_type_ != IndexType::kSecondary) {
                continue;
            }
            auto column_name = index_base->column_name();
            ColumnID column_id = 0;
            std::tie(column_id, status) = table_meta_->GetColumnIDByColumnName(column_name);
            if (!status.ok()) {
                RecoverableError(status);
            }
            if (new_candidate_column_index_map_.contains(column_id)) {
                LOG_TRACE(fmt::format("NewInitColumnIndexEntries(): Column {} has multiple secondary indexes. Skipping one.", column_id));
            } else {
                new_candidate_column_index_map_.emplace(column_id, it);
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
                if (new_candidate_column_index_map_.contains(column_id)) {
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
    TableInfo *table_info_ = nullptr;
    ScalarFunctionSet *and_scalar_function_set_ptr_ = nullptr;
    ExpressionIndexScanInfo tree_info_;

public:
    explicit IndexScanFilterExpressionPushDownMethod(QueryContext *query_context, const BaseTableRef *base_table_ref_ptr)
        : query_context_(query_context), base_table_ref_ptr_(base_table_ref_ptr) {}

    void Init() {
        const auto and_function_set_ptr = NewCatalog::GetFunctionSetByName(query_context_->storage()->new_catalog(), "AND");
        and_scalar_function_set_ptr_ = static_cast<ScalarFunctionSet *>(and_function_set_ptr.get());
        // prepare secondary index info
        if (base_table_ref_ptr_) {
            table_info_ = base_table_ref_ptr_->table_info_.get();
            tree_info_.NewInitColumnIndexEntries(table_info_, query_context_->GetNewTxn(), const_cast<BaseTableRef *>(base_table_ref_ptr_));
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
                static constexpr std::array<std::string, 5> PossibleFunctionNames{"<", ">", "<=", ">=", "="};
                constexpr std::array<FilterCompareType, 5> PossibleCompareTypes{FilterCompareType::kLess,
                                                                                FilterCompareType::kGreater,
                                                                                FilterCompareType::kLessEqual,
                                                                                FilterCompareType::kGreaterEqual,
                                                                                FilterCompareType::kEqual};
                constexpr std::array<FilterCompareType, 5> PossibleReverseCompareTypes{FilterCompareType::kGreater,
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
                            SharedPtr<TableIndexMeeta> secondary_index = tree_info_.new_candidate_column_index_map_.at(column_id);
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
                SharedPtr<IndexReader> index_reader;
                NewTxn *new_txn = query_context_->GetNewTxn();
                Status status = new_txn->GetFullTextIndexReader(*table_info_->db_name_, *table_info_->table_name_, index_reader);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Get full text index reader error: {}", status.message()));
                }

                EarlyTermAlgo early_term_algo = EarlyTermAlgo::kAuto;
                UniquePtr<QueryNode> query_tree;
                MinimumShouldMatchOption minimum_should_match_option;
                f32 score_threshold = {};
                FulltextSimilarity ft_similarity = FulltextSimilarity::kBM25;
                BM25Params bm25_params;
                Vector<String> index_names;
                {
                    SearchOptions search_ops(filter_fulltext_expr->options_text_);

                    // option: default field
                    auto iter = search_ops.options_.find("default_field");
                    String default_field;
                    if (iter != search_ops.options_.end()) {
                        default_field = iter->second;
                    }

                    // option: block max
                    iter = search_ops.options_.find("block_max");
                    if (iter == search_ops.options_.end() || iter->second == "auto") {
                        early_term_algo = EarlyTermAlgo::kAuto;
                    } else if (iter->second == "batch") {
                        early_term_algo = EarlyTermAlgo::kBatch;
                    } else if (iter->second == "false") {
                        early_term_algo = EarlyTermAlgo::kNaive;
                    } else if (iter->second == "true" || iter->second == "bmw") {
                        early_term_algo = EarlyTermAlgo::kBMW;
                    } else if (iter->second == "compare") {
                        early_term_algo = EarlyTermAlgo::kCompare;
                    } else {
                        RecoverableError(Status::SyntaxError("block_max option must be empty, auto, batch, false, true, bmw, or compare"));
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

                    // option: minimum_should_match
                    if (iter = search_ops.options_.find("minimum_should_match"); iter != search_ops.options_.end()) {
                        minimum_should_match_option = ParseMinimumShouldMatchOption(iter->second);
                    }

                    // option: threshold
                    if (iter = search_ops.options_.find("threshold"); iter != search_ops.options_.end()) {
                        score_threshold = DataType::StringToValue<FloatT>(iter->second);
                    }

                    // option: similarity
                    if (iter = search_ops.options_.find("similarity"); iter != search_ops.options_.end()) {
                        String ft_sim = iter->second;
                        ToLower(ft_sim);
                        if (ft_sim == "bm25") {
                            ft_similarity = FulltextSimilarity::kBM25;
                        } else if (ft_sim == "boolean") {
                            ft_similarity = FulltextSimilarity::kBoolean;
                        } else {
                            RecoverableError(Status::SyntaxError(R"(similarity option must be "BM25" or "boolean".)"));
                        }
                    }
                    // option: bm25_params
                    if (iter = search_ops.options_.find("bm25_param_k1"); iter != search_ops.options_.end()) {
                        const auto k1_v = DataType::StringToValue<FloatT>(iter->second);
                        if (k1_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_k1 must be a non-negative float. default value: 1.2"));
                        }
                        bm25_params.k1 = k1_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_b"); iter != search_ops.options_.end()) {
                        const auto b_v = DataType::StringToValue<FloatT>(iter->second);
                        if (b_v < 0.0f || b_v > 1.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_b must be in the range [0.0f, 1.0f]. default value: 0.75"));
                        }
                        bm25_params.b = b_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta"); iter != search_ops.options_.end()) {
                        const auto delta_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta must be a non-negative float. default value: 0.0"));
                        }
                        bm25_params.delta_term = delta_v;
                        bm25_params.delta_phrase = delta_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta_term"); iter != search_ops.options_.end()) {
                        const auto delta_term_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_term_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta_term must be a non-negative float. default value: 0.0"));
                        }
                        bm25_params.delta_term = delta_term_v;
                    }
                    if (iter = search_ops.options_.find("bm25_param_delta_phrase"); iter != search_ops.options_.end()) {
                        const auto delta_phrase_v = DataType::StringToValue<FloatT>(iter->second);
                        if (delta_phrase_v < 0.0f) {
                            RecoverableError(Status::SyntaxError("bm25_param_delta_phrase must be a non-negative float. default value: 0.0"));
                        }
                        bm25_params.delta_phrase = delta_phrase_v;
                    }

                    // option: indexes
                    if (iter = search_ops.options_.find("indexes"); iter != search_ops.options_.end()) {
                        String indexes_text = iter->second;
                        ToLower(indexes_text);
                        SizeT begin_idx = 0;
                        SizeT len = indexes_text.length();
                        while (begin_idx < len) {
                            SizeT comma_idx = indexes_text.find_first_of(',', begin_idx);
                            if (comma_idx == String::npos) {
                                auto index_name = indexes_text.substr(begin_idx);
                                index_names.emplace_back(index_name);
                                break;
                            } else {
                                auto index_name = indexes_text.substr(begin_idx, comma_idx - begin_idx);
                                index_names.emplace_back(std::move(index_name));
                                begin_idx = comma_idx + 1;
                            }
                        }
                    }

                    Map<String, String> column2analyzer = index_reader->GetColumn2Analyzer(index_names);
                    SearchDriver search_driver(column2analyzer, default_field, query_operator_option);
                    query_tree = search_driver.ParseSingleWithFields(filter_fulltext_expr->fields_, filter_fulltext_expr->matching_text_);
                    if (!query_tree) {
                        RecoverableError(Status::ParseMatchExprFailed(filter_fulltext_expr->fields_, filter_fulltext_expr->matching_text_));
                    }
                }
                return MakeUnique<IndexFilterEvaluatorFulltext>(filter_fulltext_expr,
                                                                table_info_,
                                                                early_term_algo,
                                                                std::move(index_reader),
                                                                std::move(query_tree),
                                                                std::move(minimum_should_match_option),
                                                                score_threshold,
                                                                ft_similarity,
                                                                bm25_params,
                                                                std::move(index_names));
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
    filter_expression_push_down_method.Init();
    return filter_expression_push_down_method.SolveForIndexScan(expression);
}

void FilterExpressionPushDown::BuildFilterFulltextExpression(QueryContext *query_context,
                                                             const BaseTableRef *base_table_ref_ptr,
                                                             const Vector<SharedPtr<BaseExpression>> &expressions) {
    IndexScanFilterExpressionPushDownMethod filter_expression_push_down_method(query_context, base_table_ref_ptr);
    filter_expression_push_down_method.Init();
    for (const auto &expr : expressions) {
        filter_expression_push_down_method.BuildIndexFilterEvaluatorForLeftoverFilterFulltextExpression(expr);
    }
}

} // namespace infinity
