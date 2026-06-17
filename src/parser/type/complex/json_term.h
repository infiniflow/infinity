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

#include <string>
#include <limits>

namespace infinity {

// JsonTermT is now just an alias for std::string.
// It represents a flattened JSON term used for JSON index.
// Term format: {path}:{type_tag}:{encoded_value}
// The string is kept ordered (dictionary order = value order).
using JsonTermT = std::string;

// Return a "min" term that sorts before all valid terms.
inline JsonTermT JsonTermMin() { return ""; }

// Return a "max" term that sorts after all valid terms.
// We use a single 0xFF byte, which is greater than any valid JSON term
// (valid terms are ASCII / UTF‑8, where bytes < 0xFF).
inline JsonTermT JsonTermMax() { return std::string(1, '\xFF'); }

} // namespace infinity

// Specialize std::numeric_limits for JsonTermT (as std::string)
namespace std {

template <>
struct numeric_limits<infinity::JsonTermT> {
    static constexpr bool is_specialized = true;

    static infinity::JsonTermT min() { return infinity::JsonTermMin(); }

    static infinity::JsonTermT max() { return infinity::JsonTermMax(); }

    static infinity::JsonTermT lowest() { return min(); }

    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
};

} // namespace std
