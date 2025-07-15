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
// limitations under the License.

module;

#define PCRE2_CODE_UNIT_WIDTH 8

#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <openccxx.h>
#include <pcre2.h>
#include <re2/re2.h>
#include <sstream>

#include "string_utils.h"

module infinity_core;

import :stl;
import :term;
import :stemmer;
import :analyzer;
import :darts_trie;
import :lemmatizer;
import :stemmer;
import :term;

namespace fs = std::filesystem;

namespace infinity {

static const String DICT_PATH = "rag/huqie.txt";
static const String POS_DEF_PATH = "rag/pos-id.def";
static const String TRIE_PATH = "rag/huqie.trie";
static const String WORDNET_PATH = "wordnet";

static const String OPENCC_PATH = "opencc";

static const String REGEX_SPLIT_CHAR = R"#(([ ,\.<>/?;'\[\]\`!@#$%^&*$$\{\}\|_+=《》，。？、；‘’：“”【】~！￥%……（）——-]+|[a-zA-Z\.-]+|[0-9,\.-]+))#";

static const String NLTK_TOKENIZE_PATTERN =
    R"((?:\-{2,}|\.{2,}|(?:\.\s){2,}\.)|(?=[^\(\"\`{\[:;&\#\*@\)}\]\-,])\S+?(?=\s|$|(?:[)\";}\]\*:@\'\({\[\?!])|(?:\-{2,}|\.{2,}|(?:\.\s){2,}\.)|,(?=$|\s|(?:[)\";}\]\*:@\'\({\[\?!])|(?:\-{2,}|\.{2,}|(?:\.\s){2,}\.)))|\S)";

static constexpr std::size_t MAX_SENTENCE_LEN = 100;

static inline i32 Encode(i32 freq, i32 idx) {
    u32 encoded_value = 0;
    if (freq < 0) {
        encoded_value |= (u32)(-freq);
        encoded_value |= (1U << 23);
    } else {
        encoded_value = (u32)(freq & 0x7FFFFF);
    }

    encoded_value |= (u32)idx << 24;
    return (i32)encoded_value;
}

static inline i32 DecodeFreq(i32 value) {
    u32 v1 = (u32)(value) & 0xFFFFFF;
    if (v1 & (1 << 23)) {
        v1 &= 0x7FFFFF;
        return -(i32)v1;
    } else {
        v1 = (i32)v1;
    }
    return v1;
}

void Split(const String &input, const String &split_pattern, Vector<String> &result, bool keep_delim = false) {
    re2::RE2 pattern(split_pattern);
    re2::StringPiece leftover(input.data());
    re2::StringPiece last_end = leftover;
    re2::StringPiece extracted_delim_token;

    while (RE2::FindAndConsume(&leftover, pattern, &extracted_delim_token)) {
        std::string_view token(last_end.data(), extracted_delim_token.data() - last_end.data());
        if (!token.empty()) {
            result.push_back(String(token.data(), token.size()));
        }
        if (keep_delim)
            result.push_back(String(extracted_delim_token.data(), extracted_delim_token.size()));
        last_end = leftover;
    }

    if (!leftover.empty()) {
        result.push_back(String(leftover.data(), leftover.size()));
    }
}

void Split(const String &input, const RE2 &pattern, Vector<String> &result, bool keep_delim = false) {
    re2::StringPiece leftover(input.data());
    re2::StringPiece last_end = leftover;
    re2::StringPiece extracted_delim_token;

    while (RE2::FindAndConsume(&leftover, pattern, &extracted_delim_token)) {
        std::string_view token(last_end.data(), extracted_delim_token.data() - last_end.data());
        if (!token.empty()) {
            result.push_back(String(token.data(), token.size()));
        }
        if (keep_delim)
            result.push_back(String(extracted_delim_token.data(), extracted_delim_token.size()));
        last_end = leftover;
    }

    if (!leftover.empty()) {
        result.push_back(String(leftover.data(), leftover.size()));
    }
}

String Replace(const RE2 &re, const String &replacement, const String &input) {
    String output = input;
    RE2::GlobalReplace(&output, re, replacement);
    return output;
}

template <typename T>
String Join(const Vector<T> &tokens, int start, int end, const String &delim = " ") {
    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        if (i > start)
            oss << delim;
        oss << tokens[i];
    }
    return std::move(oss).str();
}

template <typename T>
String Join(const Vector<T> &tokens, int start, const String &delim = " ") {
    return Join(tokens, start, tokens.size(), delim);
}

String Join(const TermList &tokens, int start, int end, const String &delim = " ") {
    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        if (i > start)
            oss << delim;
        oss << tokens[i].text_;
    }
    return std::move(oss).str();
}

bool IsChinese(const String &str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        if (c >= 0xE4 && c <= 0xE9) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 >= 0x80 && c2 <= 0xBF) && (c3 >= 0x80 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsAlphabet(const String &str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        if (c > 0x7F) {
            return false;
        }
    }
    return true;
}

bool IsKorean(const String &str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        if (c == 0xE1) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 == 0x84 || c2 == 0x85 || c2 == 0x86 || c2 == 0x87) && (c3 >= 0x80 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsJapanese(const String &str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];
        if (c == 0xE3) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 == 0x81 || c2 == 0x82 || c2 == 0x83) && (c3 >= 0x81 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsCJK(const String &str) {
    for (std::size_t i = 0; i < str.length(); ++i) {
        unsigned char c = str[i];

        // Check Chinese
        if (c >= 0xE4 && c <= 0xE9) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 >= 0x80 && c2 <= 0xBF) && (c3 >= 0x80 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }

        // Check Japanese
        if (c == 0xE3) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 == 0x81 || c2 == 0x82 || c2 == 0x83) && (c3 >= 0x81 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }

        // Check Korean
        if (c == 0xE1) {
            if (i + 2 < str.length()) {
                unsigned char c2 = str[i + 1];
                unsigned char c3 = str[i + 2];
                if ((c2 == 0x84 || c2 == 0x85 || c2 == 0x86 || c2 == 0x87) && (c3 >= 0x80 && c3 <= 0xBF)) {
                    return true;
                }
            }
        }
    }
    return false;
}

