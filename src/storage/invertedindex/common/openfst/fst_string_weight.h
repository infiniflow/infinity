////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>

#include "fst_string_types.h"
#include <fst/string-weight.h>

namespace fst {

template <typename Label>
class FSTStringLeftWeight;

template <typename Label>
struct FSTStringLeftWeightTraits {
    inline static const FSTStringLeftWeight<Label> &Zero();

    inline static const FSTStringLeftWeight<Label> &One();

    inline static const FSTStringLeftWeight<Label> &NoWeight();

    inline static bool Member(const FSTStringLeftWeight<Label> &weight);
};

// String semiring: (longest_common_prefix/suffix, ., Infinity, Epsilon)
template <typename Label>
class FSTStringLeftWeight : public FSTStringLeftWeightTraits<Label> {
public:
    using ReverseWeight = FSTStringLeftWeight<Label>;
    using str_t = infinity::basic_string<Label>;
    using iterator = typename str_t::const_iterator;

    static const std::string &Type() {
        static const std::string type = "left_string";
        return type;
    }

    friend bool operator==(const FSTStringLeftWeight &lhs, const FSTStringLeftWeight &rhs) noexcept { return lhs.str_ == rhs.str_; }

    FSTStringLeftWeight() = default;

    explicit FSTStringLeftWeight(Label label) : str_(1, label) {}

    template <typename Iterator>
    FSTStringLeftWeight(Iterator begin, Iterator end) : str_(begin, end) {}

    FSTStringLeftWeight(const FSTStringLeftWeight &) = default;
    FSTStringLeftWeight(FSTStringLeftWeight &&) = default;

    explicit FSTStringLeftWeight(infinity::basic_string_view<Label> rhs) : str_(rhs.data(), rhs.size()) {}

    FSTStringLeftWeight &operator=(FSTStringLeftWeight &&) = default;
    FSTStringLeftWeight &operator=(const FSTStringLeftWeight &) = default;

    FSTStringLeftWeight &operator=(infinity::basic_string_view<Label> rhs) {
        str_.assign(rhs.data(), rhs.size());
        return *this;
    }

    bool Member() const noexcept { return FSTStringLeftWeightTraits<Label>::Member(*this); }

    std::istream &Read(std::istream &strm) {
        // TODO replace istream with byteslicereader
        std::uint32_t size = 0;
        {
            // auto it = irs::irstd::make_istream_iterator(strm);
            // size = irs::vread<uint32_t>(it);
        }

        // read content
        str_.resize(size);
        strm.read(reinterpret_cast<char *>(str_.data()), size * sizeof(Label));

        return strm;
    }

    std::ostream &Write(std::ostream &strm) const {
        // write size
        // use varlen encoding since weights are usually small
        // TODO replace ostream with byteslicewriter
        const std::uint32_t size = static_cast<uint32_t>(Size());
        {
            // auto it = irs::irstd::make_ostream_iterator(strm);
            // irs::vwrite(it, size);
        }

        // write content
        strm.write(reinterpret_cast<const char *>(str_.c_str()), size * sizeof(Label));

        return strm;
    }

    size_t Hash() const noexcept { return std::hash<str_t>()(str_); }

    FSTStringLeftWeight Quantize(float delta = kDelta) const noexcept { return *this; }

    ReverseWeight Reverse() const { return ReverseWeight(str_.rbegin(), str_.rend()); }

    static std::uint64_t Properties() noexcept {
        static constexpr auto props = kLeftSemiring | kIdempotent;
        return props;
    }

    Label &operator[](size_t i) noexcept { return str_[i]; }

    const Label &operator[](size_t i) const noexcept { return str_[i]; }

    const Label *c_str() const noexcept { return str_.c_str(); }

    void Resize(size_t size) noexcept { str_.resize(size); }

    bool Empty() const noexcept { return str_.empty(); }

    void Clear() noexcept { str_.clear(); }

