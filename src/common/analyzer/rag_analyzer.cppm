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

#include "opencc/openccxx.h"

export module infinity_core:rag_analyzer;

import :term;
import :status;
import :darts_trie;
import :stemmer;
import :analyzer;
import :wordnet_lemmatizer;
import :logger;

import third_party;

namespace infinity {

// C++ reimplementation of
// https://github.com/infiniflow/ragflow/blob/main/rag/nlp/rag_tokenizer.py

class NLTKWordTokenizer;
export class RAGAnalyzer : public Analyzer {
public:
    RAGAnalyzer(const std::string &path);

    RAGAnalyzer(const RAGAnalyzer &other);

    ~RAGAnalyzer();

    void InitStemmer(Language language);

    void SetLanguage(const std::string &language);

    Status Load();

    void SetFineGrained(bool fine_grained) { fine_grained_ = fine_grained; }

    void SetEnablePosition(bool enable_position) { enable_position_ = enable_position; }

    // True if the term is in the canonical RAGFlow Chinese stopword set
    // (see rag/nlp/term_weight.py Dealer.stop_words). AnalyzeImpl drops
    // these terms from the emitted token stream.
    static bool IsStopword(const std::string &term);

    // Fold Latin-1 Supplement / Latin Extended-A letters whose NFD
    // decomposition is one ASCII letter plus combining marks to that base
    // letter, preserving case (Š→S, ď→d). Letters without such a
    // decomposition (æ, œ, ß, ø, ł, …) pass through unchanged. Applied to
    // the whole input before tokenization when SetLanguage selected a
    // diacritics-folding language (Slovak, Czech).
    static std::string FoldDiacritics(const std::string &input);

    std::pair<std::vector<std::string>, std::vector<std::pair<unsigned, unsigned>>> TokenizeWithPosition(const std::string &line);
    std::string Tokenize(const std::string &line);

    void FineGrainedTokenize(const std::string &tokens, std::vector<std::string> &result);

    void TokenizeInnerWithPosition(const std::string &L,
                                   std::vector<std::string> &tokens,
                                   std::vector<std::pair<unsigned, unsigned>> &positions,
                                   unsigned base_pos,
                                   const std::vector<unsigned> *pos_mapping = nullptr);
    // text is the input that positions index into.
    void FineGrainedTokenizeWithPosition(const std::string &text,
                                         const std::string &tokens_str,
                                         const std::vector<std::pair<unsigned, unsigned>> &positions,
                                         std::vector<std::string> &fine_tokens,
                                         std::vector<std::pair<unsigned, unsigned>> &fine_positions);
    void EnglishNormalizeWithPosition(const std::vector<std::string> &tokens,
                                      const std::vector<std::pair<unsigned, unsigned>> &positions,
                                      std::vector<std::string> &normalize_tokens,
                                      std::vector<std::pair<unsigned, unsigned>> &normalize_positions);
    unsigned MapToOriginalPosition(unsigned processed_pos, const std::vector<std::pair<unsigned, unsigned>> &mapping);
    void MergeWithPosition(const std::vector<std::string> &tokens,
                           const std::vector<std::pair<unsigned, unsigned>> &positions,
                           std::vector<std::string> &merged_tokens,
                           std::vector<std::pair<unsigned, unsigned>> &merged_positions);

    void SplitByLang(const std::string &line, std::vector<std::pair<std::string, bool>> &txt_lang_pairs) const;

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    static constexpr float DENOMINATOR = 1000000;

    static std::string StrQ2B(const std::string &input);

    static void BuildPositionMapping(const std::string &original, const std::string &converted, std::vector<unsigned> &pos_mapping);

    i32 Freq(std::string_view key) const;

    static std::string Key(std::string_view line);

    static std::string RKey(std::string_view line);

    static std::pair<std::vector<std::string>, double> Score(const std::vector<std::pair<std::string, int>> &token_freqs);

    static void SortTokens(const std::vector<std::vector<std::pair<std::string, int>>> &token_list,
                           std::vector<std::pair<std::vector<std::string>, double>> &res);

    std::pair<std::vector<std::string>, double> MaxForward(const std::string &line) const;

    std::pair<std::vector<std::string>, double> MaxBackward(const std::string &line) const;

    int DFS(const std::string &chars,
            int s,
            std::vector<std::pair<std::string, int>> &pre_tokens,
            std::vector<std::vector<std::pair<std::string, int>>> &token_list,
            std::vector<std::string> &best_tokens,
            double &max_score,
            bool memo_all,
            int depth = 0) const;

    void TokenizeInner(std::vector<std::string> &res, const std::string &L) const;

    void SplitLongText(const std::string &L, u32 length, std::vector<std::string> &sublines) const;