class RegexTokenizer {
public:
    RegexTokenizer() {
        int errorcode = 0;
        PCRE2_SIZE erroffset = 0;

        re_ = pcre2_compile((PCRE2_SPTR)(NLTK_TOKENIZE_PATTERN.c_str()),
                            PCRE2_ZERO_TERMINATED,
                            PCRE2_MULTILINE | PCRE2_UTF,
                            &errorcode,
                            &erroffset,
                            nullptr);
    }

    ~RegexTokenizer() { pcre2_code_free(re_); }

    void RegexTokenize(const String &input, TermList &tokens) {
        PCRE2_SPTR subject = (PCRE2_SPTR)input.c_str();
        PCRE2_SIZE subject_length = input.length();

        pcre2_match_data_8 *match_data = pcre2_match_data_create_8(1024, nullptr);

        PCRE2_SIZE start_offset = 0;

        while (start_offset < subject_length) {
            int res = pcre2_match(re_, subject, subject_length, start_offset, 0, match_data, nullptr);

            if (res < 0) {
                if (res == PCRE2_ERROR_NOMATCH) {
                    break; // No more matches
                } else {
                    std::cerr << "Matching error code: " << res << std::endl;
                    break; // Other error
                }
            }

            // Extract matched substring
            PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
            for (int i = 0; i < res; ++i) {
                PCRE2_SIZE start = ovector[2 * i];
                PCRE2_SIZE end = ovector[2 * i + 1];
                tokens.Add(input.c_str() + start, end - start, start, end);
            }

            // Update the start offset for the next search
            start_offset = ovector[1]; // Move to the end of the last match
        }

        // Free memory
        pcre2_match_data_free(match_data);
    }

private:
    pcre2_code_8 *re_{nullptr};
};

class MacIntyreContractions {
public:
    // List of contractions adapted from Robert MacIntyre's tokenizer.
    Vector<String> CONTRACTIONS2 = {R"((?i)\b(can)(?#X)(not)\b)",
                                    R"((?i)\b(d)(?#X)('ye)\b)",
                                    R"((?i)\b(gim)(?#X)(me)\b)",
                                    R"((?i)\b(gon)(?#X)(na)\b)",
                                    R"((?i)\b(got)(?#X)(ta)\b)",
                                    R"((?i)\b(lem)(?#X)(me)\b)",
                                    R"((?i)\b(more)(?#X)('n)\b)",
                                    R"((?i)\b(wan)(?#X)(na)(?=\s))"};
    Vector<String> CONTRACTIONS3 = {R"((?i) ('t)(?#X)(is)\b)", R"((?i) ('t)(?#X)(was)\b)"};
    Vector<String> CONTRACTIONS4 = {R"((?i)\b(whad)(dd)(ya)\b)", R"((?i)\b(wha)(t)(cha)\b)"};
};

class NLTKWordTokenizer {
    MacIntyreContractions contractions_;

public:
    // Starting quotes.
    Vector<Pair<String, String>> STARTING_QUOTES = {{String(R"(([«“‘„]|[`]+))"), String(R"( $1 )")},
                                                    {String(R"(^\")"), String(R"(``)")},
                                                    {String(R"((``))"), String(R"( $1 )")},
                                                    {String(R"(([ \(\[{<])(\"|\'{2}))"), String(R"($1 `` )")},
                                                    {String(R"((?i)(\')(?!re|ve|ll|m|t|s|d|n)(\w)\b)"), String(R"($1 $2)")}};

