// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

#include "common/utility/sparse_gram.h"
#include "planner/optimizer/regex_literal_extractor.h"

module infinity_core:regex_index_rewrite.impl;

import :regex_index_rewrite;
import :base_expression;
import :base_table_ref;
import :expression_type;
import :function_expression;
import :cast_expression;
import :column_expression;
import :filter_fulltext_expression;
import :filter_expression_push_down_helper;
import :query_context;
import :new_txn;
import :new_catalog;
import :column_index_reader;
import :meta_info;
import :scalar_function;
import :scalar_function_set;
import :utility;

import std;
import third_party;
import logical_type;

namespace infinity {

namespace {

// A run is turned into an index filter, so the number of runs bounds the number
// of filters the plan carries. The longest runs are kept: they are the ones with
// the most grams.
constexpr size_t kMaxRunsPerPredicate = 4;

struct RewriteContext {
    ScalarFunctionSet *and_function_set_{nullptr};
    ScalarFunctionSet *or_function_set_{nullptr};
    // Only columns whose full-text index was built with a sparse gram analyzer,
    // mapped to the parameters that analyzer was built with.
    std::unordered_map<std::string, SparseGramParams> column_params_;
};

std::shared_ptr<BaseExpression> UnwrapCast(const std::shared_ptr<BaseExpression> &expression) {
    std::shared_ptr<BaseExpression> current = expression;
    while (current && current->type() == ExpressionType::kCast && current->arguments().size() == 1) {
        current = current->arguments()[0];
    }
    return current;
}

std::shared_ptr<BaseExpression> MakeAnd(RewriteContext &context, std::vector<std::shared_ptr<BaseExpression>> expressions) {
    if (expressions.empty()) {
        return nullptr;
    }
    // `AND` only has a binary overload, so the conjunction is folded into a
    // left deep chain rather than one n-ary node.
    std::shared_ptr<BaseExpression> result = std::move(expressions.front());
    for (size_t i = 1; i < expressions.size(); ++i) {
        std::vector<std::shared_ptr<BaseExpression>> pair{std::move(result), std::move(expressions[i])};
        ScalarFunction and_function = context.and_function_set_->GetMostMatchFunction(pair);
        result = std::make_shared<FunctionExpression>(std::move(and_function), std::move(pair));
    }
    return result;
}

std::shared_ptr<BaseExpression> MakeFilterFulltext(const std::string &column_name, const std::string &run) {
    // Boolean similarity: the filter only decides which rows survive, and a
    // score would make the plan pay for BM25 that nothing reads.
    return std::make_shared<FilterFulltextExpression>(column_name, run, "operator=and;similarity=boolean");
}

/// Rewrite a `regex()` predicate, or return nullptr when there is nothing to do.
std::shared_ptr<BaseExpression> TryRewriteRegex(RewriteContext &context, const std::shared_ptr<BaseExpression> &expression) {
    if (expression->type() != ExpressionType::kFunction) {
        return nullptr;
    }
    auto *function_expression = static_cast<FunctionExpression *>(expression.get());
    if (function_expression->ScalarFunctionName() != "regex" || function_expression->arguments().size() != 2) {
        return nullptr;
    }
    const auto column_expression = UnwrapCast(function_expression->arguments()[0]);
    if (!column_expression || column_expression->type() != ExpressionType::kColumn) {
        return nullptr;
    }
    const std::string &column_name = static_cast<const ColumnExpression *>(column_expression.get())->column_name();
    const auto params_it = context.column_params_.find(column_name);
    if (params_it == context.column_params_.end()) {
        return nullptr;
    }
    const SparseGramParams &params = params_it->second;

    // The pattern has to be a constant: a column-supplied pattern says nothing
    // about the rows it will be evaluated against.
    if (function_expression->arguments()[1]->type() != ExpressionType::kValue) {
        return nullptr;
    }
    std::string pattern;
    try {
        pattern = FilterExpressionPushDownHelper::CalcValueResult(function_expression->arguments()[1]).ToString();
    } catch (...) {
        // An unreadable constant is not a reason to fail the query: leave the
        // predicate alone and let the regular expression answer it.
        return nullptr;
    }
    if (pattern.empty()) {
        return nullptr;
    }

    // Case folding inside the pattern is handled by the extractor, which also
    // knows that only ASCII can be folded and that CJK is caseless.
    const auto extracted = ExtractRegexLiteralRuns(pattern, false, params.fold);
    if (extracted.runs.empty()) {
        return nullptr;
    }

    // Keep the runs that the analyzer would actually turn into grams. A run it
    // produces no gram for must not be pushed down: an empty full-text query is
    // a parse error rather than an empty result.
    std::vector<const RegexLiteralRun *> usable;
    for (const auto &run : extracted.runs) {
        if (run.lowered && !params.fold) {
            continue;
        }
        if (!HasAnySparseGram(run.text, params)) {
            continue;
        }
        usable.push_back(&run);
    }
    if (usable.empty()) {
        return nullptr;
    }
    std::stable_sort(usable.begin(), usable.end(), [](const RegexLiteralRun *left, const RegexLiteralRun *right) {
        return left->text.size() > right->text.size();
    });
    if (usable.size() > kMaxRunsPerPredicate) {
        usable.resize(kMaxRunsPerPredicate);
    }

    std::vector<std::shared_ptr<BaseExpression>> conjuncts;
    conjuncts.reserve(usable.size() + 1);
    for (const auto *run : usable) {
        conjuncts.push_back(MakeFilterFulltext(column_name, run->text));
    }
    // The predicate itself stays, as the residual filter that verifies the
    // candidates the grams let through.
    conjuncts.push_back(expression);
    return MakeAnd(context, std::move(conjuncts));
}

std::shared_ptr<BaseExpression> RewriteNode(RewriteContext &context, const std::shared_ptr<BaseExpression> &expression) {
    if (!expression) {
        return expression;
    }
    if (auto replaced = TryRewriteRegex(context, expression)) {
        return replaced;
    }
    if (expression->type() != ExpressionType::kFunction) {
        return expression;
    }
    auto *function_expression = static_cast<FunctionExpression *>(expression.get());
    const std::string &name = function_expression->ScalarFunctionName();
    const bool is_and = name == "AND" || name == "and";
    const bool is_or = name == "OR" || name == "or";
    if (!is_and && !is_or) {
        return expression;
    }
    ScalarFunctionSet *function_set = is_and ? context.and_function_set_ : context.or_function_set_;
    if (function_set == nullptr) {
        return expression;
    }
    // Only boolean combinators are descended into: a `regex()` nested inside an
    // arithmetic or a comparison says nothing about which rows are returned.
    std::vector<std::shared_ptr<BaseExpression>> arguments;
    arguments.reserve(function_expression->arguments().size());
    bool changed = false;
    for (const auto &argument : function_expression->arguments()) {
        auto rewritten = RewriteNode(context, argument);
        changed = changed || rewritten != argument;
        arguments.push_back(std::move(rewritten));
    }
    if (!changed) {
        return expression;
    }
    ScalarFunction function = function_set->GetMostMatchFunction(arguments);
    return std::make_shared<FunctionExpression>(std::move(function), std::move(arguments));
}

} // namespace

std::shared_ptr<BaseExpression>
RegexIndexRewrite::Rewrite(QueryContext *query_context, const BaseTableRef *base_table_ref, const std::shared_ptr<BaseExpression> &expression) {
    if (query_context == nullptr || base_table_ref == nullptr || !expression) {
        return expression;
    }
    const TableInfo *table_info = base_table_ref->table_info_.get();
    if (table_info == nullptr) {
        return expression;
    }

    std::shared_ptr<IndexReader> index_reader;
    // A table without a readable full-text index is not an error here: the
    // predicate simply keeps whatever plan it would have had.
    if (!query_context->GetNewTxn()->GetFullTextIndexReader(*table_info->db_name_, *table_info->table_name_, index_reader).ok() ||
        index_reader == nullptr) {
        return expression;
    }

    RewriteContext context;
    for (const auto &[column_name, index2analyzer] : index_reader->GetColumn2Analyzer()) {
        for (const auto &[index_name, analyzer_name] : index2analyzer) {
            SparseGramParams params;
            if (!ParseSparseGramAnalyzerName(analyzer_name, params)) {
                continue;
            }
            context.column_params_[column_name] = params;
            break;
        }
    }
    if (context.column_params_.empty()) {
        return expression;
    }

    auto *catalog = query_context->storage()->new_catalog();
    if (const auto and_set = NewCatalog::GetFunctionSetByName(catalog, "AND"); and_set) {
        context.and_function_set_ = static_cast<ScalarFunctionSet *>(and_set.get());
    }
    if (const auto or_set = NewCatalog::GetFunctionSetByName(catalog, "OR"); or_set) {
        context.or_function_set_ = static_cast<ScalarFunctionSet *>(or_set.get());
    }
    if (context.and_function_set_ == nullptr || context.or_function_set_ == nullptr) {
        return expression;
    }

    return RewriteNode(context, expression);
}

} // namespace infinity
