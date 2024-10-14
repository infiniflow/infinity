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

#include <cmath>
#include <filesystem>
#include <fstream>

#include <openccxx.h>
#include <pcre2.h>
#include <re2/re2.h>

#include "string_utils.h"

module rag_analyzer;

import stl;
import term;
import stemmer;
import analyzer;
import darts_trie;
import lemmatizer;
import stemmer;
import term;

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

void Split(const String &input, const String &split_pattern, std::vector<String> &result, bool keep_delim = false) {
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
            result.push_back(std::string(extracted_delim_token.data(), extracted_delim_token.size()));
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

bool RegexMatch(const String &str, const String &pattern) { return RE2::PartialMatch(str, RE2(pattern)); }

String Join(const Vector<String> &tokens, int start, int end, const String &delim = " ") {
    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        if (i > start)
            oss << delim;
        oss << tokens[i];
    }
    return oss.str();
}

String Join(const Vector<String> &tokens, int start, const String &delim = " ") { return Join(tokens, start, tokens.size(), delim); }

String Join(const TermList &tokens, int start, int end, const String &delim = " ") {
    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        if (i > start)
            oss << delim;
        oss << tokens[i].text_;
    }
    return oss.str();
}

std::wstring UTF8ToWide(const String &utf8) {
    std::wstring result;
    int i = 0, length = utf8.length();

    while (i < length) {
        wchar_t wchar;
        unsigned char byte1 = utf8[i];

        if (byte1 <= 0x7F) {
            wchar = byte1;
            i += 1;
        } else if ((byte1 & 0xE0) == 0xC0) {
            if (i + 1 >= length)
                throw std::runtime_error("Invalid UTF-8 string");
            wchar = (byte1 & 0x1F) << 6 | (utf8[i + 1] & 0x3F);
            i += 2;
        } else if ((byte1 & 0xF0) == 0xE0) {
            if (i + 2 >= length)
                throw std::runtime_error("Invalid UTF-8 string");
            wchar = (byte1 & 0x0F) << 12 | (utf8[i + 1] & 0x3F) << 6 | (utf8[i + 2] & 0x3F);
            i += 3;
        } else {
            throw std::runtime_error("Invalid UTF-8 string");
        }

        result += wchar;
    }

    return result;
}