    // Ending quotes.
    Vector<Pair<String, String>> ENDING_QUOTES = {{String(R"(([»”’]))"), String(R"( $1 )")},
                                                  {String(R"('')"), String(R"( '' )")},
                                                  {String(R"(")"), String(R"( '' )")},
                                                  {String(R"(\s+)"), String(R"( )")},
                                                  {String(R"(([^' ])('[sS]|'[mM]|'[dD]|') )"), String(R"($1 $2 )")},
                                                  {String(R"(([^' ])('ll|'LL|'re|'RE|'ve|'VE|n't|N'T) )"), String(R"($1 $2 )")}};

    // Punctuation.
    Vector<Pair<String, String>> PUNCTUATION = {{String(R"(([^\.])(\.)([\]\)}>"\'»”’ ]*)\s*$)"), String(R"($1 $2 $3 )")},
                                                {String(R"(([:,])([^\d]))"), String(R"( $1 $2)")},
                                                {String(R"(([:,])$)"), String(R"($1 )")},
                                                {String(R"(\.{2,})"), String(R"($0 )")},
                                                {String(R"([;@#$%&])"), String(R"($0 )")},
                                                {String(R"(([^\.])(\.)([\]\)}>"\']*)\s*$)"), String(R"($1 $2 $3 )")},
                                                {String(R"([?!])"), String(R"($0 )")},
                                                {String(R"(([^'])' )"), String(R"($1 ' )")},
                                                {String(R"([*])"), String(R"($0 )")}};

    // Pads parentheses
    Pair<String, String> PARENS_BRACKETS = {String(R"([\]\[\(\)\{\}\<\>])"), String(R"( $0 )")};

    Vector<Pair<String, String>> CONVERT_PARENTHESES = {{String(R"(\()"), String("-LRB-")},
                                                        {String(R"(\))"), String("-RRB-")},
                                                        {String(R"(\[)"), String("-LSB-")},
                                                        {String(R"(\])"), String("-RSB-")},
                                                        {String(R"(\{)"), String("-LCB-")},
                                                        {String(R"(\})"), String("-RCB-")}};

    Pair<String, String> DOUBLE_DASHES = {String(R"(--)"), String(R"( -- )")};

    void Tokenize(const String &text, Vector<String> &tokens, bool convert_parentheses = false) {
        String result = text;

        for (const auto &[pattern, substitution] : STARTING_QUOTES) {
            result = ApplyRegex(result, pattern, substitution);
        }
        for (const auto &[pattern, substitution] : PUNCTUATION) {
            result = ApplyRegex(result, pattern, substitution);
        }

        // Handles parentheses.
        result = ApplyRegex(result, PARENS_BRACKETS.first, PARENS_BRACKETS.second);

        // Optionally convert parentheses
        if (convert_parentheses) {
            for (const auto &[pattern, substitution] : CONVERT_PARENTHESES) {
                result = ApplyRegex(result, pattern, substitution);
            }
        }

        // Handles double dash.
        result = ApplyRegex(result, DOUBLE_DASHES.first, DOUBLE_DASHES.second);

        // Add extra space to make things easier
        result = " " + result + " ";

        for (const auto &[pattern, substitution] : ENDING_QUOTES) {
            result = ApplyRegex(result, pattern, substitution);
        }

        for (const auto &pattern : contractions_.CONTRACTIONS2) {
            result = ApplyRegex(result, pattern, R"( $1 $2 )");
        }

        for (const auto &pattern : contractions_.CONTRACTIONS3) {
            result = ApplyRegex(result, pattern, R"( $1 $2 )");
        }

        // Split the result into tokens
        size_t start = 0;
        size_t end = result.find(' ');
        while (end != String::npos) {
            if (end != start) {
                tokens.push_back(result.substr(start, end - start));
            }
            start = end + 1;
            end = result.find(' ', start);
        }
        if (start != result.length()) {
            tokens.push_back(result.substr(start));
        }
    }

private:
    String ApplyRegex(const String &text, const String &pattern, const String &substitution) {
        pcre2_code *re;
        PCRE2_SPTR pcre2_pattern = reinterpret_cast<PCRE2_SPTR>(pattern.c_str());
        PCRE2_SPTR pcre2_subject = reinterpret_cast<PCRE2_SPTR>(text.c_str());
        PCRE2_SPTR pcre2_replacement = reinterpret_cast<PCRE2_SPTR>(substitution.c_str());
        int errorcode;
        PCRE2_SIZE erroroffset;
        re = pcre2_compile(pcre2_pattern, PCRE2_ZERO_TERMINATED, PCRE2_MULTILINE | PCRE2_UTF, &errorcode, &erroroffset, nullptr);
        if (re == nullptr) {
            return text;
        }

        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, nullptr);
        int rc = pcre2_match(re, pcre2_subject, text.length(), 0, 0, match_data, nullptr);
        if (rc < 0) {
            pcre2_match_data_free(match_data);
            pcre2_code_free(re);
            return text;
        }

        size_t outlength = text.length() * 2 < 1024 ? 1024 : text.length() * 2;
        auto buffer = MakeUnique<PCRE2_UCHAR[]>(outlength);
        pcre2_substitute(re,
                         pcre2_subject,
                         text.length(),
                         0,
                         PCRE2_SUBSTITUTE_GLOBAL,
                         match_data,
                         nullptr,
                         pcre2_replacement,
                         PCRE2_ZERO_TERMINATED,
                         buffer.get(),
                         &outlength);
        pcre2_match_data_free(match_data);
        pcre2_code_free(re);

        return String(reinterpret_cast<char *>(buffer.get()), outlength);
    }
};

void SentenceSplitter(const String &text, Vector<String> &result) {
    int error_code;
    PCRE2_SIZE error_offset;
    const char *pattern = R"( *[\.\?!]['"\)\]]* *)";

    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, PCRE2_MULTILINE | PCRE2_UTF, &error_code, &error_offset, nullptr);

    if (re == nullptr) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(error_code, buffer, sizeof(buffer));
        std::cerr << "PCRE2 compilation failed at offset " << error_offset << ": " << buffer << std::endl;
        return;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, nullptr);

    PCRE2_SIZE start_offset = 0;
    while (start_offset < text.size()) {
        int rc = pcre2_match(re, (PCRE2_SPTR)text.c_str(), text.size(), start_offset, 0, match_data, nullptr);

        if (rc < 0) {
            result.push_back(text.substr(start_offset));
            break;
        }

        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
        PCRE2_SIZE match_start = ovector[0];
        PCRE2_SIZE match_end = ovector[1];

        if (match_start > start_offset) {
            result.push_back(text.substr(start_offset, match_end - start_offset));
        }

        start_offset = match_end;
    }

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);
}

RAGAnalyzer::RAGAnalyzer(const String &path)
    : dict_path_(path), stemmer_(MakeUnique<Stemmer>()), lowercase_string_buffer_(term_string_buffer_limit_),
      nltk_tokenizer_(MakeUnique<NLTKWordTokenizer>()) {
    InitStemmer(STEM_LANG_ENGLISH);
}

RAGAnalyzer::RAGAnalyzer(const RAGAnalyzer &other)
    : own_dict_(false), trie_(other.trie_), pos_table_(other.pos_table_), lemma_(other.lemma_), stemmer_(MakeUnique<Stemmer>()),
      opencc_(other.opencc_), lowercase_string_buffer_(term_string_buffer_limit_), fine_grained_(other.fine_grained_),
      nltk_tokenizer_(MakeUnique<NLTKWordTokenizer>()) {
    InitStemmer(STEM_LANG_ENGLISH);
}

RAGAnalyzer::~RAGAnalyzer() {
    if (own_dict_) {
        delete trie_;
        delete pos_table_;
        delete lemma_;
        delete opencc_;
    }
}

Status RAGAnalyzer::Load() {
    fs::path root(dict_path_);
    fs::path dict_path(root / DICT_PATH);

    if (!fs::exists(dict_path)) {
        return Status::InvalidAnalyzerFile(dict_path);
    }

    fs::path pos_def_path(root / POS_DEF_PATH);
    if (!fs::exists(pos_def_path)) {
        return Status::InvalidAnalyzerFile(pos_def_path);
    }
    own_dict_ = true;
    trie_ = new DartsTrie();
    pos_table_ = new POSTable(pos_def_path.string());
    if (!pos_table_->Load().ok()) {
        return Status::InvalidAnalyzerFile("Failed to load RAGAnalyzer POS definition");
    }

    fs::path trie_path(root / TRIE_PATH);
    if (fs::exists(trie_path)) {
        trie_->Load(trie_path.string());
    } else {
        // Build trie
        try {
            std::ifstream from(dict_path.string());
            String line;
            RE2 re_pattern(R"([\r\n]+)");
            String split_pattern("([ \t])");

            while (getline(from, line)) {
                line = line.substr(0, line.find('\r'));
                if (line.empty())
                    continue;
                line = Replace(re_pattern, "", line);
                Vector<String> results;
                Split(line, split_pattern, results);
                if (results.size() != 3)
                    throw std::runtime_error("Invalid dictionary format");
                i32 freq = std::stoi(results[1]);
                freq = i32(std::log(float(freq) / DENOMINATOR) + 0.5);
                i32 pos_idx = pos_table_->GetPOSIndex(results[2]);
                int value = Encode(freq, pos_idx);
                trie_->Add(results[0], value);
                String rkey = RKey(results[0]);
                trie_->Add(rkey, Encode(1, 0));
            }
            trie_->Build();
        } catch (const std::exception &e) {
            return Status::InvalidAnalyzerFile("Failed to load RAGAnalyzer analyzer");
        }
        trie_->Save(trie_path.string());
    }

    fs::path lemma_path(root / WORDNET_PATH);
    if (!fs::exists(lemma_path)) {
        return Status::InvalidAnalyzerFile(lemma_path);
    }
    lemma_ = new Lemmatizer(lemma_path.string());

    fs::path opencc_path(root / OPENCC_PATH);

    if (!fs::exists(opencc_path)) {
        return Status::InvalidAnalyzerFile(opencc_path);
    }
    try {
        opencc_ = new ::OpenCC(opencc_path.string());
    } catch (const std::exception &e) {
        return Status::InvalidAnalyzerFile("Failed to load OpenCC");
    }

    return Status::OK();
}

