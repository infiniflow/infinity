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

#include <cassert>

#define SearchScannerSuffix InfinitySyntax
#include "search_scanner_derived_helper.h"
#undef SearchScannerSuffix

module infinity_core:search_driver.impl;

import :search_driver;
import :stl;
import :term;
import :infinity_exception;
import :status;
import :logger;
import :analyzer;
import :analyzer_pool;
import :query_node;

import std;
import third_party;

import search_scanner;

namespace infinity {

Pair<String, float> ParseField(const std::string_view &field) {
    size_t cap_idx = field.find_first_of('^', 0);
    if (cap_idx == String::npos) {
        return MakePair(String(field), 1.0F);
    } else {
        String field_name(field.substr(0, cap_idx));
        std::string_view field_boost = field.substr(cap_idx + 1);
        float boost = std::stof(String(field_boost));
        return MakePair(std::move(field_name), boost);
    }
}

void ParseFields(const String &fields_str, Vector<Pair<String, float>> &fields) {
    fields.clear();
    if (fields_str.empty()) {
        return;
    }
    size_t begin_idx = 0;
    size_t len = fields_str.length();
    while (begin_idx < len) {
        size_t comma_idx = fields_str.find_first_of(',', begin_idx);
        if (comma_idx == String::npos) {
            auto field = ParseField(fields_str.substr(begin_idx));
            fields.emplace_back(std::move(field));
            break;
        } else {
            auto field = ParseField(fields_str.substr(begin_idx, comma_idx - begin_idx));
            fields.emplace_back(std::move(field));
            begin_idx = comma_idx + 1;
        }
    }
}

UniquePtr<QueryNode> SearchDriver::ParseSingleWithFields(const String &fields_str, const String &query) const {
    UniquePtr<QueryNode> parsed_query_tree;
    Vector<Pair<String, float>> fields;
    ParseFields(fields_str, fields);
    if (fields.empty()) {
        parsed_query_tree = ParseSingle(query);
    } else if (fields.size() == 1) {
        parsed_query_tree = ParseSingle(query, &(fields[0].first));
        if (parsed_query_tree) {
            parsed_query_tree->MultiplyWeight(fields[0].second);
        }
    } else {
        Vector<UniquePtr<QueryNode>> or_children;
        for (auto &[default_field, boost] : fields) {
            if (auto sub_result = ParseSingle(query, &default_field)) {
                sub_result->MultiplyWeight(boost);
                or_children.emplace_back(std::move(sub_result));
            }
        }
        if (or_children.size() == 1) {
            parsed_query_tree = std::move(or_children[0]);
        } else if (!or_children.empty()) {
            parsed_query_tree = MakeUnique<OrQueryNode>();
            static_cast<OrQueryNode *>(parsed_query_tree.get())->children_ = std::move(or_children);
        }
    }
#ifdef INFINITY_DEBUG
    {
        OStringStream oss;
        oss << "Query tree (without filter):\n";
        if (parsed_query_tree) {
            parsed_query_tree->PrintTree(oss);
        } else {
            oss << "Empty query tree!\n";
        }
        LOG_DEBUG(std::move(oss).str());
    }
#endif
    return parsed_query_tree;
}

inline TermList GetTermListFromAnalyzer(const String &analyzer_name, Analyzer *analyzer, const String &query_str) {
    TermList result;
    Term input_term;
    input_term.text_ = query_str;
    TermList temp_output_terms;
    analyzer->Analyze(input_term, temp_output_terms);
    // remove duplicates and only keep the root words for query
    constexpr u32 INVALID_TERM_OFFSET = -1;
    Term last_term;
    last_term.word_offset_ = INVALID_TERM_OFFSET;
    for (const Term &term : temp_output_terms) {
        if (last_term.word_offset_ != INVALID_TERM_OFFSET) {
            assert(term.word_offset_ >= last_term.word_offset_);
        }
        if (last_term.word_offset_ != term.word_offset_) {
            if (last_term.word_offset_ != INVALID_TERM_OFFSET) {
                result.emplace_back(last_term);
            }
            last_term.text_ = term.text_;
            last_term.word_offset_ = term.word_offset_;
        } else {
            if (term.text_.size() < last_term.text_.size()) {
                last_term.text_ = term.text_;
            }
        }
    }
    if (last_term.word_offset_ != INVALID_TERM_OFFSET) {
        result.emplace_back(last_term);
    }
    if (result.empty()) {
        LOG_TRACE(std::format("{}: Analyzer {} analyzes following text as empty terms: {}", __func__, analyzer_name, query_str));
    }
    return result;
}

inline String GetAnalyzerName(const String &field, const Map<String, String> &field2analyzer) {
    if (!field.empty()) {
        if (const auto it = field2analyzer.find(field); it != field2analyzer.end()) {
            return it->second;
        }
    }
    return "standard";
}

inline static const auto keyword_analyzer_name_int = AnalyzerPool::AnalyzerNameToInt(AnalyzerPool::KEYWORD.data());

UniquePtr<QueryNode> SearchDriver::ParseSingle(const String &query, const String *default_field_ptr) const {
    std::istringstream iss(query);
    if (!iss.good()) {
        return nullptr;
    }
    if (!default_field_ptr) {
        default_field_ptr = &default_field_;
    }
    const auto &default_field = *default_field_ptr;
    const auto default_analyzer_name = GetAnalyzerName(default_field, field2analyzer_);
    if (const auto default_analyzer_name_int = AnalyzerPool::AnalyzerNameToInt(default_analyzer_name.c_str());
        default_analyzer_name_int != keyword_analyzer_name_int && operator_option_ == FulltextQueryOperatorOption::kInfinitySyntax) {
        // use parser
        UniquePtr<QueryNode> result;
        const auto scanner = MakeUnique<SearchScannerInfinitySyntax>(&iss);
        const auto parser = MakeUnique<SearchParser>(*scanner, *this, *default_field_ptr, result);
        if (constexpr int accept = 0; parser->parse() != accept) {
            return nullptr;
        }
        return result;
    } else {
        // use analyzer for the whole query string
        auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(default_analyzer_name);
        if (!status.ok()) {
            RecoverableError(std::move(status));
        }
        TermList terms = GetTermListFromAnalyzer(default_analyzer_name, analyzer.get(), query);
        if (terms.empty()) {
            return nullptr;
        }
        if (terms.size() == 1 && default_analyzer_name_int != keyword_analyzer_name_int) {
            auto q = MakeUnique<TermQueryNode>();
            q->term_ = terms.front().text_;
            q->column_ = default_field;
            return q;
        }
        UniquePtr<MultiQueryNode> multi_query;
        if (default_analyzer_name_int == keyword_analyzer_name_int) {
            multi_query = MakeUnique<KeywordQueryNode>();
        } else if (operator_option_ == FulltextQueryOperatorOption::kOr) {
            multi_query = MakeUnique<OrQueryNode>();
        } else if (operator_option_ == FulltextQueryOperatorOption::kAnd) {
            multi_query = MakeUnique<AndQueryNode>();
        }
        for (const auto &term : terms) {
            auto subquery = MakeUnique<TermQueryNode>();
            subquery->term_ = term.text_;
            subquery->column_ = default_field;
            multi_query->Add(std::move(subquery));
        }
        return multi_query;
    }
}

UniquePtr<QueryNode>
SearchDriver::AnalyzeAndBuildQueryNode(const String &field, const String &text, const bool from_quoted, const unsigned long slop) const {
    assert(operator_option_ == FulltextQueryOperatorOption::kInfinitySyntax);
    if (text.empty()) {
        LOG_TRACE(std::format("{} : Empty query text: {}", __func__, text));
        return nullptr;
    }
    // 1. analyze
    const auto analyzer_name = GetAnalyzerName(field, field2analyzer_);
    auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(analyzer_name);
    if (!status.ok()) {
        RecoverableError(std::move(status));
    }
    TermList terms = GetTermListFromAnalyzer(analyzer_name, analyzer.get(), text);
    if (terms.empty()) {
        return nullptr;
    }
    if (AnalyzerPool::AnalyzerNameToInt(analyzer_name.c_str()) == keyword_analyzer_name_int) {
        auto result = MakeUnique<KeywordQueryNode>();
        for (const auto &term : terms) {
            auto subquery = MakeUnique<TermQueryNode>();
            subquery->term_ = term.text_;
            subquery->column_ = field;
            result->Add(std::move(subquery));
        }
        return result;
    }

    // 2. build query node
    if (terms.empty()) {
        auto result = MakeUnique<TermQueryNode>();
        result->term_ = text;
        result->column_ = field;
        return result;
    } else if (terms.size() == 1) {
        auto result = MakeUnique<TermQueryNode>();
        result->term_ = terms.front().text_;
        result->column_ = field;
        return result;
    } else {
        if (from_quoted) {
            auto result = MakeUnique<PhraseQueryNode>();
            for (const auto &term : terms) {
                result->AddTerm(term.text_);
            }
            result->column_ = field;
            result->slop_ = slop;
            return result;
        } else {
            auto result = MakeUnique<OrQueryNode>();
            for (const auto &term : terms) {
                auto subquery = MakeUnique<TermQueryNode>();
                subquery->term_ = term.text_;
                subquery->column_ = field;
                result->Add(std::move(subquery));
            }
            return result;
        }
    }
}

// Unescape reserved characters per https://www.elastic.co/guide/en/elasticsearch/reference/current/query-dsl-query-string-query.html
// Shall keep sync with ESCAPEABLE in search_lexer.l
// [\x20()^"'~*?:\\]
String SearchDriver::Unescape(const String &text) {
    String result;
    result.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\\' && i + 1 < text.size()) {
            switch (text[i + 1]) {
                case ' ':
                case '(':
                case ')':
                case '^':
                case '"':
                case '\'':
                case '~':
                case '*':
                case '?':
                case ':':
                case '\\':
                    result.push_back(text[i + 1]);
                    ++i;
                    break;
                default:
                    result.push_back(text[i]);
                    break;
            }
        } else {
            result.push_back(text[i]);
        }
    }
    return result;
}

} // namespace infinity
