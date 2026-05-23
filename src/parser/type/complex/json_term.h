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

#include <compare>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

// JsonTermT represents a flattened JSON term used for JSON index.
// Term format: {path}:{type_tag}:{encoded_value}
// The string is kept ordered (dictionary order = value order).
// Uses fixed-size storage for trivially copyable serialization.

constexpr size_t JSON_TERM_MAX_LENGTH = 512;

namespace infinity {

struct JsonTermT {
    uint32_t length_{0};
    char data_[JSON_TERM_MAX_LENGTH]{};

    JsonTermT() = default;

    explicit JsonTermT(const std::string &term) { Assign(term.data(), term.size()); }

    bool operator==(const JsonTermT &other) const {
        const auto this_len = std::min<size_t>(length_, JSON_TERM_MAX_LENGTH);
        const auto other_len = std::min<size_t>(other.length_, JSON_TERM_MAX_LENGTH);
        if (this_len != other_len)
            return false;
        return std::memcmp(data_, other.data_, this_len) == 0;
    }

    auto operator<=>(const JsonTermT &other) const {
        // Defensive: ensure valid lengths
        auto this_len = std::min<size_t>(length_, JSON_TERM_MAX_LENGTH);
        auto other_len = std::min<size_t>(other.length_, JSON_TERM_MAX_LENGTH);
        auto cmp = std::memcmp(data_, other.data_, std::min(this_len, other_len));
        if (cmp != 0)
            return cmp < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
        return this_len <=> other_len;
    }

    std::string ToString() const { return std::string(data_, std::min<size_t>(length_, JSON_TERM_MAX_LENGTH)); }

    void Reset() { length_ = 0; }

    void Append(const char *src, size_t len) {
        const auto current_len = std::min<size_t>(length_, JSON_TERM_MAX_LENGTH);
        if (current_len + len > JSON_TERM_MAX_LENGTH) {
            throw std::length_error("JsonTermT overflow: JSON index term exceeds JSON_TERM_MAX_LENGTH");
        }
        std::memcpy(data_ + current_len, src, len);
        length_ = static_cast<uint32_t>(current_len + len);
    }

    // Return a "max" term that sorts after all valid terms
    static JsonTermT Max() {
        JsonTermT result;
        result.length_ = JSON_TERM_MAX_LENGTH;
        memset(result.data_, 0xFF, JSON_TERM_MAX_LENGTH);
        return result;
    }

    // Return a "min" term that sorts before all valid terms
    static JsonTermT Min() {
        JsonTermT result;
        result.length_ = 0;
        memset(result.data_, 0, JSON_TERM_MAX_LENGTH);
        return result;
    }

private:
    void Assign(const char *src, size_t len) {
        if (len > JSON_TERM_MAX_LENGTH) {
            throw std::length_error("JsonTermT overflow: JSON index term exceeds JSON_TERM_MAX_LENGTH");
        }
        std::memcpy(data_, src, len);
        length_ = static_cast<uint32_t>(len);
    }
};

} // namespace infinity

// Specialize std::numeric_limits for JsonTermT
namespace std {

template <>
struct numeric_limits<infinity::JsonTermT> {
    static constexpr bool is_specialized = true;

    static infinity::JsonTermT min() { return infinity::JsonTermT::Min(); }

    static infinity::JsonTermT max() { return infinity::JsonTermT::Max(); }

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
