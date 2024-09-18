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

#include <cstddef>
#include <iostream>
#include <vector>

#include "common/analyzer/string_utils.h"

module highlighter;

import stl;
import aho_corasick;
import analyzer;
import term;

namespace infinity {

Highlighter::Highlighter() {
    Set<String> patterns;
    patterns.insert(".");
    patterns.insert(",");
    patterns.insert("?");
    patterns.insert("!");
    patterns.insert(";");
    patterns.insert("\n");
    patterns.insert("，");
    patterns.insert("。");
    patterns.insert("？");
    patterns.insert("！");
    patterns.insert("；");
    Vector<String> pattern_vector(patterns.begin(), patterns.end());
    sentence_delimiter_.Build(pattern_vector);
}

void Highlighter::GetHighlightWithoutStemmer(const Vector<String> &query, const String &raw_text, String &output) {
    const u32 max_results = 1024;
    Vector<AhoCorasick::ResultType> matches(max_results + 1);
    auto num_results = sentence_delimiter_.Find(raw_text, matches.data(), max_results);
    Vector<String> sentences;
    sentences.reserve(num_results);
    std::size_t last_position = 0;
    for (u32 i = 0; i < num_results; ++i) {
        AhoCorasick::ResultType r = matches[i];
        if (r.position > 0) {
            std::size_t end_offset = r.position - last_position + r.length;
            while (raw_text[last_position + end_offset - 1] == '\n') {
                --end_offset;
            };
            if (end_offset == 0)
                continue;
            sentences.push_back(raw_text.substr(last_position, end_offset));
            last_position = r.position + r.length;
        }
    }
    Set<String> patterns;
    for (auto &query_term : query) {
        patterns.insert(ToLowerString(query_term));
    }
    Vector<String> pattern_vector(patterns.begin(), patterns.end());
    AhoCorasick automaton;
    automaton.Build(pattern_vector);
    std::size_t last_sentence_pos = 0;
    for (unsigned i = 0; i < sentences.size(); ++i) {
        String &sentence_raw = sentences[i];
        String sentence = ToLowerString(sentence_raw);

        num_results = automaton.Find(sentence, matches.data(), max_results);
        if (num_results > 0) {
            last_position = 0;
            if (i > (last_sentence_pos + 1))
                output.append("...");

            for (u32 i = 0; i < num_results; ++i) {
                AhoCorasick::ResultType r = matches[i];
                output.append(sentence_raw.substr(last_position, r.position - last_position));
                output.append("<em>");
                output.append(pattern_vector[r.value]);
                output.append("</em>");
                last_position = r.position + r.length;
            }
            output.append(sentence_raw.substr(last_position, sentence_raw.size() - last_position));
            last_sentence_pos = i;
        }
    }
}

void Highlighter::GetHighlightWithStemmer(const Vector<String> &query, const String &raw_text, String &output, Analyzer *analyzer) {
    analyzer->SetCharOffset(true);
    TermList term_list;
    analyzer->Analyze(raw_text, term_list);
    std::sort(term_list.begin(), term_list.end(), [](const Term &lhs, const Term &rhs) noexcept { return lhs.text_ < rhs.text_; });

    TermList hit_list;
    for (auto &query_term : query) {
        auto lower = std::lower_bound(term_list.begin(), term_list.end(), query_term, [](auto element, auto query_term) {
            return element.text_ < query_term;
        });
        for (auto iter = lower; iter != term_list.end(); ++iter) {
            if (iter->text_ == query_term) {
                hit_list.push_back(*iter);
            }
        }
    }
    std::sort(hit_list.begin(), hit_list.end(), [](const Term &lhs, const Term &rhs) noexcept { return lhs.word_offset_ < rhs.word_offset_; });

    const u32 max_results = 1024;
    Vector<AhoCorasick::ResultType> matches(max_results + 1);
    auto num_results = sentence_delimiter_.Find(raw_text, matches.data(), max_results);
    Vector<std::size_t> sentence_boundaries;
    sentence_boundaries.reserve(num_results);
    std::size_t last_position = 0;
    for (u32 i = 0; i < num_results; ++i) {
        AhoCorasick::ResultType r = matches[i];
        if (r.position > 0) {
            std::size_t end_offset = r.position - last_position + r.length;
            while (raw_text[last_position + end_offset - 1] == '\n') {
                --end_offset;
            };
            if (end_offset == 0)
                continue;
            sentence_boundaries.push_back(last_position + end_offset);
            last_position = r.position + r.length;
        }
    }

    std::size_t last_sentence_pos = 0;
    std::size_t last_term_pos = 0;
    Vector<std::size_t>::iterator last_sentence_iter = sentence_boundaries.end();
    for (auto &term : hit_list) {
        auto it = std::lower_bound(sentence_boundaries.begin(), sentence_boundaries.end(), term.word_offset_, [](auto element, auto value) {
            return element < value;
        });

        if (last_sentence_iter == sentence_boundaries.end()) {
            last_sentence_pos = 0;
            output.append(raw_text.substr(last_sentence_pos, term.word_offset_ - term.text_.size() - last_sentence_pos));
        } else {
            std::size_t distance = it - last_sentence_iter;
            if (distance > 1) {
                output.append("...");
                last_sentence_pos = *(it - 1);
                output.append(raw_text.substr(last_sentence_pos, term.word_offset_ - term.text_.size() - last_sentence_pos));
            } else if (distance == 1) {
                last_sentence_pos = *(it - 1);
                output.append(raw_text.substr(last_sentence_pos, term.word_offset_ - term.text_.size() - last_sentence_pos));
            } else {
                // multiple terms hit in one sentence
                output.append(raw_text.substr(last_term_pos, term.word_offset_ - term.text_.size() - last_term_pos));
            }
        }

        output.append("<em>");
        output.append(raw_text.substr(term.word_offset_ - term.text_.size(), term.text_.size()));
        output.append("</em>");
        last_term_pos = term.word_offset_;
        last_sentence_iter = it;
        last_sentence_pos = *it;
    }
    if (last_term_pos < last_sentence_pos)
        output.append(raw_text.substr(last_term_pos, last_sentence_pos - last_term_pos));
    if (last_sentence_iter != sentence_boundaries.end()) {
        output.append("...");
    }
}

}; // namespace infinity