String RAGAnalyzer::StrQ2B(const String &input) {
    String output;
    size_t i = 0;

    while (i < input.size()) {
        unsigned char c = input[i];

        uint32_t codepoint = 0;
        if (c < 0x80) {
            codepoint = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            codepoint = (c & 0x1F) << 6;
            codepoint |= (input[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            codepoint = (c & 0x0F) << 12;
            codepoint |= (input[i + 1] & 0x3F) << 6;
            codepoint |= (input[i + 2] & 0x3F);
            i += 3;
        } else {
            output += c;
            i += 1;
            continue;
        }

        if (codepoint >= 0xFF01 && codepoint <= 0xFF5E) {
            output += static_cast<char>(codepoint - 0xFEE0);
        } else if (codepoint == 0x3000) {
            output += ' ';
        } else {
            if (codepoint < 0x80) {
                output += static_cast<char>(codepoint);
            } else if (codepoint < 0x800) {
                output += static_cast<char>(0xC0 | (codepoint >> 6));
                output += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else if (codepoint < 0x10000) {
                output += static_cast<char>(0xE0 | (codepoint >> 12));
                output += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                output += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }
    }

    return output;
}

i32 RAGAnalyzer::Freq(const std::string_view key) const {
    i32 v = trie_->Get(key);
    v = DecodeFreq(v);
    return static_cast<i32>(std::exp(v) * DENOMINATOR + 0.5);
}

String RAGAnalyzer::Key(const std::string_view line) { return ToLowerString(line); }

String RAGAnalyzer::RKey(const std::string_view line) {
    String reversed;
    reversed.reserve(line.size() + 2);
    reversed += "DD";
    for (size_t i = line.size(); i > 0;) {
        size_t start = i - 1;
        while (start > 0 && (line[start] & 0xC0) == 0x80) {
            --start;
        }
        reversed += line.substr(start, i - start);
        i = start;
    }
    ToLower(reversed.data() + 2, reversed.size() - 2);
    return reversed;
}

#define DIVIDE_F_BY_N 1

Pair<Vector<String>, double> RAGAnalyzer::Score(const Vector<Pair<String, int>> &token_freqs) {
    constexpr i64 B = 30;
    i64 F = 0, L = 0;
    Vector<String> tokens;
    tokens.reserve(token_freqs.size());
    for (const auto &[token, freq_tag] : token_freqs) {
        F += DecodeFreq(freq_tag);
        L += (UTF8Length(token) < 2) ? 0 : 1;
        tokens.push_back(token);
    }
#ifdef DIVIDE_F_BY_N
    const auto score = (B + L + F) / static_cast<double>(tokens.size());
#else
    const auto score = F + (B + L) / static_cast<double>(tokens.size());
#endif
    return {std::move(tokens), score};
}

void RAGAnalyzer::SortTokens(const Vector<Vector<Pair<String, int>>> &token_list, Vector<Pair<Vector<String>, double>> &res) {
    for (const auto &tfts : token_list) {
        res.push_back(Score(tfts));
    }
    std::sort(res.begin(), res.end(), [](const auto &a, const auto &b) { return a.second > b.second; });
}

Pair<Vector<String>, double> RAGAnalyzer::MaxForward(const String &line) const {
    Vector<Pair<String, int>> res;
    std::size_t s = 0;
    std::size_t len = UTF8Length(line);

    while (s < len) {
        std::size_t e = s + 1;
        String t = UTF8Substr(line, s, e - s);

        while (e < len && trie_->HasKeysWithPrefix(Key(t))) {
            e += 1;
            t = UTF8Substr(line, s, e - s);
        }

        while (e - 1 > s && trie_->Get(Key(t)) == -1) {
            e -= 1;
            t = UTF8Substr(line, s, e - s);
        }

        int v = trie_->Get(Key(t));
        if (v != -1) {
            res.emplace_back(std::move(t), v);
        } else {
            res.emplace_back(std::move(t), 0);
        }

        s = e;
    }

    return Score(res);
}

Pair<Vector<String>, double> RAGAnalyzer::MaxBackward(const String &line) const {
    Vector<Pair<String, int>> res;
    int s = UTF8Length(line) - 1;

    while (s >= 0) {
        const int e = s + 1;
        String t = UTF8Substr(line, s, e - s);
        while (s > 0 && trie_->HasKeysWithPrefix(RKey(t))) {
            s -= 1;
            t = UTF8Substr(line, s, e - s);
        }
        while (s + 1 < e && trie_->Get(Key(t)) == -1) {
            s += 1;
            t = UTF8Substr(line, s, e - s);
        }

        int v = trie_->Get(Key(t));
        if (v != -1) {
            res.emplace_back(std::move(t), v);
        } else {
            res.emplace_back(std::move(t), 0);
        }

        s -= 1;
    }

    std::reverse(res.begin(), res.end());
    return Score(res);
}

int RAGAnalyzer::DFS(const String &chars,
                     const int s,
                     Vector<Pair<String, int>> &pre_tokens,
                     Vector<Vector<Pair<String, int>>> &token_list,
                     Vector<String> &best_tokens,
                     double &max_score,
                     const bool memo_all) const {
    int res = s;
    const int len = UTF8Length(chars);
    if (s >= len) {
        if (memo_all) {
            token_list.push_back(pre_tokens);
        } else if (auto [vec_str, current_score] = Score(pre_tokens); current_score > max_score) {
            best_tokens = std::move(vec_str);
            max_score = current_score;
        }
        return res;
    }
    // pruning
    int S = s + 1;
    if (s + 2 <= len) {
        String t1 = UTF8Substr(chars, s, 1);
        String t2 = UTF8Substr(chars, s, 2);
        if (trie_->HasKeysWithPrefix(Key(t1)) && !trie_->HasKeysWithPrefix(Key(t2))) {
            S = s + 2;
        }
    }

    if (pre_tokens.size() > 2 && UTF8Length(pre_tokens[pre_tokens.size() - 1].first) == 1 &&
        UTF8Length(pre_tokens[pre_tokens.size() - 2].first) == 1 && UTF8Length(pre_tokens[pre_tokens.size() - 3].first) == 1) {
        String t1 = pre_tokens[pre_tokens.size() - 1].first + UTF8Substr(chars, s, 1);
        if (trie_->HasKeysWithPrefix(Key(t1))) {
            S = s + 2;
        }
    }

    for (int e = S; e <= len; ++e) {
        String t = UTF8Substr(chars, s, e - s);
        String k = Key(t);

        if (e > s + 1 && !trie_->HasKeysWithPrefix(k)) {
            break;
        }

        if (const int v = trie_->Get(k); v != -1) {
            auto pretks = pre_tokens;
            pretks.emplace_back(std::move(t), v);
            res = std::max(res, DFS(chars, e, pretks, token_list, best_tokens, max_score, memo_all));
        }
    }

    if (res > s) {
        return res;
    }

    String t = UTF8Substr(chars, s, 1);
    if (const int v = trie_->Get(Key(t)); v != -1) {
        pre_tokens.emplace_back(std::move(t), v);
    } else {
        pre_tokens.emplace_back(std::move(t), Encode(-12, 0));
    }

    return DFS(chars, s + 1, pre_tokens, token_list, best_tokens, max_score, memo_all);
}

struct TokensList {
    const TokensList *prev = nullptr;
    std::string_view token = {};
};

struct BestTokenCandidate {
    static constexpr i64 B = 30;
    TokensList tl{};
    // N: token num
    // L: num of tokens with length >= 2
    // F: sum of freq
    u32 N{};
    u32 L{};
    i64 F{};
    auto k() const {
#ifdef DIVIDE_F_BY_N
        return N;
#else
        return std::make_pair(N, L);
#endif
    }
    auto v() const { return F; }
    auto score() const {
#ifdef DIVIDE_F_BY_N
        return static_cast<double>(B + L + F) / N;
#else
        return F + (static_cast<double>(B + L) / N);
#endif
    }
    BestTokenCandidate update(const std::string_view new_token_sv, const i32 key_f, const u32 add_l) const {
        return {{&tl, new_token_sv}, N + 1, L + add_l, F + key_f};
    }
};

struct GrowingBestTokenCandidatesTopN {
    i32 top_n{};
    Vector<BestTokenCandidate> candidates{};

    explicit GrowingBestTokenCandidatesTopN(const i32 top_n) : top_n(top_n) {}

    void AddBestTokenCandidateTopN(const BestTokenCandidate &add_candidate) {
        const auto [it_b, it_e] =
            std::equal_range(candidates.begin(), candidates.end(), add_candidate, [](const auto &a, const auto &b) { return a.k() < b.k(); });
        auto target_it = it_b;
        bool do_replace = false;
        if (const auto match_cnt = std::distance(it_b, it_e); match_cnt >= top_n) {
            assert(match_cnt == top_n);
            const auto it = std::min_element(it_b, it_e, [](const auto &a, const auto &b) { return a.v() < b.v(); });
            if (it->v() >= add_candidate.v()) {
                return;
            }
            target_it = it;
            do_replace = true;
        }
        if (do_replace) {
            *target_it = add_candidate;
        } else {
            candidates.insert(target_it, add_candidate);
        }
    }
};

Vector<Pair<Vector<std::string_view>, double>> RAGAnalyzer::GetBestTokensTopN(const std::string_view chars, const u32 n) const {
    const auto utf8_len = UTF8Length(chars);
    Vector<GrowingBestTokenCandidatesTopN> dp_vec(utf8_len + 1, GrowingBestTokenCandidatesTopN(n));
    dp_vec[0].candidates.resize(1);
    const char *current_utf8_ptr = chars.data();
    u32 current_left_chars = chars.size();
    String growing_key; // in lower case
    for (u32 i = 0; i < utf8_len; ++i) {
        const std::string_view current_chars{current_utf8_ptr, current_left_chars};
        const u32 left_utf8_cnt = utf8_len - i;
        growing_key.clear();
        const char *lookup_until = current_utf8_ptr;
        u32 lookup_left_chars = current_left_chars;
        std::size_t reuse_node_pos = 0;
        std::size_t reuse_key_pos = 0;
        for (u32 j = 1; j <= left_utf8_cnt; ++j) {
            {
                // handle growing_key
                const auto next_one_utf8 = UTF8Substrview({lookup_until, lookup_left_chars}, 0, 1);
                if (next_one_utf8.size() == 1 && next_one_utf8[0] >= 'A' && next_one_utf8[0] <= 'Z') {
                    growing_key.push_back(next_one_utf8[0] - 'A' + 'a');
                } else {
                    growing_key.append(next_one_utf8);
                }
                lookup_until += next_one_utf8.size();
                lookup_left_chars -= next_one_utf8.size();
            }
            auto dp_f = [&dp_vec, i, j, original_sv = std::string_view{current_utf8_ptr, growing_key.size()}](const i32 key_f, const u32 add_l) {
                for (auto &target_dp = dp_vec[i + j]; const auto &c : dp_vec[i].candidates) {
                    target_dp.AddBestTokenCandidateTopN(c.update(original_sv, key_f, add_l));
                }
            };
            if (const auto traverse_result = trie_->Traverse(growing_key.data(), reuse_node_pos, reuse_key_pos, growing_key.size());
                traverse_result >= 0) {
                // in dictionary
                const i32 key_f = DecodeFreq(traverse_result);
                const auto add_l = static_cast<u32>(j >= 2);
                dp_f(key_f, add_l);
            } else {
                // not in dictionary
                if (j == 1) {
                    // also give a score: -12
                    dp_f(-12, 0);
                }
                if (traverse_result == -2) {
                    // no more results
                    break;
                }
            }
        }
        // update current_utf8_ptr and current_left_chars
        const auto forward_cnt = UTF8Substrview(current_chars, 0, 1).size();
        current_utf8_ptr += forward_cnt;
        current_left_chars -= forward_cnt;
    }
    Vector<Pair<const TokensList *, double>> mid_result;
    mid_result.reserve(n);
    for (const auto &c : dp_vec.back().candidates) {
        const auto new_pair = std::make_pair(&(c.tl), c.score());
        if (mid_result.size() < n) {
            mid_result.push_back(new_pair);
        } else {
            assert(mid_result.size() == n);
            if (new_pair.second > mid_result.back().second) {
                mid_result.pop_back();
                const auto insert_pos = std::lower_bound(mid_result.begin(), mid_result.end(), new_pair, [](const auto &a, const auto &b) {
                    return a.second > b.second;
                });
                mid_result.insert(insert_pos, new_pair);
            }
        }
    }
    class HelperFunc {
        u32 cnt = 0;
        Vector<std::string_view> result{};
        void GetTokensInner(const TokensList *tl) {
            if (!tl->prev) {
                result.reserve(cnt);
                return;
            }
            ++cnt;
            GetTokensInner(tl->prev);
            result.push_back(tl->token);
        }

    public:
        Vector<std::string_view> GetTokens(const TokensList *tl) {
            GetTokensInner(tl);
            return std::move(result);
        }
    };
    Vector<Pair<Vector<std::string_view>, double>> result;
    result.reserve(mid_result.size());
    for (const auto [tl, score] : mid_result) {
        result.emplace_back(HelperFunc{}.GetTokens(tl), score);
    }
    return result;
}

// TODO: for test
// #ifndef INFINITY_DEBUG
// #define INFINITY_DEBUG 1
// #endif

#ifdef INFINITY_DEBUG
namespace dp_debug {
template <typename T>
String TestPrintTokens(const Vector<T> &tokens) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        oss << (i ? " #" : "#") << tokens[i] << "#";
    }
    return std::move(oss).str();
}

auto print_1 = [](const bool b) { return b ? "✅" : "❌"; };
auto print_2 = [](const bool b) { return b ? "equal" : "not equal"; };

void compare_score_and_tokens(const Vector<String> &dfs_tokens,
                              const double dfs_score,
                              const Vector<std::string_view> &dp_tokens,
                              const double dp_score,
                              const String &prefix) {
    std::ostringstream oss;
    const auto b_score_eq = dp_score == dfs_score;
    oss << std::format("\n{} {} DFS and DP score {}:\nDFS: {}\nDP : {}\n", print_1(b_score_eq), prefix, print_2(b_score_eq), dfs_score, dp_score);
    bool vec_equal = true;
    if (dp_tokens.size() != dfs_tokens.size()) {
        vec_equal = false;
    } else {
        for (std::size_t k = 0; k < dp_tokens.size(); ++k) {
            if (dp_tokens[k] != dfs_tokens[k]) {
                vec_equal = false;
                break;
            }
        }
    }
    oss << std::format("{} {} DFS and DP result {}:\nDFS: {}\nDP : {}\n",
                       print_1(vec_equal),
                       prefix,
                       print_2(vec_equal),
                       TestPrintTokens(dfs_tokens),
                       TestPrintTokens(dp_tokens));
    std::cerr << std::move(oss).str() << std::endl;
}

inline void CheckDP(const RAGAnalyzer *this_ptr,
                    const std::string_view input_str,
                    const Vector<String> &dfs_tokens,
                    const double dfs_score,
                    const auto t0,
                    const auto t1) {
    const auto dp_result = this_ptr->GetBestTokensTopN(input_str, 1);
    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto dfs_duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0);
    const auto dp_duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1);
    const auto dp_faster = dp_duration < dfs_duration;
    std::cerr << "\n!!! " << print_1(dp_faster) << "\nTOP1 DFS duration: " << dfs_duration << " \nDP  duration: " << dp_duration;
    const auto &[dp_vec, dp_score] = dp_result[0];
    compare_score_and_tokens(dfs_tokens, dfs_score, dp_vec, dp_score, "[1 in top1]");
}

inline void CheckDP2(const RAGAnalyzer *this_ptr, const std::string_view input_str, auto get_dfs_sorted_tokens, const auto t0, const auto t1) {
    constexpr int topn = 2;
    const auto dp_result = this_ptr->GetBestTokensTopN(input_str, topn);
    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto dfs_duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t1 - t0);
    const auto dp_duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1);
    const auto dp_faster = dp_duration < dfs_duration;
    std::cerr << "\n!!! " << print_1(dp_faster) << "\nTOP2 DFS duration: " << dfs_duration << " \nTOP2 DP  duration: " << dp_duration;
    const auto dfs_sorted_tokens = get_dfs_sorted_tokens();
    for (int i = 0; i < topn; ++i) {
        compare_score_and_tokens(dfs_sorted_tokens[i].first,
                                 dfs_sorted_tokens[i].second,
                                 dp_result[i].first,
                                 dp_result[i].second,
                                 std::format("[{} in top{}]", i + 1, topn));
    }
}
} // namespace dp_debug
#endif

