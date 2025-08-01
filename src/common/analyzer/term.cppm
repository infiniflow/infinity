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

export module infinity_core:term;

import :stl;
import :jieba;

namespace infinity {
export class Term {
public:
    Term() : word_offset_(0), payload_(0) {}
    Term(const String &str) : text_(str), word_offset_(0), payload_(0) {}
    ~Term() {}

    void Reset();

    u32 Length() { return text_.length(); }

    String Text() const { return text_; }

public:
    String text_;
    u32 word_offset_;
    u32 end_offset_;
    u16 payload_;
};

export class TermList : public Deque<Term> {
public:
    void Add(const char *text, const u32 len, const u32 offset, const u32 end_offset, const u16 payload = 0) {
        push_back(global_temporary_);
        back().text_.assign(text, len);
        back().word_offset_ = offset;
        back().end_offset_ = end_offset;
        back().payload_ = payload;
    }

    void Add(cppjieba::Word &cut_word) {
        push_back(global_temporary_);
        std::swap(back().text_, cut_word.word);
        back().word_offset_ = cut_word.offset;
    }

    void Add(const String &token, const u32 offset, const u32 end_offset, const u16 payload = 0) {
        push_back(global_temporary_);
        back().text_ = token;
        back().word_offset_ = offset;
        back().end_offset_ = end_offset;
        back().payload_ = payload;
    }

    void Add(String &token, const u32 offset, const u32 end_offset, const u16 payload = 0) {
        push_back(global_temporary_);
        std::swap(back().text_, token);
        back().word_offset_ = offset;
        back().end_offset_ = end_offset;
        back().payload_ = payload;
    }

private:
    static Term global_temporary_;
};

export extern String PLACE_HOLDER;

} // namespace infinity