    std::string Merge(const std::string &tokens) const;

    void EnglishNormalize(const std::vector<std::string> &tokens, std::vector<std::string> &res);

public:
    std::vector<std::pair<std::vector<std::string_view>, double>> GetBestTokensTopN(std::string_view chars, u32 n) const;

    static const size_t term_string_buffer_limit_ = 4096 * 3;

    std::string dict_path_;

    bool own_dict_{};

    DartsTrie *trie_{nullptr};

    POSTable *pos_table_{nullptr};

    WordNetLemmatizer *wordnet_lemma_{nullptr};

    bool use_lemmatizer_{true}; // WordNet only supports English

    bool use_stemmer_{true}; // Disabled for languages without a Snowball stemmer (Slovak, Czech)

    bool fold_diacritics_{false}; // Fold Latin diacritics to ASCII before tokenization (Slovak, Czech)

    std::unique_ptr<Stemmer> stemmer_;

    OpenCC *opencc_{nullptr};

    std::vector<char> lowercase_string_buffer_;

    bool fine_grained_{false};

    bool enable_position_{false};

    // Used with FullMatch: only tokens made entirely of these characters are
    // lemmatized and stemmed, as in rag_tokenizer.py's _normalize_token.
    static inline re2::RE2 pattern1_{"[a-zA-Z_-]+"};

    static inline re2::RE2 pattern2_{"[a-zA-Z\\.-]+$"};

    static inline re2::RE2 pattern3_{"[0-9\\.-]+$"};

    static inline re2::RE2 pattern4_{"[0-9,\\.-]+$"};

    static inline re2::RE2 pattern5_{"[a-zA-Z\\.-]+"};

    // The word run's letters. rag_tokenizer.py lowercases the line and then
    // splits it on [a-zA-Z0-9À-ÖØ-öø-ɏḀ-ỿ]. This pattern splits the original
    // text and each token is lowercased after, so the class holds every code
    // point whose lowercase is one code point in Python's class: each boundary
    // falls where Python puts it. Against Python's class it drops Ⱥ U+023A,
    // Ⱦ U+023E and the 21 capitals whose lowercase is in IPA Extensions
    // (U+0181 ... U+0245), and adds the Kelvin and Angstrom signs U+212A and
    // U+212B, U+2C7E, U+2C7F and U+A7DC. It also drops İ U+0130, the one letter
    // whose lowercase is two code points: Python splits it between i and
    // U+0307, which no class on the original text can do. Generated with
    // Python 3.14 (3.13 gives the same without U+A7DC, unassigned in its
    // Unicode 15.1); is_split_word_char in the Rust port has the same ranges:
    //   P = re.compile(r"[a-zA-Z0-9À-ÖØ-öø-ɏḀ-ỿ]")
    //   cps = [i for i in range(0x110000) if not 0xD800 <= i <= 0xDFFF
    //          and len(chr(i).lower()) == 1 and P.fullmatch(chr(i).lower())]
    //   runs = [[cps[0], cps[0]]]
    //   for cp in cps[1:]:
    //       if cp == runs[-1][1] + 1: runs[-1][1] = cp
    //       else: runs.append([cp, cp])
    //   cls = "".join(f"\\x{{{a:04X}}}" + (f"-\\x{{{b:04X}}}" if b > a else "") for a, b in runs[3:])
    static inline re2::RE2 regex_split_pattern_{
        R"#(([ ,\.<>/?;:'\[\]\\`!@#$%^&*\(\)\{\}\|_+=《》，。？、；‘’：“”【】~！￥%……（）——-]+|[a-zA-Z0-9)#"
        R"#(\x{00C0}-\x{00D6}\x{00D8}-\x{00F6}\x{00F8}-\x{012F}\x{0131}-\x{0180}\x{0182}-\x{0185}\x{0187}-\x{0188}\x{018B}-\x{018E})#"
        R"#(\x{0191}-\x{0192}\x{0195}\x{0198}-\x{019B}\x{019E}\x{01A0}-\x{01A5}\x{01A7}-\x{01A8}\x{01AA}-\x{01AD}\x{01AF}-\x{01B0}\x{01B3}-\x{01B6})#"
        R"#(\x{01B8}-\x{0239}\x{023B}-\x{023D}\x{023F}-\x{0243}\x{0246}-\x{024F}\x{1E00}-\x{1EFF}\x{212A}-\x{212B}\x{2C7E}-\x{2C7F}\x{A7DC})#"
        R"#(,\.-]+))#"};

    static inline re2::RE2 blank_pattern_{"( )"};

    static inline re2::RE2 replace_space_pattern_{R"#(([ ]+))#"};
};

export void SentenceSplitter(const std::string &text, std::vector<std::string> &result);
} // namespace infinity