String RAGAnalyzer::Merge(const String &tks_str) const {
    String tks = tks_str;

    tks = Replace(replace_space_pattern_, " ", tks);

    Vector<String> tokens;
    Split(tks, blank_pattern_, tokens);
    Vector<String> res;
    std::size_t s = 0;
    while (true) {
        if (s >= tokens.size())
            break;

        std::size_t E = s + 1;
        for (std::size_t e = s + 2; e < std::min(tokens.size() + 1, s + 6); ++e) {
            String tk = Join(tokens, s, e, "");
            if (RE2::PartialMatch(tk, regex_split_pattern_)) {
                if (Freq(tk) > 0) {
                    E = e;
                }
            }
        }
        res.push_back(Join(tokens, s, E, ""));
        s = E;
    }

    return Join(res, 0, res.size());
}

void RAGAnalyzer::EnglishNormalize(const Vector<String> &tokens, Vector<String> &res) {
    for (auto &t : tokens) {
        if (RE2::PartialMatch(t, pattern1_)) { //"[a-zA-Z_-]+$"
            String lemma_term = lemma_->Lemmatize(t);
            char *lowercase_term = lowercase_string_buffer_.data();
            ToLower(lemma_term.c_str(), lemma_term.size(), lowercase_term, term_string_buffer_limit_);
            String stem_term;
            stemmer_->Stem(lowercase_term, stem_term);
            res.push_back(stem_term);
        } else {
            res.push_back(t);
        }
    }
}

