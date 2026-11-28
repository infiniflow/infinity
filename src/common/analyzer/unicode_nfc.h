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

// Unicode NFC normalization, the _compose() step of rag_tokenizer.py: NFC,
// except that the protected code points (Full_Composition_Exclusion and the
// code points not assigned in Unicode 13.0) stay as they are and nothing
// composes across them. The tables come from Python's unicodedata through
// tools/generate_unicode_nfc.py, which leaves the protected code points out of
// them, so both sides compose the same text; the Rust port (unicode_nfc.rs)
// uses the same tables and algorithm.
//
// The input is split into segments, each starting at a boundary: a code point
// with ccc 0 and NFC_Quick_Check=Yes, or a protected one. A segment that
// passes the quick check is already NFC; any other one is decomposed, put in
// canonical order and composed again (UAX #15). Segments normalize
// independently, so text that is already NFC is never copied.

#include "unicode_nfc_data.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace infinity {

// Where the bytes of an NFC form come from in its input; see NormalizeNFC.
struct NFCPositionMap {
    std::vector<unsigned> start;
    std::vector<unsigned> end;
};

namespace nfc_detail {

// Hangul syllables are composed and decomposed arithmetically (Unicode 3.12).
inline constexpr char32_t HANGUL_S_BASE = 0xAC00;
inline constexpr char32_t HANGUL_L_BASE = 0x1100;
inline constexpr char32_t HANGUL_V_BASE = 0x1161;
inline constexpr char32_t HANGUL_T_BASE = 0x11A7;
inline constexpr char32_t HANGUL_L_COUNT = 19;
inline constexpr char32_t HANGUL_V_COUNT = 21;
inline constexpr char32_t HANGUL_T_COUNT = 28;
inline constexpr char32_t HANGUL_N_COUNT = HANGUL_V_COUNT * HANGUL_T_COUNT;
inline constexpr char32_t HANGUL_S_COUNT = HANGUL_L_COUNT * HANGUL_N_COUNT;

// A code point of a segment being normalized, and the input bytes
// [offset, end) it comes from.
struct NFCUnit {
    char32_t cp;
    uint8_t ccc;
    unsigned offset;
    unsigned end;
};

// Decodes the UTF-8 sequence at in[i] into cp and returns its length, or 0
// when in[i] does not start a well-formed sequence (Unicode Table 3-7):
// truncated, overlong, surrogate, beyond U+10FFFF or a stray continuation.
inline std::size_t DecodeUTF8(std::string_view in, std::size_t i, char32_t &cp) {
    const auto lead = static_cast<unsigned char>(in[i]);
    if (lead < 0x80) {
        cp = lead;
        return 1;
    }
    std::size_t len = 0;
    unsigned char lo = 0x80;
    unsigned char hi = 0xBF;
    if (lead >= 0xC2 && lead <= 0xDF) {
        len = 2;
        cp = lead & 0x1F;
    } else if (lead >= 0xE0 && lead <= 0xEF) {
        len = 3;
        cp = lead & 0x0F;
        lo = lead == 0xE0 ? 0xA0 : 0x80;
        hi = lead == 0xED ? 0x9F : 0xBF;
    } else if (lead >= 0xF0 && lead <= 0xF4) {
        len = 4;
        cp = lead & 0x07;
        lo = lead == 0xF0 ? 0x90 : 0x80;
        hi = lead == 0xF4 ? 0x8F : 0xBF;
    } else {
        return 0;
    }
    if (in.size() - i < len) {
        return 0;
    }
    for (std::size_t k = 1; k < len; ++k) {
        const auto byte = static_cast<unsigned char>(in[i + k]);
        if (byte < (k == 1 ? lo : 0x80) || byte > (k == 1 ? hi : 0xBF)) {
            return 0;
        }
        cp = (cp << 6) | (byte & 0x3F);
    }
    return len;
}

inline void EncodeUTF8(char32_t cp, std::string &out) {
    if (cp < 0x80) {
        out += static_cast<char>(cp);
    } else if (cp < 0x800) {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

// The NFC_NON_BOUNDARY range holding cp, or nullptr when cp is a boundary
// (ccc 0 and NFC_Quick_Check=Yes, or protected), as every code point below
// U+0300 is.
inline const NFCRange *FindNonBoundary(char32_t cp) {
    if (cp < NFC_NON_BOUNDARY[0].first) {
        return nullptr;
    }
    const NFCRange *it =
        std::upper_bound(std::begin(NFC_NON_BOUNDARY), std::end(NFC_NON_BOUNDARY), cp, [](char32_t c, const NFCRange &r) { return c < r.first; });
    --it;
    return cp <= it->last ? it : nullptr;
}

inline uint8_t CanonicalCombiningClass(char32_t cp) {
    const NFCRange *range = FindNonBoundary(cp);
    return range ? range->ccc : 0;
}

// Appends the full canonical decomposition of cp to buf; every code point of
// it comes from input bytes [offset, end).
inline void Decompose(char32_t cp, unsigned offset, unsigned end, std::vector<NFCUnit> &buf) {
    if (cp >= HANGUL_S_BASE && cp < HANGUL_S_BASE + HANGUL_S_COUNT) {
        const char32_t s = cp - HANGUL_S_BASE;
        buf.push_back({HANGUL_L_BASE + s / HANGUL_N_COUNT, 0, offset, end});
        buf.push_back({HANGUL_V_BASE + s % HANGUL_N_COUNT / HANGUL_T_COUNT, 0, offset, end});
        if (s % HANGUL_T_COUNT != 0) {
            buf.push_back({HANGUL_T_BASE + s % HANGUL_T_COUNT, 0, offset, end});
        }
        return;
    }
    const NFCDecomposition *it =
        std::lower_bound(std::begin(NFC_DECOMPOSITIONS), std::end(NFC_DECOMPOSITIONS), cp, [](const NFCDecomposition &d, char32_t c) {
            return d.cp < c;
        });
    if (it != std::end(NFC_DECOMPOSITIONS) && it->cp == cp) {
        Decompose(it->first, offset, end, buf);
        Decompose(it->second, offset, end, buf);
        return;
    }
    buf.push_back({cp, CanonicalCombiningClass(cp), offset, end});
}

// The primary composite of first + second, or 0 when there is none.
inline char32_t Compose(char32_t first, char32_t second) {
    if (first >= HANGUL_L_BASE && first < HANGUL_L_BASE + HANGUL_L_COUNT && second >= HANGUL_V_BASE && second < HANGUL_V_BASE + HANGUL_V_COUNT) {
        return HANGUL_S_BASE + ((first - HANGUL_L_BASE) * HANGUL_V_COUNT + second - HANGUL_V_BASE) * HANGUL_T_COUNT;
    }
    if (first >= HANGUL_S_BASE && first < HANGUL_S_BASE + HANGUL_S_COUNT && (first - HANGUL_S_BASE) % HANGUL_T_COUNT == 0 && second > HANGUL_T_BASE &&
        second < HANGUL_T_BASE + HANGUL_T_COUNT) {
        return first + (second - HANGUL_T_BASE);
    }
    const NFCComposition *it =
        std::lower_bound(std::begin(NFC_COMPOSITIONS),
                         std::end(NFC_COMPOSITIONS),
                         std::pair{first, second},
                         [](const NFCComposition &c, const std::pair<char32_t, char32_t> &key) { return std::pair{c.first, c.second} < key; });
    if (it != std::end(NFC_COMPOSITIONS) && it->first == first && it->second == second) {
        return it->composite;
    }
    return 0;
}

// Puts a decomposed segment in canonical order and composes it (UAX #15). A
// composite keeps its starter's offset and comes from the input bytes of
// everything it took in.
inline void ReorderAndCompose(std::vector<NFCUnit> &buf) {
    // Canonical ordering: a stable sort of each run of non-starters, so a long
    // run of marks stays O(n log n).
    for (auto it = buf.begin(); it != buf.end();) {
        if (it->ccc == 0) {
            ++it;
            continue;
        }
        const auto run_end = std::find_if(it, buf.end(), [](const NFCUnit &u) { return u.ccc == 0; });
        std::stable_sort(it, run_end, [](const NFCUnit &a, const NFCUnit &b) { return a.ccc < b.ccc; });
        it = run_end;
    }
    // Canonical composition. A code point is blocked from the last starter when
    // a code point kept in between has a ccc of 0 or not lower than its own.
    std::size_t starter = 0;
    std::size_t kept = 0;
    bool has_starter = false;
    uint8_t last_ccc = 0;
    for (std::size_t k = 0; k < buf.size(); ++k) {
        const NFCUnit unit = buf[k];
        if (has_starter && (last_ccc == 0 || last_ccc < unit.ccc)) {
            if (const char32_t composite = Compose(buf[starter].cp, unit.cp); composite != 0) {
                buf[starter].cp = composite;
                buf[starter].end = std::max(buf[starter].end, unit.end);
                continue;
            }
        }
        if (unit.ccc == 0) {
            starter = kept;
            has_starter = true;
        }
        last_ccc = unit.ccc;
        buf[kept++] = unit;
    }
    buf.resize(kept);
}

// Normalizes the segment in[start, end) into buf. Returns false when the
// segment is already NFC.
inline bool NormalizeSegment(std::string_view in, std::size_t start, std::size_t end, std::vector<NFCUnit> &buf) {
    buf.clear();
    char32_t cp = 0;
    for (std::size_t i = start; i < end;) {
        const std::size_t len = DecodeUTF8(in, i, cp);
        Decompose(cp, static_cast<unsigned>(i), static_cast<unsigned>(i + len), buf);
        i += len;
    }
    ReorderAndCompose(buf);
    std::size_t i = start;
    for (const NFCUnit &unit : buf) {
        if (i == end) {
            return true;
        }
        i += DecodeUTF8(in, i, cp);
        if (cp != unit.cp) {
            return true;
        }
    }
    return i != end;
}

// Appends a normalized segment; each byte of a code point records the input
// bytes [offset, end) it comes from, in map->start and map->end.
inline void AppendSegment(const std::vector<NFCUnit> &buf, std::string &out, NFCPositionMap *map) {
    for (const NFCUnit &unit : buf) {
        const std::size_t before = out.size();
        EncodeUTF8(unit.cp, out);
        if (map) {
            map->start.insert(map->start.end(), out.size() - before, unit.offset);
            map->end.insert(map->end.end(), out.size() - before, unit.end);
        }
    }
}

} // namespace nfc_detail

// Returns the NFC form of in, or nothing when in is already NFC. Bytes that
// are not well-formed UTF-8 are copied as they are and split segments.
//
// When a string is returned and map is set, map->start and map->end get
// out.size() + 1 entries each, which never decrease. Outside the segments NFC
// changed, each byte maps to its own input byte; inside such a segment, to the
// span of the input characters it comes from, which reordering can widen. A
// token at output bytes [a, b) comes from input bytes
// [map->start[a], map->end[b]): start[a] is the lowest input offset of the
// code points from output byte a on, end[b] the highest input end of the code
// points before output byte b (a composite comes from everything it took in).
// The span is never empty for a non-empty token and covers the input of every
// code point in it; when canonical reordering moved a mark, it also covers the
// marks it moved past, so it can overlap the next token.
inline std::optional<std::string> NormalizeNFC(std::string_view in, NFCPositionMap *map = nullptr) {
    using namespace nfc_detail;
    std::optional<std::string> out;
    std::size_t copied = 0; // in[0, copied) is already in *out
    std::vector<NFCUnit> buf;
    const auto copy_until = [&](std::size_t end) {
        out->append(in.substr(copied, end - copied));
        for (std::size_t i = copied; map && i < end; ++i) {
            map->start.push_back(static_cast<unsigned>(i));
            map->end.push_back(static_cast<unsigned>(i + 1));
        }
        copied = end;
    };
    const auto flush_segment = [&](std::size_t start, std::size_t end) {
        if (!NormalizeSegment(in, start, end, buf)) {
            return;
        }
        if (!out) {
            out.emplace();
            out->reserve(in.size());
            if (map) {
                map->start.clear();
                map->start.reserve(in.size() + 1);
                map->end.assign(1, 0);
                map->end.reserve(in.size() + 1);
            }
        }
        copy_until(start);
        AppendSegment(buf, *out, map);
        copied = end;
    };

    // The quick check of the current segment: every code point has
    // NFC_Quick_Check=Yes and the non-starters are in canonical order.
    std::size_t segment_start = 0;
    bool quick_check_yes = true;
    uint8_t last_ccc = 0;
    for (std::size_t i = 0; i < in.size();) {
        char32_t cp = 0;
        const std::size_t len = DecodeUTF8(in, i, cp);
        const NFCRange *range = len == 0 ? nullptr : FindNonBoundary(cp);
        if (range == nullptr) {
            if (!quick_check_yes) {
                flush_segment(segment_start, i);
            }
            // An invalid byte is a segment of its own.
            segment_start = len == 0 ? i + 1 : i;
            quick_check_yes = true;
            last_ccc = 0;
            i += len == 0 ? 1 : len;
            continue;
        }
        if (!range->quick_check_yes || (range->ccc != 0 && last_ccc > range->ccc)) {
            quick_check_yes = false;
        }
        last_ccc = range->ccc;
        i += len;
    }
    if (!quick_check_yes) {
        flush_segment(segment_start, in.size());
    }
    if (out) {
        copy_until(in.size());
        if (map) {
            // Per output byte so far: its code point's input [start, end).
            // start becomes the minimum from each byte on, end (shifted by
            // one) the maximum before each byte.
            map->start.push_back(static_cast<unsigned>(in.size()));
            for (std::size_t k = map->start.size() - 1; k-- > 0;) {
                map->start[k] = std::min(map->start[k], map->start[k + 1]);
            }
            for (std::size_t k = 1; k < map->end.size(); ++k) {
                map->end[k] = std::max(map->end[k], map->end[k - 1]);
            }
        }
    }
    return out;
}

} // namespace infinity
