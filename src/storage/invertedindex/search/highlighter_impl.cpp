// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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
// limitations under the License.module;

module;

#include "common/analyzer/string_utils.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

module infinity_core:highlighter.impl;

import :highlighter;
import :aho_corasick;
import :analyzer;
import :term;
import :analyzer_pool;
import :rag_analyzer;
import :infinity_exception;

import std.compat;

namespace infinity {

void Highlighter::GetHighlight(const std::string &matching_text, const std::string &raw_text, std::string &output, Analyzer *analyzer) {
    if (matching_text.empty() || raw_text.empty()) {
        output = raw_text;
        return;
    }

    std::vector<std::string> ascii_only_queries;
    std::vector<std::string> unicode_queries;

    TermList matching_terms;
    analyzer->Analyze(matching_text, matching_terms);
    for (auto &term : matching_terms) {
        if (IsASCIIWord(term.text_)) {
            ascii_only_queries.push_back(term.text_);
        } else {
            unicode_queries.push_back(term.text_);
        }
    }

    std::vector<std::pair<size_t, size_t>> matches;
    if (!ascii_only_queries.empty()) {
        GetASCIIWordHighlight(ascii_only_queries, raw_text, matches, analyzer);
    }
    if (!unicode_queries.empty()) {
        GetUnicodeWordHighlight(unicode_queries, raw_text, matches);
    }

    ApplyHighlights(raw_text, matches, output);
}

void Highlighter::GetASCIIWordHighlight(const std::vector<std::string> &query_texts,
                                        const std::string &raw_text,
                                        std::vector<std::pair<size_t, size_t>> &matches,
                                        Analyzer *analyzer) {

    TermList doc_terms;
    analyzer->Analyze(raw_text, doc_terms);
    for (const auto &term : doc_terms) {
        if (std::find(query_texts.begin(), query_texts.end(), term.text_) != query_texts.end()) {
            matches.emplace_back(term.word_offset_, term.end_offset_);
        }
    }
}

void Highlighter::GetUnicodeWordHighlight(const std::vector<std::string> &query_texts,
                                          const std::string &raw_text,
                                          std::vector<std::pair<size_t, size_t>> &matches) {
    for (const auto &query_text : query_texts) {
        size_t start = 0;
        while (true) {
            size_t pos = raw_text.find(query_text, start);
            if (pos == std::string::npos) {
                break;
            }
            matches.emplace_back(pos, pos + query_text.length());
            start = pos + 1;
        }
    }
}

void Highlighter::ApplyHighlights(const std::string &text, const std::vector<std::pair<size_t, size_t>> &matches, std::string &output) {
    if (matches.empty()) {
        output = text;
        return;
    }

    auto merged_matches = MergeMatches(matches);

    output = text;
    for (auto it = merged_matches.rbegin(); it != merged_matches.rend(); ++it) {
        size_t start = it->first;
        size_t end = it->second;

        std::string before = output.substr(0, start);
        std::string highlighted = output.substr(start, end - start);
        std::string after = output.substr(end);

        output = before + pre_tag_ + highlighted + post_tag_ + after;
    }
}

void Highlighter::FindASCIIWords(const std::string &pattern, const std::string &text, std::vector<std::pair<size_t, size_t>> &matches) {
    pcre2_code *re;
    PCRE2_SPTR text_ptr = (PCRE2_SPTR)text.c_str();
    PCRE2_SIZE text_len = text.length();

    int errornumber;
    PCRE2_SIZE erroroffset;

    std::string regex_pattern = "\\b" + EscapeRegex(pattern) + "\\b";
    PCRE2_SPTR regex_ptr = (PCRE2_SPTR)regex_pattern.c_str();

    re = pcre2_compile(regex_ptr, PCRE2_ZERO_TERMINATED, PCRE2_CASELESS, &errornumber, &erroroffset, NULL);
    if (re == NULL) {
        return;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);

    PCRE2_SIZE start_offset = 0;
    int rc;

    while (start_offset < text_len) {
        rc = pcre2_match(re, text_ptr, text_len, start_offset, 0, match_data, NULL);

        if (rc < 0) {
            break;
        }

        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
        size_t start = ovector[0];
        size_t end = ovector[1];

        matches.emplace_back(start, end);
        start_offset = end;

        if (start == end) {
            start_offset++;
        }
    }

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
}

std::string Highlighter::EscapeRegex(const std::string &input) {
    static const std::string special_chars = "\\^$.*+?()[]{}|";
    std::string result;

    for (char c : input) {
        if (special_chars.find(c) != std::string::npos) {
            result += '\\';
        }
        result += c;
    }

    return result;
}

std::vector<std::pair<size_t, size_t>> Highlighter::MergeMatches(const std::vector<std::pair<size_t, size_t>> &matches) {
    if (matches.empty()) {
        return {};
    }

    auto sorted_matches = matches;
    std::sort(sorted_matches.begin(), sorted_matches.end());

    std::vector<std::pair<size_t, size_t>> merged;
    size_t current_start = sorted_matches[0].first;
    size_t current_end = sorted_matches[0].second;

    for (size_t i = 1; i < sorted_matches.size(); ++i) {
        size_t start = sorted_matches[i].first;
        size_t end = sorted_matches[i].second;

        if (start <= current_end) {
            current_end = std::max(current_end, end);
        } else {
            merged.emplace_back(current_start, current_end);
            current_start = start;
            current_end = end;
        }
    }

    merged.emplace_back(current_start, current_end);
    return merged;
}

bool Highlighter::IsASCIIWord(const std::string &word) {
    for (char c : word) {
        if (static_cast<unsigned char>(c) > 127) {
            return false;
        }
    }
    return true;
}
}; // namespace infinity