void RAGAnalyzer::TokenizeInner(Vector<String> &res, const String &L) const {
    auto [tks, s] = MaxForward(L);
    auto [tks1, s1] = MaxBackward(L);
#if 0
    std::size_t i = 0, j = 0, _i = 0, _j = 0, same = 0;
    while ((i + same < tks1.size()) && (j + same < tks.size()) && tks1[i + same] == tks[j + same]) {
        same++;
    }
    if (same > 0) {
        res.push_back(Join(tks, j, j + same));
    }
    _i = i + same;
    _j = j + same;
    j = _j + 1;
    i = _i + 1;
    while (i < tks1.size() && j < tks.size()) {
        String tk1 = Join(tks1, _i, i, "");
        String tk = Join(tks, _j, j, "");
        if (tk1 != tk) {
            if (tk1.length() > tk.length()) {
                j++;
            } else {
                i++;
            }
            continue;
        }
        if (tks1[i] != tks[j]) {
            i++;
            j++;
            continue;
        }
        Vector<Pair<String, int>> pre_tokens;
        Vector<Vector<Pair<String, int>>> token_list;
        Vector<String> best_tokens;
        double max_score = std::numeric_limits<double>::lowest();
        const auto str_for_dfs = Join(tks, _j, j, "");
#ifdef INFINITY_DEBUG
        const auto t0 = std::chrono::high_resolution_clock::now();
#endif
        DFS(str_for_dfs, 0, pre_tokens, token_list, best_tokens, max_score, false);
#ifdef INFINITY_DEBUG
        const auto t1 = std::chrono::high_resolution_clock::now();
        dp_debug::CheckDP(this, str_for_dfs, best_tokens, max_score, t0, t1);
#endif
        res.push_back(Join(best_tokens, 0));

        same = 1;
        while (i + same < tks1.size() && j + same < tks.size() && tks1[i + same] == tks[j + same])
            same++;
        res.push_back(Join(tks, j, j + same));
        _i = i + same;
        _j = j + same;
        j = _j + 1;
        i = _i + 1;
    }
    if (_i < tks1.size()) {
        Vector<Pair<String, int>> pre_tokens;
        Vector<Vector<Pair<String, int>>> token_list;
        Vector<String> best_tokens;
        double max_score = std::numeric_limits<double>::lowest();
        const auto str_for_dfs = Join(tks, _j, tks.size(), "");
#ifdef INFINITY_DEBUG
        const auto t0 = std::chrono::high_resolution_clock::now();
#endif
        DFS(str_for_dfs, 0, pre_tokens, token_list, best_tokens, max_score, false);
#ifdef INFINITY_DEBUG
        const auto t1 = std::chrono::high_resolution_clock::now();
        dp_debug::CheckDP(this, str_for_dfs, best_tokens, max_score, t0, t1);
#endif
        res.push_back(Join(best_tokens, 0));
    }

#else
    Vector<int> diff(std::max(tks.size(), tks1.size()), 0);
    for (std::size_t i = 0; i < std::min(tks.size(), tks1.size()); ++i) {
        if (tks[i] != tks1[i]) {
            diff[i] = 1;
        }
    }

    if (s1 > s) {
        tks = tks1;
    }

    std::size_t i = 0;
    while (i < tks.size()) {
        std::size_t s = i;
        while (s < tks.size() && diff[s] == 0) {
            s++;
        }
        if (s == tks.size()) {
            res.push_back(Join(tks, i, tks.size()));
            break;
        }
        if (s > i) {
            res.push_back(Join(tks, i, s));
        }

        std::size_t e = s;
        while (e < tks.size() && e - s < 5 && diff[e] == 1) {
            e++;
        }

        Vector<Pair<String, int>> pre_tokens;
        Vector<Vector<Pair<String, int>>> token_list;
        Vector<String> best_tokens;
        double max_score = std::numeric_limits<double>::lowest();
        const auto str_for_dfs = Join(tks, s, e < tks.size() ? e + 1 : e, "");
#ifdef INFINITY_DEBUG
        const auto t0 = std::chrono::high_resolution_clock::now();
#endif
        DFS(str_for_dfs, 0, pre_tokens, token_list, best_tokens, max_score, false);
#ifdef INFINITY_DEBUG
        const auto t1 = std::chrono::high_resolution_clock::now();
        dp_debug::CheckDP(this, str_for_dfs, best_tokens, max_score, t0, t1);
#endif
        // Vector<Pair<Vector<String>, double>> sorted_tokens;
        // SortTokens(token_list, sorted_tokens);
        // res.push_back(Join(sorted_tokens[0].first, 0));
        res.push_back(Join(best_tokens, 0));
        i = e + 1;
    }
#endif
}

