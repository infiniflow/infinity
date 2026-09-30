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

// Extract, from a regular expression, the literal substrings that every match
// is guaranteed to contain.
//
// These runs are the only thing the full-text index can use: a run that must
// appear in every match can be turned into grams, and the grams into an index
// narrowing, while the exact regular expression still runs afterwards and
// removes false positives.
//
// The extractor only ever *under*-approximates: a run is returned only when the
// pattern structure proves it mandatory. Everything it cannot reason about
// either ends the current run or, for constructs where a wrong answer would be
// unsound (backreferences, conditionals, verbose mode, possessive quantifiers),
// abandons the whole pattern. Returning fewer or no runs costs pruning power and
// never correctness.

#include "common/utility/sparse_gram.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace infinity {

struct RegexLiteralRun {
    /// Bytes that must occur contiguously in every match.
    std::string text;
    /// Whether the run had to be ASCII-lowercased. Such a run is only usable
    /// against an index built with ASCII case folding.
    bool lowered{false};
};

struct RegexExtractionResult {
    bool case_insensitive{false};
    std::vector<RegexLiteralRun> runs;
};

namespace regex_literal_detail {

inline bool IsAsciiLetter(uint32_t cp) { return (cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z'); }

/// Whether a code point has a case that ASCII folding cannot handle. CJK, kana
/// and hangul are caseless, so a case-insensitive pattern may still require
/// them verbatim; accented Latin, Greek and Cyrillic may not.
inline bool IsUnfoldableCasedCodePoint(uint32_t cp) {
    if (cp < 0x80) {
        return false;
    }
    return !IsDenseCodePoint(cp);
}

/// Return the index just past the `]` closing the class at `pattern[open]`, or
/// `npos` when it is unterminated. A `]` in first position is a member, and
/// `[` nests, so `[[:digit:]]` closes on the second `]`.
inline size_t SkipClass(std::string_view pattern, size_t open) {
    size_t i = open + 1;
    if (i < pattern.size() && pattern[i] == '^') {
        ++i;
    }
    if (i < pattern.size() && pattern[i] == ']') {
        ++i;
    }
    size_t depth = 1;
    while (i < pattern.size()) {
        const char ch = pattern[i];
        if (ch == '\\') {
            i += 2;
            continue;
        }
        if (ch == '[') {
            ++depth;
        } else if (ch == ']') {
            if (--depth == 0) {
                return i + 1;
            }
        }
        ++i;
    }
    return std::string_view::npos;
}

/// Return the index just past the `}` of a `{n}`, `{n,}` or `{n,m}` repetition
/// starting at `pattern[open]`, or `npos` when the brace is a literal.
inline size_t RepetitionEnd(std::string_view pattern, size_t open, uint32_t &min_repeat) {
    size_t i = open + 1;
    const auto digits = [&](uint32_t &value) -> bool {
        const size_t start = i;
        value = 0;
        while (i < pattern.size() && pattern[i] >= '0' && pattern[i] <= '9') {
            value = value * 10 + static_cast<uint32_t>(pattern[i] - '0');
            ++i;
        }
        return i > start;
    };
    uint32_t lower = 0;
    if (!digits(lower)) {
        return std::string_view::npos;
    }
    if (i < pattern.size() && pattern[i] == ',') {
        ++i;
        uint32_t upper = 0;
        digits(upper);
    }
    if (i >= pattern.size() || pattern[i] != '}') {
        return std::string_view::npos;
    }
    min_repeat = lower;
    return i + 1;
}

} // namespace regex_literal_detail

/// Extract mandatory literal runs from `pattern`.
///
/// `case_insensitive` mirrors the pattern's own case folding. When it is set,
/// runs are lowercased and cut at code points whose case ASCII folding cannot
/// equate, and `allow_folded_index` must be true: without an index built with
/// ASCII folding the folded run would not match an uppercase occurrence.
inline RegexExtractionResult ExtractRegexLiteralRuns(std::string_view pattern, bool case_insensitive, bool allow_folded_index) {
    using namespace regex_literal_detail;

    RegexExtractionResult result;
    result.case_insensitive = case_insensitive;
    if (case_insensitive && !allow_folded_index) {
        return result;
    }

    // Group frames: runs collected inside a group that turns out to be one
    // branch of an alternation are not mandatory and get dropped when the group
    // closes. Runs outside it are unaffected.
    struct GroupFrame {
        size_t runs_begin;
        bool has_alternation;
        bool case_insensitive;
    };
    std::vector<GroupFrame> frames;
    std::vector<RegexLiteralRun> collected;
    std::string run;
    bool ci = case_insensitive;
    // Bytes contributed by the most recent atom, so an optional quantifier can
    // drop it again: `colou?r` must be planned as "colo" plus "r".
    size_t last_atom_len = 0;
    bool run_open = false;
    // Whether the most recent atom was a group, and where that group's entries
    // start in `collected`. A group contributes to `collected` rather than to
    // `run`, so `(?:abc)?` needs the treatment `colou?r` gives the `u` applied
    // to the group's entries: once the group closes it is the atom a following
    // quantifier can drop, and nothing else may claim that position.
    bool last_atom_is_group = false;
    size_t last_group_runs_begin = 0;
    // True right after a quantifier, where a following `?` is a laziness
    // modifier and a following `+` is a possessive modifier.
    bool after_quantifier = false;

    const auto flush = [&]() {
        if (!run.empty()) {
            if (ci && !allow_folded_index) {
                // The index has no folded terms, so nothing in this run can be
                // required without risking a missed match.
                run.clear();
            } else if (!ci) {
                collected.push_back(RegexLiteralRun{run, false});
            } else {
                // Keep only what the index's ASCII folding can answer for. A
                // caseless code point (CJK, kana, hangul) stays: any match must
                // contain it verbatim. A cased non-ASCII one cannot be required
                // at all, and splits the run in two.
                std::string piece;
                const auto flush_piece = [&]() {
                    if (!piece.empty()) {
                        collected.push_back(RegexLiteralRun{piece, true});
                        piece.clear();
                    }
                };
                for (size_t pos = 0; pos < run.size();) {
                    uint32_t width = 1;
                    const uint32_t cp = SparseGramDecodeUtf8(run.data() + pos, static_cast<uint32_t>(run.size() - pos), width);
                    if (IsUnfoldableCasedCodePoint(cp)) {
                        flush_piece();
                    } else if (cp >= 'A' && cp <= 'Z') {
                        piece.push_back(static_cast<char>(cp - 'A' + 'a'));
                    } else {
                        piece.append(run.data() + pos, width);
                    }
                    pos += width;
                }
                flush_piece();
            }
            run.clear();
        }
        // A flush ends the pending atom: the run was pushed or dropped and the
        // text that ended it cannot be rewound by a following quantifier.
        run_open = false;
        last_atom_len = 0;
        last_atom_is_group = false;
    };

    const auto bail = [&]() {
        result.runs.clear();
        return result;
    };

    size_t i = 0;
    while (i < pattern.size()) {
        const char ch = pattern[i];

        if (ch == '\\') {
            if (i + 1 >= pattern.size()) {
                return bail(); // dangling escape
            }
            const char next = pattern[i + 1];
            // Classes, anchors and word boundaries interrupt a literal.
            if (next == 'd' || next == 'D' || next == 'w' || next == 'W' || next == 's' || next == 'S' || next == 'b' || next == 'B' || next == 'A' ||
                next == 'z' || next == 'Z') {
                flush();
                i += 2;
                after_quantifier = false;
                continue;
            }
            // Constructs with no widening: a wrong literal here would drop real
            // matches.
            if ((next >= '1' && next <= '9') || next == 'k' || next == 'g' || next == 'K' || next == 'G') {
                return bail();
            }
            if (next == 'x') {
                // \xHH below 0x80 is an ASCII byte. Anything else is a raw byte
                // whose UTF-8 meaning the index does not share, and the hex
                // digits after it would otherwise be read as literals.
                const auto hex = [](char c) -> int {
                    if (c >= '0' && c <= '9') {
                        return c - '0';
                    }
                    if (c >= 'a' && c <= 'f') {
                        return c - 'a' + 10;
                    }
                    if (c >= 'A' && c <= 'F') {
                        return c - 'A' + 10;
                    }
                    return -1;
                };
                if (i + 3 < pattern.size()) {
                    const int hi = hex(pattern[i + 2]);
                    const int lo = hex(pattern[i + 3]);
                    if (hi >= 0 && lo >= 0 && (hi * 16 + lo) < 0x80) {
                        run.push_back(static_cast<char>(hi * 16 + lo));
                        last_atom_len = 1;
                        run_open = true;
                        last_atom_is_group = false;
                        i += 4;
                        after_quantifier = false;
                        continue;
                    }
                }
                return bail();
            }
            if (next == 'u' || next == 'p' || next == 'P') {
                // \uHHHH, \p{...}: the escape spellings and the following
                // characters are not literal text this scanner can read.
                return bail();
            }
            // \n, \t, \r, \f, \v are literal characters spelled as escapes.
            const auto push_escaped_char = [&](char value) {
                run.push_back(value);
                last_atom_len = 1;
                run_open = true;
                last_atom_is_group = false;
            };
            switch (next) {
                case 'n': {
                    push_escaped_char('\n');
                    ++i;
                    break;
                }
                case 'r': {
                    push_escaped_char('\r');
                    ++i;
                    break;
                }
                case 't': {
                    push_escaped_char('\t');
                    ++i;
                    break;
                }
                case 'f': {
                    push_escaped_char('\f');
                    ++i;
                    break;
                }
                case 'v': {
                    push_escaped_char('\v');
                    ++i;
                    break;
                }
                case '0': {
                    return bail(); // NUL cannot be a term
                }
                default: {
                    // Any other escaped character is that character literally.
                    run.push_back(next);
                    last_atom_len = 1;
                    run_open = true;
                    last_atom_is_group = false;
                    ++i;
                    break;
                }
            }
            i += 1;
            after_quantifier = false;
            continue;
        }

        if (ch == '[') {
            const size_t end = SkipClass(pattern, i);
            if (end == std::string_view::npos) {
                return bail();
            }
            flush();
            i = end;
            after_quantifier = false;
            continue;
        }

        if (ch == '(') {
            // Bytes the opener occupies: `(` alone, or the three of `(?:` and
            // `(?>` which share the plain-group frame setup below.
            size_t group_open_len = 1;
            // Classify the group.
            if (i + 1 < pattern.size() && pattern[i + 1] == '?') {
                if (i + 2 >= pattern.size()) {
                    return bail();
                }
                const char kind = pattern[i + 2];
                if (kind == '=' || kind == '!') {
                    // A lookaround is zero width: its content is not part of the
                    // match. A positive one does require the text to be present,
                    // but proving that needs the position the match starts at,
                    // so nothing inside is collected.
                    int depth = 1;
                    size_t j = i + 2;
                    while (j < pattern.size() && depth > 0) {
                        if (pattern[j] == '\\') {
                            j += 2;
                            continue;
                        }
                        if (pattern[j] == '[') {
                            const size_t class_end = SkipClass(pattern, j);
                            if (class_end == std::string_view::npos) {
                                return bail();
                            }
                            j = class_end;
                            continue;
                        }
                        if (pattern[j] == '(') {
                            ++depth;
                        } else if (pattern[j] == ')') {
                            --depth;
                        }
                        ++j;
                    }
                    if (depth != 0) {
                        return bail();
                    }
                    flush();
                    i = j;
                    after_quantifier = false;
                    continue;
                }
                if (kind == '<') {
                    if (i + 3 < pattern.size() && (pattern[i + 3] == '=' || pattern[i + 3] == '!')) {
                        // Lookbehind: same reasoning as lookahead.
                        int depth = 1;
                        size_t j = i + 3;
                        while (j < pattern.size() && depth > 0) {
                            if (pattern[j] == '\\') {
                                j += 2;
                                continue;
                            }
                            if (pattern[j] == '(') {
                                ++depth;
                            } else if (pattern[j] == ')') {
                                --depth;
                            }
                            ++j;
                        }
                        if (depth != 0) {
                            return bail();
                        }
                        flush();
                        i = j;
                        after_quantifier = false;
                        continue;
                    }
                    // `(?<name>...)` holds literals the way a plain group does,
                    // but this scanner would have to read the name to skip it
                    // and reading it as literal text would require characters
                    // the pattern never contains.
                    return bail();
                }
                if (kind == ':' || kind == '>') {
                    // Non-capturing and atomic groups contribute literals
                    // exactly like a plain group, so they share its frame setup
                    // and only skip past the longer opener. An atomic group
                    // matches a subset of what the same plain group matches, so
                    // a literal every plain-group match contains stays
                    // mandatory.
                    group_open_len = 3;
                } else if (kind == '(') {
                    return bail(); // conditional
                } else if (kind == '#') {
                    return bail(); // comment
                } else if (kind == 'P' && i + 3 < pattern.size() && pattern[i + 3] == '=') {
                    return bail(); // /(?P=name)/ backreference
                } else if (kind == 'i' || kind == 'm' || kind == 's' || kind == 'u' || kind == 'U' || kind == 'R' || kind == '-' || kind == 'x') {
                    // Inline flags. `x` changes how the pattern is lexed, so the
                    // rest of the pattern cannot be read by this scanner.
                    size_t j = i + 2;
                    bool enable = true;
                    bool saw_x = false;
                    while (j < pattern.size() && pattern[j] != ')' && pattern[j] != ':') {
                        if (pattern[j] == '-') {
                            enable = false;
                        } else if (pattern[j] == 'x') {
                            saw_x = true;
                        } else if (pattern[j] != 'i' && pattern[j] != 'm' && pattern[j] != 's' && pattern[j] != 'u' && pattern[j] != 'U' &&
                                   pattern[j] != 'R') {
                            return bail();
                        }
                        ++j;
                    }
                    if (j >= pattern.size()) {
                        return bail();
                    }
                    if (saw_x) {
                        return bail(); // whitespace and `#` stop being literal
                    }
                    // Neither form contributes literal text: `(?i:` is scoped to
                    // the group, `(?i)` to the rest of the enclosing one.
                    flush();
                    const bool scoped = pattern[j] == ':';
                    if (scoped) {
                        frames.push_back(GroupFrame{collected.size(), false, ci});
                    }
                    bool sets_case = false;
                    bool case_value = false;
                    for (size_t k = i + 2; k < j; ++k) {
                        if (pattern[k] == 'i') {
                            sets_case = true;
                            case_value = enable;
                        }
                    }
                    if (sets_case) {
                        ci = case_value;
                    }
                    i = j + 1;
                    after_quantifier = false;
                    continue;
                } else {
                    // `(?P<name>...)`, `(?1)`, `(?&name)` and anything else this
                    // scanner does not know: reading the opener as literal text
                    // would require characters the pattern never contains.
                    return bail();
                }
            }
            flush();
            // The frame remembers where this group's runs start, and the flush
            // above must already have happened so a run ending at the group
            // boundary is not attributed to the group.
            frames.push_back(GroupFrame{collected.size(), false, ci});
            i += group_open_len;
            after_quantifier = false;
            continue;
        }

        if (ch == ')') {
            if (!frames.empty()) {
                const GroupFrame frame = frames.back();
                frames.pop_back();
                ci = frame.case_insensitive;
                flush();
                if (frame.has_alternation) {
                    collected.resize(frame.runs_begin);
                }
                // The closed group is the most recent atom now. A quantifier
                // directly after it drops everything the group collected, and
                // `frame.runs_begin` is exactly where those entries start.
                last_atom_is_group = true;
                last_group_runs_begin = frame.runs_begin;
            }
            ++i;
            after_quantifier = false;
            continue;
        }

        if (ch == '|') {
            flush();
            if (!frames.empty()) {
                frames.back().has_alternation = true;
            } else {
                // A top level alternation makes every run collected so far
                // optional.
                collected.clear();
            }
            ++i;
            after_quantifier = false;
            continue;
        }

        if (ch == '*' || ch == '?' || ch == '+' || ch == '{') {
            if (after_quantifier && ch == '?') {
                ++i; // lazy modifier
                continue;
            }
            if (after_quantifier && ch == '+') {
                return bail(); // possessive quantifier
            }
            uint32_t min_repeat = 1;
            size_t next = i + 1;
            if (ch == '{') {
                const size_t end = RepetitionEnd(pattern, i, min_repeat);
                if (end == std::string_view::npos) {
                    // A literal brace.
                    run.push_back('{');
                    last_atom_len = 1;
                    run_open = true;
                    last_atom_is_group = false;
                    ++i;
                    after_quantifier = false;
                    continue;
                }
                next = end;
            }
            if (ch == '*' || ch == '?') {
                min_repeat = 0;
            }
            if (min_repeat == 0) {
                if (last_atom_is_group) {
                    // The group may be absent, so nothing it collected can be
                    // required: `(?:abc)?d` has to plan on `d` alone.
                    if (collected.size() > last_group_runs_begin) {
                        collected.resize(last_group_runs_begin);
                    }
                } else if (run_open && last_atom_len > 0) {
                    // The quantified atom may be absent, so it cannot be required.
                    run.resize(run.size() - std::min(last_atom_len, run.size()));
                }
            }
            // Either way the run ends here: further copies may follow, or the
            // atom may be missing, so the text after the quantifier is not
            // contiguous with what precedes it.
            flush();
            i = next;
            after_quantifier = true;
            continue;
        }

        if (ch == '.' || ch == '^' || ch == '$') {
            flush();
            ++i;
            after_quantifier = false;
            continue;
        }

        if (ch == '}' || ch == ']') {
            run.push_back(ch);
            last_atom_len = 1;
            run_open = true;
            last_atom_is_group = false;
            ++i;
            after_quantifier = false;
            continue;
        }

        // A literal code point. A quantifier applies to the whole code point, so
        // the run advances by the full UTF-8 sequence: `数据模型?` may only drop
        // `型`, and a lone trailing byte would be a run the index never saw.
        uint32_t width = 1;
        SparseGramDecodeUtf8(pattern.data() + i, static_cast<uint32_t>(pattern.size() - i), width);
        run.append(pattern.data() + i, width);
        last_atom_len = width;
        run_open = true;
        last_atom_is_group = false;
        i += width;
        after_quantifier = false;
    }

    if (!frames.empty()) {
        return bail(); // unbalanced
    }
    flush();

    for (auto &item : collected) {
        if (!item.text.empty()) {
            result.runs.push_back(std::move(item));
        }
    }
    return result;
}

} // namespace infinity
