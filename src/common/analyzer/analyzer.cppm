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

export module analyzer;

import jieba;
import stl;
import term;
import tokenizer;

namespace infinity {

export enum class CutGrain {
    kCoarse,
    kFine,
};

export class Analyzer {
public:
    Analyzer() = default;

    virtual ~Analyzer() = default;

    void SetInnerAnalyzer(SharedPtr<Analyzer> &analyzer) { inner_analyzer_ = analyzer; }

    void SetExtractSpecialChar(bool extract_special_char, bool convert_to_placeholder = true) {
        extract_special_char_ = extract_special_char;
        convert_to_placeholder_ = convert_to_placeholder;
    }

    void SetCharOffset(bool set) { get_char_offset_ = set; }

    int Analyze(const Term &input, TermList &output) {
        void *array[2] = {&output, this};
        return AnalyzeImpl(input, &array, &Analyzer::AppendTermList);
    }

protected:
    typedef void (
        *HookType)(void *data, const char *text, const u32 len, const u32 offset, const u8 and_or_bit, const u8 level, const bool is_special_char);

    virtual int AnalyzeImpl(const Term &input, void *data, HookType func) { return -1; }

    static void
    AppendTermList(void *data, const char *text, const u32 len, const u32 offset, const u8 and_or_bit, const u8 level, const bool is_special_char) {
        void **parameters = (void **)data;
        TermList *output = (TermList *)parameters[0];
        Analyzer *analyzer = (Analyzer *)parameters[1];

        if (is_special_char && !analyzer->extract_special_char_)
            return;
        if (is_special_char && analyzer->convert_to_placeholder_) {
            if (output->empty() == true || output->back().text_.compare(PLACE_HOLDER) != 0)
                output->Add(PLACE_HOLDER.c_str(), PLACE_HOLDER.length(), offset, and_or_bit, level);
        } else {
            output->Add(text, len, offset, and_or_bit, level);
        }
    }

    static void AppendTermListForJieba(void *data, cppjieba::Word &cut_word) {
        void **parameters = (void **)data;
        TermList *output = (TermList *)parameters[0];
        output->Add(cut_word);
    }

    Tokenizer tokenizer_;

    SharedPtr<Analyzer> inner_analyzer_;
    /// Whether including speical characters (e.g. puncutations) in the result.
    bool extract_special_char_;

    /// Whether converting speical characters (e.g. puncutations) into a particular place holder
    /// symbol in the result.
    /// Be effect only when extract_special_char_ is set.
    bool convert_to_placeholder_;

    bool get_char_offset_{false};
};
} // namespace infinity