void RAGAnalyzer::SplitLongText(const String &L, u32 length, Vector<String> &sublines) const {
    u32 slice_count = length / MAX_SENTENCE_LEN + 1;
    sublines.reserve(slice_count);
    std::size_t last_sentence_start = 0;
    std::size_t next_sentence_start = 0;
    for (unsigned i = 0; i < slice_count; ++i) {
        next_sentence_start = MAX_SENTENCE_LEN * (i + 1) - 5;
        if (next_sentence_start + 5 < length) {
            std::size_t sentence_length = MAX_SENTENCE_LEN * (i + 1) + 5 > length ? length - next_sentence_start : 10;
            String substr = UTF8Substr(L, next_sentence_start, sentence_length);
            auto [tks, s] = MaxForward(substr);
            auto [tks1, s1] = MaxBackward(substr);
            Vector<int> diff(std::max(tks.size(), tks1.size()), 0);
            for (std::size_t j = 0; j < std::min(tks.size(), tks1.size()); ++j) {
                if (tks[j] != tks1[j]) {
                    diff[j] = 1;
                }
            }

            if (s1 > s) {
                tks = tks1;
            }
            std::size_t start = 0;
            std::size_t forward_same_len = 0;
            while (start < tks.size() && diff[start] == 0) {
                forward_same_len += UTF8Length(tks[start]);
                start++;
            }
            if (forward_same_len == 0) {
                std::size_t end = tks.size() - 1;
                std::size_t backward_same_len = 0;
                while (end >= 0 && diff[end] == 0) {
                    backward_same_len += UTF8Length(tks[end]);
                    end--;
                }
                next_sentence_start += sentence_length - backward_same_len;
            } else
                next_sentence_start += forward_same_len;
        } else
            next_sentence_start = length;
        if (next_sentence_start == last_sentence_start)
            continue;
        String str = UTF8Substr(L, last_sentence_start, next_sentence_start - last_sentence_start);
        sublines.push_back(str);
        last_sentence_start = next_sentence_start;
    }
}