    size_t Size() const noexcept { return str_.size(); }

    void PushBack(Label label) { str_.push_back(label); }

    template <typename Iterator>
    void PushBack(Iterator begin, Iterator end) {
        str_.append(begin, end);
    }

    void PushBack(const FSTStringLeftWeight &w) { PushBack(w.begin(), w.end()); }

    void Reserve(size_t capacity) { str_.reserve(capacity); }

    iterator begin() const noexcept { return str_.begin(); }
    iterator end() const noexcept { return str_.end(); }

    // intentionally implicit
    operator infinity::basic_string_view<Label>() const noexcept { return str_; }

    // intentionally implicit
    operator infinity::basic_string<Label>() && noexcept { return std::move(str_); }

private:
    str_t str_;
};

template <typename Label>
const FSTStringLeftWeight<Label> &FSTStringLeftWeightTraits<Label>::Zero() {
    static const FSTStringLeftWeight<Label> zero(static_cast<Label>(kStringInfinity)); // cast same as in FST
    return zero;
}

template <typename Label>
const FSTStringLeftWeight<Label> &FSTStringLeftWeightTraits<Label>::One() {
    static const FSTStringLeftWeight<Label> one;
    return one;
}

template <typename Label>
const FSTStringLeftWeight<Label> &FSTStringLeftWeightTraits<Label>::NoWeight() {
    static const FSTStringLeftWeight<Label> no_weight(static_cast<Label>(kStringBad)); // cast same as in FST
    return no_weight;
}

template <typename Label>
bool FSTStringLeftWeightTraits<Label>::Member(const FSTStringLeftWeight<Label> &weight) {
    return weight != NoWeight();
}

template <typename Label>
inline bool operator!=(const FSTStringLeftWeight<Label> &w1, const FSTStringLeftWeight<Label> &w2) {
    return !(w1 == w2);
}

template <typename Label>
inline std::ostream &operator<<(std::ostream &strm, const FSTStringLeftWeight<Label> &weight) {
    if (weight.Empty()) {
        return strm << "Epsilon";
    }

    auto begin = weight.begin();
    const auto &first = *begin;

    if (first == kStringInfinity) {
        return strm << "Infinity";
    } else if (first == kStringBad) {
        return strm << "BadString";
    }

    const auto end = weight.end();
    if (begin != end) {
        strm << *begin;

        for (++begin; begin != end; ++begin) {
            strm << kStringSeparator << *begin;
        }
    }

    return strm;
}

template <typename Label>
inline std::istream &operator>>(std::istream &strm, FSTStringLeftWeight<Label> &weight) {
    std::string str;
    strm >> str;
    if (str == "Infinity") {
        weight = FSTStringLeftWeight<Label>::Zero();
    } else if (str == "Epsilon") {
        weight = FSTStringLeftWeight<Label>::One();
    } else {
        weight.Clear();
        char *p = nullptr;
        for (const char *cs = str.c_str(); !p || *p != '\0'; cs = p + 1) {
            const Label label = strtoll(cs, &p, 10);
            if (p == cs || (*p != 0 && *p != kStringSeparator)) {
                strm.clear(std::ios::badbit);
                break;
            }
            weight.PushBack(label);
        }
    }
    return strm;
}

// Longest common prefix for left string semiring.
template <typename Label>
inline FSTStringLeftWeight<Label> Plus(const FSTStringLeftWeight<Label> &lhs, const FSTStringLeftWeight<Label> &rhs) {
    typedef FSTStringLeftWeight<Label> Weight;

    if (!lhs.Member() || !rhs.Member()) {
        return Weight::NoWeight();
    }

    if (lhs == Weight::Zero()) {
        return rhs;
    }

    if (rhs == Weight::Zero()) {
        return lhs;
    }

    const auto *plhs = &lhs;
    const auto *prhs = &rhs;

    if (rhs.Size() > lhs.Size()) {
        // enusre that 'prhs' is shorter than 'plhs'
        // The behavior is undefined if the second range is shorter than the first
        // range. (http://en.cppreference.com/w/cpp/algorithm/mismatch)
        std::swap(plhs, prhs);
    }

    assert(prhs->Size() <= plhs->Size());

    return Weight(prhs->begin(), std::mismatch(prhs->begin(), prhs->end(), plhs->begin()).first);
}

template <typename Label>
inline FSTStringLeftWeight<Label> Times(const FSTStringLeftWeight<Label> &lhs, const FSTStringLeftWeight<Label> &rhs) {
    typedef FSTStringLeftWeight<Label> Weight;

    if (!lhs.Member() || !rhs.Member()) {
        return Weight::NoWeight();
    }

    if (lhs == Weight::Zero() || rhs == Weight::Zero()) {
        return Weight::Zero();
    }

    Weight product;
    product.Reserve(lhs.Size() + rhs.Size());
    product.PushBack(lhs.begin(), lhs.end());
    product.PushBack(rhs.begin(), rhs.end());
    return product;
}

// Left division in a left string semiring.
template <typename Label>
inline FSTStringLeftWeight<Label> DivideLeft(const FSTStringLeftWeight<Label> &lhs, const FSTStringLeftWeight<Label> &rhs) {
    typedef FSTStringLeftWeight<Label> Weight;

    if (!lhs.Member() || !rhs.Member()) {
        return Weight::NoWeight();
    }

    if (rhs == Weight::Zero()) {
        return Weight::NoWeight();
    }
    if (lhs == Weight::Zero()) {
        return Weight::Zero();
    }

    if (rhs.Size() > lhs.Size()) {
        return Weight();
    }

    assert(::infinity::basic_string_view<Label>(lhs).starts_with(rhs));

    return Weight(lhs.begin() + rhs.Size(), lhs.end());
}

template <typename Label>
inline FSTStringLeftWeight<Label> Divide(const FSTStringLeftWeight<Label> &lhs, const FSTStringLeftWeight<Label> &rhs, DivideType typ) {
    return DivideLeft(lhs, rhs);
}

template <>
struct FSTStringLeftWeightTraits<infinity::byte_type> {
    static const FSTStringLeftWeight<infinity::byte_type> &Zero() noexcept {
        static const FSTStringLeftWeight<infinity::byte_type> zero;
        return zero;
    }

