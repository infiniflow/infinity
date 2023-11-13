#pragma once

#include <fst/fst.h>

namespace fst {

template <typename Label>
struct EmptyLabel {
    constexpr EmptyLabel() noexcept = default;
    constexpr EmptyLabel &operator=(Label) noexcept { return *this; }
    constexpr bool operator==(EmptyLabel) const noexcept { return true; }
    constexpr bool operator!=(EmptyLabel) const noexcept { return false; }
    constexpr bool operator==(Label) const noexcept { return true; }
    constexpr bool operator!=(Label) const noexcept { return false; }
    constexpr bool operator<(EmptyLabel) const noexcept { return false; }
    constexpr bool operator>(EmptyLabel) const noexcept { return false; }
    constexpr operator Label() const noexcept { return kNoLabel; }
    constexpr operator Label() noexcept { return kNoLabel; }
    constexpr void Write(std::ostream &) const noexcept {}

    friend constexpr bool operator==(Label, EmptyLabel) noexcept { return true; }
    friend constexpr bool operator!=(Label, EmptyLabel) noexcept { return false; }
    friend constexpr std::ostream &operator<<(std::ostream &strm, EmptyLabel) noexcept { return strm; }
};

template <typename Label, typename T>
constexpr bool operator==(EmptyLabel<Label>, const T &) noexcept {
    return true;
}
template <typename Label, typename T>
constexpr bool operator!=(EmptyLabel<Label>, const T &) noexcept {
    return false;
}

template <typename Weight>
struct EmptyWeight {
    using ReverseWeight = EmptyWeight;

    constexpr EmptyWeight &operator=(Weight) noexcept { return *this; }

    constexpr ReverseWeight Reverse() const noexcept { return *this; }
    constexpr EmptyWeight Quantize([[maybe_unused]] float delta = kDelta) const noexcept { return {}; }
    constexpr operator Weight() const noexcept { return Weight::One(); }
    constexpr operator Weight() noexcept { return Weight::One(); }
    constexpr bool operator==(EmptyWeight) const noexcept { return true; }
    constexpr bool operator!=(EmptyWeight) const noexcept { return false; }

    std::ostream &Write(std::ostream &strm) const {
        Weight::One().Write(strm);
        return strm;
    }

    std::istream &Read(std::istream &strm) {
        Weight().Read(strm);
        return strm;
    }
};

template <typename W, typename L = int32_t>
struct ILabelArc {
    using Weight = W;
    using Label = L;
    using StateId = int32_t;

    static const std::string &Type() {
        static const std::string type("ILabelArc");
        return type;
    }

    Label ilabel{fst::kNoLabel};
    StateId nextstate{fst::kNoStateId};
    union {
        Weight weight{};
        EmptyLabel<Label> olabel;
    };

    constexpr ILabelArc() = default;

    constexpr ILabelArc(Label ilabel, StateId nextstate) : ilabel(ilabel), nextstate(nextstate) {}

    // satisfy openfst API
    constexpr ILabelArc(Label ilabel, Label, Weight, StateId nextstate) : ilabel(ilabel), nextstate(nextstate) {}

    // satisfy openfst API
    constexpr ILabelArc(Label ilabel, Label, StateId nextstate) : ilabel(ilabel), nextstate(nextstate) {}
};

} // namespace fst

namespace std {

template <typename L>
inline void swap(typename ::fst::EmptyLabel<L> & /*lhs*/, L &rhs) noexcept {
    rhs = ::fst::kNoLabel;
}

template <typename L>
inline void swap(L &lhs, typename ::fst::EmptyLabel<L> & /*rhs*/) noexcept {
    lhs = ::fst::kNoLabel;
}

template <typename L>
struct hash<typename ::fst::EmptyLabel<L>> {
    constexpr size_t operator()(typename ::fst::EmptyLabel<L>) const noexcept { return 0; }
};

} // namespace std