String RAGAnalyzer::Tokenize(const String &line) {
    String str1 = StrQ2B(line);
    String strline;
    opencc_->convert(str1, strline);
    Vector<String> res;
    std::size_t alpha_num = 0;
    int len = UTF8Length(strline);
    for (int i = 0; i < len; ++i) {
        String t = UTF8Substr(strline, i, 1);
        if (IsAlphabet(t)) {
            alpha_num++;
        }
    }
    if (alpha_num > (std::size_t)(len * 0.9)) {
        Vector<String> term_list;
        Vector<String> sentences;
        SentenceSplitter(line, sentences);
        for (auto &sentence : sentences) {
            nltk_tokenizer_->Tokenize(sentence, term_list);
        }
        for (unsigned i = 0; i < term_list.size(); ++i) {
            String t = lemma_->Lemmatize(term_list[i]);
            char *lowercase_term = lowercase_string_buffer_.data();
            ToLower(t.c_str(), t.size(), lowercase_term, term_string_buffer_limit_);
            String stem_term;
            stemmer_->Stem(lowercase_term, stem_term);
            res.push_back(stem_term);
        }
        String ret = Join(res, 0);
        return ret;
    }

    Vector<String> arr;
    Split(strline, regex_split_pattern_, arr, true);
    for (const auto &L : arr) {
        auto length = UTF8Length(L);
        if (length < 2 || RE2::PartialMatch(L, pattern2_) || RE2::PartialMatch(L, pattern3_)) { //[a-z\\.-]+$  [0-9\\.-]+$
            res.push_back(L);
            continue;
        }
#if 1
        if (length > MAX_SENTENCE_LEN) {
            Vector<String> sublines;
            SplitLongText(L, length, sublines);
            for (auto &l : sublines) {
                TokenizeInner(res, l);
            }
        } else
#endif
            TokenizeInner(res, L);
    }
    Vector<String> normalize_res;
    EnglishNormalize(res, normalize_res);
    String r = Join(normalize_res, 0);
    String ret = Merge(r);
    return ret;
}

void RAGAnalyzer::FineGrainedTokenize(const String &tokens, Vector<String> &result) {
    Vector<String> tks;
    Split(tokens, blank_pattern_, tks);
    Vector<String> res;
    std::size_t zh_num = 0;
    for (auto &token : tks) {
        int len = UTF8Length(token);
        for (int i = 0; i < len; ++i) {
            String t = UTF8Substr(token, i, 1);
            if (IsChinese(t)) {
                zh_num++;
            }
        }
    }
    if (zh_num < tks.size() * 0.2) {
        for (auto &token : tks) {
            std::istringstream iss(token);
            String sub_token;
            while (std::getline(iss, sub_token, '/')) {
                result.push_back(sub_token);
            }
        }
        // String ret = Join(res, 0);
        return;
    }

    for (auto &token : tks) {
        const auto token_len = UTF8Length(token);
        if (token_len < 3 || RE2::PartialMatch(token, pattern4_)) { //[0-9,\\.-]+$
            res.push_back(token);
            continue;
        }
        Vector<Vector<Pair<String, int>>> token_list;
        if (token_len > 10) {
            Vector<Pair<String, int>> tk;
            tk.emplace_back(token, Encode(-1, 0));
            token_list.push_back(tk);
        } else {
            Vector<Pair<String, int>> pre_tokens;
            Vector<String> best_tokens;
            double max_score = 0.0F;
#ifdef INFINITY_DEBUG
            const auto t0 = std::chrono::high_resolution_clock::now();
#endif
            DFS(token, 0, pre_tokens, token_list, best_tokens, max_score, true);
#ifdef INFINITY_DEBUG
            const auto t1 = std::chrono::high_resolution_clock::now();
            auto get_dfs_sorted_tokens = [&]() {
                Vector<Pair<Vector<String>, double>> sorted_tokens;
                SortTokens(token_list, sorted_tokens);
                return sorted_tokens;
            };
            dp_debug::CheckDP2(this, token, get_dfs_sorted_tokens, t0, t1);
#endif
        }
        if (token_list.size() < 2) {
            res.push_back(token);
            continue;
        }
        Vector<Pair<Vector<String>, double>> sorted_tokens;
        SortTokens(token_list, sorted_tokens);
        const auto &stk = sorted_tokens[1].first;
        if (stk.size() == token_len) {
            res.push_back(token);
        } else if (RE2::PartialMatch(token, pattern5_)) { // [a-z\\.-]+
            bool need_append_stk = true;
            for (auto &t : stk) {
                if (UTF8Length(t) < 3) {
                    res.push_back(token);
                    need_append_stk = false;
                    break;
                }
            }
            if (need_append_stk) {
                for (auto &t : stk) {
                    res.push_back(t);
                }
            }
        } else {
            for (auto &t : stk) {
                res.push_back(t);
            }
        }
    }
    EnglishNormalize(res, result);
    // String ret = Join(normalize_res, 0);
    // return ret;
}

int RAGAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    Vector<String> tokens;
    String output = Tokenize(input.text_);
    if (fine_grained_) {
        FineGrainedTokenize(output, tokens);
    } else
        Split(output, blank_pattern_, tokens);
    unsigned offset = 0;
    for (auto &t : tokens) {
        func(data, t.c_str(), t.size(), offset++, 0, false, 0);
    }
    return 0;
}

} // namespace infinity