String WideCharToUTF8(wchar_t wchar) {
    String result;

    if (wchar <= 0x7F) {
        result += static_cast<char>(wchar); // 1 byte
    } else if (wchar <= 0x7FF) {
        result += static_cast<char>(0xC0 | ((wchar >> 6) & 0x1F)); // 2 bytes
        result += static_cast<char>(0x80 | (wchar & 0x3F));
    } else if (wchar <= 0xFFFF) {
        result += static_cast<char>(0xE0 | ((wchar >> 12) & 0x0F)); // 3 bytes
        result += static_cast<char>(0x80 | ((wchar >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (wchar & 0x3F));
    } else if (wchar <= 0x10FFFF) {
        result += static_cast<char>(0xF0 | ((wchar >> 18) & 0x07)); // 4 bytes
        result += static_cast<char>(0x80 | ((wchar >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((wchar >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (wchar & 0x3F));
    }

    return result;
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

RAGAnalyzer::RAGAnalyzer(const String &path)
    : dict_path_(path), lowercase_string_buffer_(term_string_buffer_limit_), regex_tokenizer_(MakeUnique<RegexTokenizer>()) {}

RAGAnalyzer::RAGAnalyzer(const RAGAnalyzer &other)
    : own_dict_(false), trie_(other.trie_), pos_table_(other.pos_table_), lemma_(other.lemma_), stemmer_(other.stemmer_), opencc_(other.opencc_),
      lowercase_string_buffer_(term_string_buffer_limit_), fine_grained_(other.fine_grained_), regex_tokenizer_(MakeUnique<RegexTokenizer>()) {}

RAGAnalyzer::~RAGAnalyzer() {
    if (own_dict_) {
        delete trie_;
        delete pos_table_;
        delete lemma_;
        delete stemmer_;
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

    stemmer_ = new Stemmer();
    InitStemmer(STEM_LANG_ENGLISH);

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
    std::wstring wide_str = UTF8ToWide(input);
    String result;

    for (wchar_t wchar : wide_str) {
        int code = static_cast<int>(wchar);
        if (code == 0x3000) {
            result += ' ';
        } else if (code >= 0xFF01 && code <= 0xFF5E) {
            // Convert full-width characters to half-width
            result += static_cast<char>(code - 0xfee0);
        } else {
            result += WideCharToUTF8(wchar);
        }
    }

    return result;
}

i32 RAGAnalyzer::Freq(const String &key) {
    i32 v = trie_->Get(key);
    v = DecodeFreq(v);
    return i32(std::exp(v) * DENOMINATOR + 0.5);
}

String RAGAnalyzer::Key(const String &line) { return ToLowerString(line); }

String RAGAnalyzer::RKey(const String &line) {
    String reversed;
    for (size_t i = line.size(); i > 0;) {
        size_t start = i - 1;
        while (start > 0 && (line[start] & 0xC0) == 0x80) {
            --start;
        }
        reversed += line.substr(start, i - start);
        i = start;
    }
    return "DD" + ToLowerString(reversed);
}

Pair<Vector<String>, double> RAGAnalyzer::Score(const Vector<Pair<String, int>> &token_freqs) {
    const int B = 30;
    double F = 0.0, L = 0.0;
    Vector<String> tokens;
    tokens.reserve(token_freqs.size());

    for (const auto &tk_freq_tag : token_freqs) {
        const String &token = tk_freq_tag.first;
        const auto &freq_tag = tk_freq_tag.second;
        i32 freq = DecodeFreq(freq_tag);

        F += freq;
        L += (UTF8Length(token) < 2) ? 0 : 1;
        tokens.push_back(token);
    }

    if (!tokens.empty()) {
        F /= tokens.size();
        L /= tokens.size();
    }

    double score = B / static_cast<double>(tokens.size()) + L + F;
    return {tokens, score};
}

void RAGAnalyzer::SortTokens(const Vector<Vector<Pair<String, int>>> &token_list, Vector<Pair<Vector<String>, double>> &res) {
    for (const auto &tfts : token_list) {
        auto [tks, score] = Score(tfts);
        res.emplace_back(tks, score);
    }

    std::sort(res.begin(), res.end(), [](const auto &a, const auto &b) { return a.second > b.second; });
}

Pair<Vector<String>, double> RAGAnalyzer::MaxForward(const String &line) {
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
            res.emplace_back(t, v);
        } else {
            res.emplace_back(t, 0);
        }

        s = e;
    }

    return Score(res);
}

Pair<Vector<String>, double> RAGAnalyzer::MaxBackward(const String &line) {
    Vector<Pair<String, int>> res;
    int s = UTF8Length(line) - 1;

    while (s >= 0) {
        int e = s + 1;
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
        if (v != -1)
            res.emplace_back(t, v);
        else
            res.emplace_back(t, 0);

        s -= 1;
    }

    std::reverse(res.begin(), res.end());
    return Score(res);
}

int RAGAnalyzer::DFS(const String &chars, int s, Vector<Pair<String, int>> &pre_tokens, Vector<Vector<Pair<String, int>>> &token_list) {
    int res = s;
    int len = UTF8Length(chars);
    if (s >= len) {
        token_list.push_back(pre_tokens);
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

        if (trie_->HasKeysWithPrefix(k)) {
            auto pretks = pre_tokens;
            int v = trie_->Get(k);
            if (v != -1)
                pretks.emplace_back(t, v);
            else
                pretks.emplace_back(t, Encode(-12, 0));
            res = std::max(res, DFS(chars, e, pretks, token_list));
        }
    }

    if (res > s) {
        return res;
    }

    String t = UTF8Substr(chars, s, 1);
    String k = Key(t);
    int v = trie_->Get(k);
    if (v != -1)
        pre_tokens.emplace_back(t, v);
    else
        pre_tokens.emplace_back(t, Encode(-12, 0));

    return DFS(chars, s + 1, pre_tokens, token_list);
}

String RAGAnalyzer::Merge(const String &tks_str) {
    String tks = tks_str;

    RE2 re_space(R"#(([ ]+))#");
    tks = Replace(re_space, " ", tks);

    Vector<String> tokens;
    Split(tks, "( )", tokens);
    Vector<String> res;
    std::size_t s = 0;
    while (true) {
        if (s >= tokens.size())
            break;

        std::size_t E = s + 1;
        for (std::size_t e = s + 2; e < std::min(tokens.size() + 1, s + 6); ++e) {
            String tk = Join(tokens, s, e, "");
            if (RE2::PartialMatch(tk, REGEX_SPLIT_CHAR)) {
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
        if (RegexMatch(t, "[a-zA-Z_-]+$")) {
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

String RAGAnalyzer::Tokenize(const String &line) {
    String str1 = StrQ2B(line);
    String strline;
    opencc_->convert(str1, strline);
    Vector<String> res;
    std::size_t zh_num = 0;
    int len = UTF8Length(strline);
    for (int i = 0; i < len; ++i) {
        String t = UTF8Substr(strline, i, 1);
        if (IsChinese(t)) {
            zh_num++;
        }
    }
    if (zh_num == 0) {
        TermList term_list;
        // tokenizer_.Tokenize(line, term_list);
        regex_tokenizer_->RegexTokenize(line, term_list);
        for (unsigned i = 0; i < term_list.size(); ++i) {
            String t = lemma_->Lemmatize(term_list[i].text_);
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
    Split(strline, REGEX_SPLIT_CHAR, arr, true);
    for (const auto &L : arr) {
        if (UTF8Length(L) < 2 || RegexMatch(L, "[a-z\\.-]+$") || RegexMatch(L, "[0-9\\.-]+$")) {
            res.push_back(L);
            continue;
        }
        auto [tks, s] = MaxForward(L);
        auto [tks1, s1] = MaxBackward(L);

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
            DFS(Join(tks, s, e < tks.size() ? e + 1 : e, ""), 0, pre_tokens, token_list);
            Vector<Pair<Vector<String>, double>> sorted_tokens;
            SortTokens(token_list, sorted_tokens);
            res.push_back(Join(sorted_tokens[0].first, 0));
            i = e + 1;
        }
    }

    Vector<String> normalize_res;
    EnglishNormalize(res, normalize_res);
    String r = Join(normalize_res, 0);
    String ret = Merge(r);
    return ret;
}

void RAGAnalyzer::FineGrainedTokenize(const String &tokens, Vector<String> &result) {
    Vector<String> tks;
    Split(tokens, "( )", tks);
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
        if (UTF8Length(token) < 3 || RegexMatch(token, "[0-9,\\.-]+$")) {
            res.push_back(token);
            continue;
        }
        Vector<Vector<Pair<String, int>>> token_list;
        if (UTF8Length(token) > 10) {
            Vector<Pair<String, int>> tk;
            tk.emplace_back(token, Encode(-1, 0));
            token_list.push_back(tk);
        } else {
            Vector<Pair<String, int>> pre_tokens;
            DFS(token, 0, pre_tokens, token_list);
        }
        if (token_list.size() < 2) {
            res.push_back(token);
            continue;
        }
        Vector<Pair<Vector<String>, double>> sorted_tokens;
        SortTokens(token_list, sorted_tokens);
        auto stk = sorted_tokens[1].first;
        String s_token;
        if (stk.size() == token.length()) {
            s_token = token;
        } else if (RegexMatch(token, "[a-z\\.-]+")) {
            for (auto &t : stk) {
                if (UTF8Length(t) < 3) {
                    s_token = token;
                    break;
                }
            }
        } else {
            s_token = Join(stk, 0);
        }
        res.push_back(s_token);
    }
    EnglishNormalize(res, result);
    // String ret = Join(normalize_res, 0);
    // return ret;
}

int RAGAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    unsigned level = 0;
    Vector<String> tokens;
    String output = Tokenize(input.text_.c_str());
    if (fine_grained_) {
        FineGrainedTokenize(output, tokens);
    } else
        Split(output, "( )", tokens);
    unsigned offset = 0;
    for (auto &t : tokens) {
        func(data, t.c_str(), t.size(), offset++, 0, Term::AND, level, false);
    }
    return 0;
}

} // namespace infinity