    static const FSTStringLeftWeight<infinity::byte_type> &One() noexcept { return Zero(); }

    static const FSTStringLeftWeight<infinity::byte_type> &NoWeight() noexcept { return Zero(); }

    static bool Member(const FSTStringLeftWeight<infinity::byte_type> &) noexcept {
        // always member
        return true;
    }
};

inline std::ostream &operator<<(std::ostream &strm, const FSTStringLeftWeight<infinity::byte_type> &weight) {
    if (weight.Empty()) {
        return strm << "Epsilon";
    }

    auto begin = weight.begin();

    const auto end = weight.end();
    if (begin != end) {
        strm << *begin;

        for (++begin; begin != end; ++begin) {
            strm << kStringSeparator << *begin;
        }
    }

    return strm;
}

inline std::istream &operator>>(std::istream &strm, FSTStringLeftWeight<infinity::byte_type> &weight) {
    std::string str;
    strm >> str;
    if (str == "Epsilon") {
        weight = FSTStringLeftWeight<infinity::byte_type>::One();
    } else {
        weight.Clear();
        char *p = nullptr;
        for (const char *cs = str.c_str(); !p || *p != '\0'; cs = p + 1) {
            const infinity::byte_type label = strtoll(cs, &p, 10);
            if (p == cs || (*p != 0 && *p != kStringSeparator)) {
                strm.clear(std::ios::badbit);
                break;
            }
            weight.PushBack(label);
        }
    }
    return strm;
}

// Longest common prefix for left string semiring.
// For binary strings that's impossible to use
// Zero() or NoWeight() as they may interfere
// with real values
inline infinity::bytes_view PlusImpl(infinity::bytes_view lhs, infinity::bytes_view rhs) {
    const auto *plhs = &lhs;
    const auto *prhs = &rhs;

    if (rhs.size() > lhs.size()) {
        // enusre that 'prhs' is shorter than 'plhs'
        // The behavior is undefined if the second range is shorter than the first
        // range. (http://en.cppreference.com/w/cpp/algorithm/mismatch)
        std::swap(plhs, prhs);
    }

    auto pair = std::mismatch(prhs->data(), prhs->data() + prhs->size(), plhs->data());
    return {prhs->data(), static_cast<size_t>(pair.first - prhs->data())};
}

inline infinity::bytes_view Plus(const FSTStringLeftWeight<infinity::byte_type> &lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs) {
    return PlusImpl(lhs, rhs);
}

inline infinity::bytes_view Plus(infinity::bytes_view lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs) { return PlusImpl(lhs, rhs); }

inline infinity::bytes_view Plus(const FSTStringLeftWeight<infinity::byte_type> &lhs, infinity::bytes_view rhs) { return PlusImpl(lhs, rhs); }

// For binary strings that's impossible to use
// Zero() or NoWeight() as they may interfere
// with real values
inline FSTStringLeftWeight<infinity::byte_type> TimesImpl(infinity::bytes_view lhs, infinity::bytes_view rhs) {
    using Weight = FSTStringLeftWeight<infinity::byte_type>;

    Weight product;
    product.Reserve(lhs.size() + rhs.size());
    product.PushBack(lhs.begin(), lhs.end());
    product.PushBack(rhs.begin(), rhs.end());
    return product;
}

inline FSTStringLeftWeight<infinity::byte_type> Times(const FSTStringLeftWeight<infinity::byte_type> &lhs,
                                                   const FSTStringLeftWeight<infinity::byte_type> &rhs) {
    return TimesImpl(lhs, rhs);
}

inline FSTStringLeftWeight<infinity::byte_type> Times(infinity::bytes_view lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs) {
    return TimesImpl(lhs, rhs);
}

inline FSTStringLeftWeight<infinity::byte_type> Times(const FSTStringLeftWeight<infinity::byte_type> &lhs, infinity::bytes_view rhs) {
    return TimesImpl(lhs, rhs);
}

// Left division in a left string semiring.
// For binary strings that's impossible to use
// Zero() or NoWeight() as they may interfere
// with real values
inline infinity::bytes_view DivideLeftImpl(infinity::bytes_view lhs, infinity::bytes_view rhs) {
    if (rhs.size() > lhs.size()) {
        return {};
    }
    return {lhs.data() + rhs.size(), lhs.size() - rhs.size()};
}

inline infinity::bytes_view DivideLeft(const FSTStringLeftWeight<infinity::byte_type> &lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs) {
    return DivideLeftImpl(lhs, rhs);
}

inline infinity::bytes_view DivideLeft(infinity::bytes_view lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs) {
    return DivideLeftImpl(lhs, rhs);
}

inline infinity::bytes_view DivideLeft(const FSTStringLeftWeight<infinity::byte_type> &lhs, infinity::bytes_view rhs) {
    return DivideLeftImpl(lhs, rhs);
}

inline infinity::bytes_view DivideLeft(infinity::bytes_view lhs, infinity::bytes_view rhs) { return DivideLeftImpl(lhs, rhs); }

inline FSTStringLeftWeight<infinity::byte_type>
Divide(const FSTStringLeftWeight<infinity::byte_type> &lhs, const FSTStringLeftWeight<infinity::byte_type> &rhs, DivideType typ) {
    return FSTStringLeftWeight<infinity::byte_type>(DivideLeft(lhs, rhs));
}

} // namespace fst
