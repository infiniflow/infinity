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

#include "string_utils.h"
#include <cstring>

module infinity_core:common_analyzer.impl;

import :stl;
import :term;
import :stemmer;
import :analyzer;
import :tokenizer;
import :common_analyzer;
import :default_values;

namespace infinity {

CommonLanguageAnalyzer::CommonLanguageAnalyzer()
    : Analyzer(), lowercase_string_buffer_(term_string_buffer_limit_), stemmer_(MakeUnique<Stemmer>()), case_sensitive_(false), contain_lower_(false),
      extract_eng_stem_(true), extract_synonym_(false), cjk_(false), remove_stopwords_(false) {}

CommonLanguageAnalyzer::~CommonLanguageAnalyzer() {}

void CommonLanguageAnalyzer::InitStemmer(Language language) { stemmer_->Init(language); }

int CommonLanguageAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    Parse(input.text_);

    int temp_offset = 0;

    while (NextToken()) {
        if (len_ == 0)
            continue;

        if (len_ >= MAX_TUPLE_LENGTH)
            continue;

        if (remove_stopwords_ && IsStopword())
            continue;

        if (is_index_) {
            if (IsSpecialChar()) {
                func(data, token_, len_, offset_, end_offset_, true, 0);
                temp_offset = offset_;
                continue;
            }
            if (is_raw_) {
                func(data, token_, len_, offset_, end_offset_, false, 0);
                temp_offset = offset_;
                continue;
            }

            // foreign language, e.g. English
            if (IsAlpha()) {
                char *lowercase_term = lowercase_string_buffer_.data();
                ToLower(token_, len_, lowercase_term, term_string_buffer_limit_);
                SizeT stemming_term_str_size = 0;
                String stem_term;
                if (extract_eng_stem_) {
                    stemmer_->Stem(lowercase_term, stem_term);
                    if (strcmp(stem_term.c_str(), lowercase_term)) {
                        stemming_term_str_size = stem_term.length();
                    }
                }
                bool lowercase_is_different = memcmp(token_, lowercase_term, len_) != 0;

                if (stemming_term_str_size && stem_only_) {
                    func(data, stem_term.c_str(), stemming_term_str_size, offset_, end_offset_, false, 0);
                    temp_offset = offset_;
                } else if (stemming_term_str_size || (case_sensitive_ && contain_lower_ && lowercase_is_different)) {
                    /// have more than one output
                    if (case_sensitive_) {
                        func(data, token_, len_, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    } else {
                        func(data, lowercase_term, len_, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    }
                    if (stemming_term_str_size) {
                        func(data, stem_term.c_str(), stemming_term_str_size, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    }
                    if (case_sensitive_ && contain_lower_ && lowercase_is_different) {
                        func(data, lowercase_term, len_, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    }
                } else {
                    /// have only one output
                    if (case_sensitive_) {
                        func(data, token_, len_, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    } else {
                        func(data, lowercase_term, len_, offset_, end_offset_, false, 0);
                        temp_offset = offset_;
                    }
                }
            } else {
                func(data, token_, len_, offset_, end_offset_, false, 0);
                temp_offset = offset_;
            }
        }
    }
    return temp_offset + 1;
}

} // namespace infinity
