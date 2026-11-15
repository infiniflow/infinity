// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

#pragma once

// Sparse gram extraction: content-defined, multi-length n-grams used to
// accelerate regular-expression search through a full-text index.
//
// The scheme follows the "sparse gram" idea: every window in
// [min_n, max_n] code points is selected when its two *boundary* bigrams are
// the two largest bigrams inside the window. The rule is a pure function of the
// window content (context free), so for any substring q of a document d:
//
//     grams(q) subset of grams(d)
//
// which is what makes the index sound: a query may only *require* grams, and a
// document that contains the literal necessarily has them. False positives are
// removed by the exact regular expression that runs afterwards.
//
// Selection probability for a window of n code points is 2 / ((n-1) * (n-2)),
// which is 1 at n = 3 and decays as 1/n^2, so the expected number of grams for
// a string of length L stays linear (about 2L) no matter how large max_n is.
//
// Script awareness
// ----------------
// A fixed window length cannot serve every script. Three bytes of Latin text is
// a useful, selective unit; for CJK one *character* is already three bytes of
// UTF-8 and carries comparable information, so "3 code points" would make
// single-character and two-character CJK queries unindexable. Dense scripts
// therefore get their own floor of one character (and two characters, which is
// the practical search unit for Chinese), while the shared rule above still
// supplies the longer, sparser windows. This keeps a Chinese corpus and an
// English corpus at comparable index cost per input byte.

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace infinity {

// Default length bounds, in code points.
constexpr uint32_t SPARSE_GRAM_DEFAULT_MIN = 3;
constexpr uint32_t SPARSE_GRAM_DEFAULT_MAX = 12;

// Longest window considered for a dense span before the sparse rule starts.
// All bigrams are already emitted for a dense span, so a three-character window
// would be redundant with them.
constexpr uint32_t SPARSE_GRAM_DENSE_SPARSE_MIN = 4;

struct SparseGramParams {
    uint32_t min_n{SPARSE_GRAM_DEFAULT_MIN};
    uint32_t max_n{SPARSE_GRAM_DEFAULT_MAX};
    // Emit, for every gram, the ASCII-lowercased form as well. Case-insensitive
    // patterns can then require the folded gram while the exact-case gram stays
    // available to case-sensitive patterns. Cutting a literal at non-ASCII
    // bytes remains necessary: ASCII folding cannot equate 'u' with 'u-umlaut'.
    bool fold{false};
};

/// Code points that are the atomic unit of their script: one character is
/// already worth roughly three Latin characters of information.
inline bool IsDenseCodePoint(uint32_t cp) {
    return (cp >= 0x3040 && cp <= 0x30FF) || // Hiragana, Katakana
           (cp >= 0x3400 && cp <= 0x4DBF) || // CJK Unified Ideographs Extension A
           (cp >= 0x4E00 && cp <= 0x9FFF) || // CJK Unified Ideographs
           (cp >= 0xAC00 && cp <= 0xD7AF) || // Hangul Syllables
           (cp >= 0xF900 && cp <= 0xFAFF) || // CJK Compatibility Ideographs
           (cp >= 0x20000 && cp <= 0x2FA1F); // CJK Extensions B .. F
}

/// Deterministic weight of a bigram. Only requires to be stable across the
/// indexer and the planner, and to spread ties thinly; splitmix64 gives both
/// without a table.
///
/// ASCII letters are folded first, which makes a window's selection invariant
/// under ASCII case: the lowercased copy of a window is then selected exactly
/// when the window itself is. Without that, `-fold` would index a lowercased
/// term for a window its own lowercased form would not have selected, and a
/// case-insensitive query requiring that term would lose the row.
inline uint64_t SparseGramBigramWeight(uint32_t first, uint32_t second) {
    const auto fold = [](uint32_t code_point) -> uint32_t { return (code_point >= 'A' && code_point <= 'Z') ? code_point - 'A' + 'a' : code_point; };
    uint64_t x = (static_cast<uint64_t>(fold(first)) << 32) | static_cast<uint64_t>(fold(second));
    x += 0x9E3779B97F4A7C15ull;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

/// Decode one UTF-8 sequence. Undecodable bytes are treated as single-byte code
/// points so that arbitrary bytes are never split in a way the caller cannot
/// reproduce.
inline uint32_t SparseGramDecodeUtf8(const char *data, uint32_t len, uint32_t &width) {
    const auto byte = [&](uint32_t index) -> uint32_t { return static_cast<unsigned char>(data[index]); };
    const uint32_t b0 = byte(0);
    if (b0 < 0x80) {
        width = 1;
        return b0;
    }
    if ((b0 & 0xE0) == 0xC0 && len >= 2 && (byte(1) & 0xC0) == 0x80) {
        width = 2;
        return ((b0 & 0x1F) << 6) | (byte(1) & 0x3F);
    }
    if ((b0 & 0xF0) == 0xE0 && len >= 3 && (byte(1) & 0xC0) == 0x80 && (byte(2) & 0xC0) == 0x80) {
        width = 3;
        return ((b0 & 0x0F) << 12) | ((byte(1) & 0x3F) << 6) | (byte(2) & 0x3F);
    }
    if ((b0 & 0xF8) == 0xF0 && len >= 4 && (byte(1) & 0xC0) == 0x80 && (byte(2) & 0xC0) == 0x80 && (byte(3) & 0xC0) == 0x80) {
        width = 4;
        return ((b0 & 0x07) << 18) | ((byte(1) & 0x3F) << 12) | ((byte(2) & 0x3F) << 6) | (byte(3) & 0x3F);
    }
    width = 1;
    return b0;
}

namespace sparse_gram_detail {

/// Decoded view of a text: code points, their byte offsets, and the byte
/// offset just past each of them. `offsets.size() == code_points.size() + 1`.
struct DecodedText {
    std::vector<uint32_t> code_points;
    std::vector<uint32_t> offsets;
    bool any_ascii_upper{false};

    void Reset(std::string_view text) {
        code_points.clear();
        offsets.clear();
        any_ascii_upper = false;
        uint32_t pos = 0;
        const uint32_t len = static_cast<uint32_t>(text.size());
        while (pos < len) {
            uint32_t width = 1;
            const uint32_t cp = SparseGramDecodeUtf8(text.data() + pos, len - pos, width);
            if (cp >= 'A' && cp <= 'Z') {
                any_ascii_upper = true;
            }
            code_points.push_back(cp);
            offsets.push_back(pos);
            pos += width;
        }
        offsets.push_back(len);
    }
};

/// ASCII-lowercase a byte range into `out`.
inline std::string_view AsciiLowerCopy(std::string &out, std::string_view text, uint32_t begin, uint32_t end) {
    out.assign(text.data() + begin, end - begin);
    for (char &ch : out) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return std::string_view(out);
}

/// Whether a window's bytes may be used as a term. A NUL would truncate the
/// term in the dictionary, which stores C strings.
inline bool WindowIsIndexable(std::string_view text, uint32_t begin, uint32_t end) {
    for (uint32_t i = begin; i < end; ++i) {
        if (text[i] == '\0') {
            return false;
        }
    }
    return true;
}

} // namespace sparse_gram_detail

/// Visit every selected gram of `text`.
///
/// `emit` is called as `emit(std::string_view gram, uint32_t byte_begin, uint32_t byte_end)`.
/// When `params.fold` is set, a window containing an ASCII uppercase byte is
/// emitted twice, the second time lowercased.
template <class Emit>
void ForEachSparseGram(std::string_view text, const SparseGramParams &params, Emit &&emit) {
    using namespace sparse_gram_detail;

    if (params.min_n == 0 || params.max_n < params.min_n) {
        return;
    }
    const uint32_t cp_count = static_cast<uint32_t>(text.size());
    if (cp_count == 0) {
        return;
    }

    DecodedText decoded;
    decoded.Reset(text);
    const uint32_t n = static_cast<uint32_t>(decoded.code_points.size());
    const auto &cp = decoded.code_points;
    const auto &off = decoded.offsets;
    std::string fold_buffer;

    const auto emit_window = [&](uint32_t begin_cp, uint32_t end_cp) {
        const uint32_t begin = off[begin_cp];
        const uint32_t end = off[end_cp];
        if (!WindowIsIndexable(text, begin, end)) {
            return;
        }
        emit(std::string_view(text.data() + begin, end - begin), begin, end);
        if (!params.fold) {
            return;
        }
        bool has_upper = false;
        for (uint32_t i = begin; i < end && !has_upper; ++i) {
            has_upper = text[i] >= 'A' && text[i] <= 'Z';
        }
        if (has_upper) {
            emit(AsciiLowerCopy(fold_buffer, text, begin, end), begin, end);
        }
    };

    // A window of L code points carries L-1 bigrams. Its two boundary bigrams
    // are the first and the last; the rule requires them to be the two largest
    // inside the window, so nothing in the interior may exceed either of them.
    const auto window_is_selected = [&](uint32_t begin_cp, uint32_t end_cp) {
        const uint32_t length = end_cp - begin_cp;
        if (length < 3) {
            // Fewer than two bigrams: no interior to compare against, so the
            // rule is degenerate. Dense floors handle these lengths.
            return false;
        }
        bool all_dense = true;
        for (uint32_t i = begin_cp; i < end_cp && all_dense; ++i) {
            all_dense = IsDenseCodePoint(cp[i]);
        }
        const uint32_t effective_min =
            all_dense ? (params.min_n > SPARSE_GRAM_DENSE_SPARSE_MIN ? params.min_n : SPARSE_GRAM_DENSE_SPARSE_MIN) : params.min_n;
        if (length < effective_min || length > params.max_n) {
            return false;
        }
        const uint64_t head = SparseGramBigramWeight(cp[begin_cp], cp[begin_cp + 1]);
        const uint64_t tail = SparseGramBigramWeight(cp[end_cp - 2], cp[end_cp - 1]);
        const uint64_t weaker_boundary = head < tail ? head : tail;
        for (uint32_t i = begin_cp + 1; i + 1 < end_cp - 1; ++i) {
            if (SparseGramBigramWeight(cp[i], cp[i + 1]) > weaker_boundary) {
                return false;
            }
        }
        return true;
    };

    for (uint32_t begin_cp = 0; begin_cp < n; ++begin_cp) {
        // Dense floor: one character of CJK, kana or hangul, and a two
        // character pair, which is the practical unit for Chinese search.
        if (IsDenseCodePoint(cp[begin_cp])) {
            emit_window(begin_cp, begin_cp + 1);
            if (begin_cp + 1 < n && IsDenseCodePoint(cp[begin_cp + 1])) {
                emit_window(begin_cp, begin_cp + 2);
            }
        }
        const uint32_t limit = (n - begin_cp) < params.max_n ? n : begin_cp + params.max_n;
        for (uint32_t end_cp = begin_cp + 3; end_cp <= limit; ++end_cp) {
            if (window_is_selected(begin_cp, end_cp)) {
                emit_window(begin_cp, end_cp);
            }
        }
    }
}

/// Whether a literal would contribute at least one gram. A pattern whose
/// literals all yield nothing must not be pushed down: an empty full-text query
/// is an error, not an empty result.
inline bool HasAnySparseGram(std::string_view text, const SparseGramParams &params) {
    bool found = false;
    ForEachSparseGram(text, params, [&found](std::string_view, uint32_t, uint32_t) { found = true; });
    return found;
}

/// Parse an analyzer name of the form
/// `sparsegram[-<min>[-<max>]][-fold]`. Returns false when the name is not a
/// sparse gram analyzer name.
inline bool ParseSparseGramAnalyzerName(std::string_view name, SparseGramParams &params) {
    constexpr std::string_view kBase = "sparsegram";
    if (name.size() < kBase.size() || name.substr(0, kBase.size()) != kBase) {
        return false;
    }
    params = SparseGramParams{};
    std::string_view rest = name.substr(kBase.size());
    uint32_t numbers[2] = {0, 0};
    int number_count = 0;
    while (!rest.empty()) {
        if (rest.front() != '-') {
            return false;
        }
        rest.remove_prefix(1);
        const auto dash = rest.find('-');
        std::string_view segment = rest.substr(0, dash);
        rest = dash == std::string_view::npos ? std::string_view{} : rest.substr(dash);
        if (segment == "fold") {
            params.fold = true;
            continue;
        }
        if (segment.empty() || number_count >= 2) {
            return false;
        }
        uint32_t value = 0;
        for (const char ch : segment) {
            if (ch < '0' || ch > '9') {
                return false;
            }
            value = value * 10 + static_cast<uint32_t>(ch - '0');
            if (value > 1024) {
                return false;
            }
        }
        numbers[number_count++] = value;
    }
    if (number_count >= 1) {
        params.min_n = numbers[0];
    }
    if (number_count >= 2) {
        params.max_n = numbers[1];
    }
    if (params.min_n == 0 || params.max_n < params.min_n) {
        return false;
    }
    return true;
}

} // namespace infinity
