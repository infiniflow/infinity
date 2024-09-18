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

export module tokenizer;

import stl;
import term;

namespace infinity {
constexpr unsigned BYTE_MAX = 255;

export class TokenizeConfig {
public:
    void AddAllows(String astr) { allows_ += astr; }
    void AddDivides(String dstr) { divides_ += dstr; }
    void AddUnites(String ustr) { unites_ += ustr; }
    String allows_;
    String divides_;
    String unites_;
};

export typedef unsigned char CharType;

export extern const CharType ALLOW_CHR;     /// < regular term
export extern const CharType DELIMITER_CHR; /// < delimiter
export extern const CharType SPACE_CHR;     /// < space term
export extern const CharType UNITE_CHR;     /// < united term

export class CharTypeTable {
    CharType char_type_table_[BYTE_MAX];

public:
    CharTypeTable(bool use_def_delim = true);

    void SetConfig(const TokenizeConfig &conf);

    CharType GetType(u8 c) { return char_type_table_[c]; }

    bool IsAllow(u8 c) { return char_type_table_[c] == ALLOW_CHR; }

    bool IsDivide(u8 c) { return char_type_table_[c] == DELIMITER_CHR; }

    bool IsUnite(u8 c) { return char_type_table_[c] == UNITE_CHR; }

    bool IsEqualType(u8 c1, u8 c2) { return char_type_table_[c1] == char_type_table_[c2]; }
};

export class Tokenizer {
public:
    Tokenizer(bool use_def_delim = true) : table_(use_def_delim) { output_buffer_ = new char[output_buffer_size_](); }

    ~Tokenizer() { delete[] output_buffer_; }

    /// \brief set the user defined char types
    /// \param list char type option list
    void SetConfig(const TokenizeConfig &conf);

    /// \brief tokenize the input text, call nextToken(), getToken(), getLength() to get the result.
    /// \param input input text string
    void Tokenize(const String &input);

    bool NextToken();

    inline const char *GetToken() { return output_buffer_; }

    inline SizeT GetLength() { return output_buffer_cursor_; }

    inline bool IsDelimiter() { return is_delimiter_; }

    inline SizeT GetInputCursor() const { return input_cursor_; }

    bool Tokenize(const String &input_string, TermList &special_terms, TermList &prim_terms);

    /// \brief tokenize the input text, remove the space chars, output raw term list
    bool TokenizeWhite(const String &input_string, TermList &raw_terms);

    /// \brief tokenize the input text, output two term lists: raw term list and primary term list
    bool Tokenize(const String &input_string, TermList &prim_terms);

private:
    bool GrowOutputBuffer();

private:
    CharTypeTable table_;

    String *input_{nullptr};

    SizeT input_cursor_{0};

    SizeT output_buffer_size_{4096};

    char *output_buffer_{nullptr};

    SizeT output_buffer_cursor_{0};

    bool is_delimiter_{false};
};
} // namespace infinity
