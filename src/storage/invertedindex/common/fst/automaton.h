#pragma once

#include <type_traits>
#include <vector>
// clang-format off
#include <fst/fst.h>
// clang-format on
#include <fst/connect.h>
#include <fst/test-properties.h>

#include "automaton_decl.h"
#include "fst_utils.h"

namespace fst {
class BooleanWeight {
public:
    using ReverseWeight = BooleanWeight;
    using PayloadType = uint8_t;

    static const std::string &Type() {
        static const std::string kType = "boolean";
        return kType;
    }

    static constexpr BooleanWeight Zero() noexcept { return false; }
    static constexpr BooleanWeight One() noexcept { return true; }
    static constexpr BooleanWeight NoWeight() noexcept { return {}; }

    static constexpr std::uint64_t Properties() noexcept { return kLeftSemiring | kRightSemiring | kCommutative | kIdempotent | kPath; }

    constexpr BooleanWeight() noexcept = default;
    // cppcheck-suppress noExplicitConstructor
    constexpr BooleanWeight(bool v, PayloadType payload = 0) noexcept : v_(static_cast<PayloadType>(v)), p_(payload) {}

    constexpr bool Member() const noexcept { return kInvalid != v_; }
    constexpr BooleanWeight Quantize([[maybe_unused]] float delta = kDelta) const noexcept { return {}; }
    std::istream &Read(std::istream &strm) noexcept {
        v_ = strm.get();
        if (strm.fail()) {
            v_ = kInvalid;
        }
        return strm;
    }
    std::ostream &Write(std::ostream &strm) const noexcept {
        strm.put(v_);
        return strm;
    }
    constexpr size_t Hash() const noexcept { return static_cast<size_t>(v_); }
    constexpr ReverseWeight Reverse() const noexcept { return *this; }
    constexpr PayloadType Payload() const noexcept { return p_; }
    constexpr operator bool() const noexcept { return v_ == kTrue; }

    friend constexpr bool operator==(const BooleanWeight &lhs, const BooleanWeight &rhs) noexcept { return lhs.Hash() == rhs.Hash(); }
    friend constexpr bool operator!=(const BooleanWeight &lhs, const BooleanWeight &rhs) noexcept { return !(lhs == rhs); }

    // Note: | and & used instead of || and && because gcc cannot optimize it

    friend constexpr BooleanWeight Plus(const BooleanWeight &lhs, const BooleanWeight &rhs) noexcept {
        return {static_cast<bool>(static_cast<unsigned>(static_cast<bool>(lhs.Hash())) | static_cast<unsigned>(static_cast<bool>(rhs.Hash()))),
                static_cast<PayloadType>(lhs.Payload() | rhs.Payload())};
    }
    friend constexpr BooleanWeight Times(const BooleanWeight &lhs, const BooleanWeight &rhs) noexcept {
        return {static_cast<bool>(static_cast<unsigned>(static_cast<bool>(lhs.Hash())) & static_cast<unsigned>(static_cast<bool>(rhs.Hash()))),
                static_cast<PayloadType>(lhs.Payload() & rhs.Payload())};
    }
    friend constexpr BooleanWeight
    Divide([[maybe_unused]] const BooleanWeight &lhs, [[maybe_unused]] const BooleanWeight &rhs, [[maybe_unused]] DivideType type) noexcept {
        return NoWeight();
    }
    friend constexpr BooleanWeight Divide([[maybe_unused]] const BooleanWeight &lhs, [[maybe_unused]] const BooleanWeight &rhs) noexcept {
        return NoWeight();
    }
    friend constexpr BooleanWeight DivideLeft([[maybe_unused]] const BooleanWeight &lhs, [[maybe_unused]] const BooleanWeight &rhs) noexcept {
        return NoWeight();
    }

    friend std::ostream &operator<<(std::ostream &strm, const BooleanWeight &w) {
        if (w.Member()) {
            strm << "{" << static_cast<char>(static_cast<int>(static_cast<bool>(w)) + 48) << "," << static_cast<int>(w.Payload()) << "}";
        }
        return strm;
    }
    friend constexpr bool ApproxEqual(const BooleanWeight &lhs, const BooleanWeight &rhs, [[maybe_unused]] float delta = kDelta) {
        return lhs == rhs;
    }

private:
    static constexpr PayloadType kFalse = 0;
    static constexpr PayloadType kTrue = 1;    // "is true" mask
    static constexpr PayloadType kInvalid = 2; // "not a member" value

    // TODO(MBkkt) can be bool?
    // TODO(MBkkt) try to make it more signaling
    PayloadType v_{kInvalid};
    PayloadType p_{};
};

struct RangeLabelLE {
    constexpr RangeLabelLE(int64_t ilabel) noexcept : ilabel{ilabel} {}
    constexpr RangeLabelLE(uint32_t min, uint32_t max) noexcept : max{max}, min{min} {}

    union {
        int64_t ilabel;
        struct {
            uint32_t max;
            uint32_t min;
        };
    };
};

struct RangeLabelBE {
    constexpr RangeLabelBE(int64_t ilabel) noexcept : ilabel{ilabel} {}
    constexpr RangeLabelBE(uint32_t min, uint32_t max) noexcept : min{min}, max{max} {}

    union {
        int64_t ilabel;
        struct {
            uint32_t min;
            uint32_t max;
        };
    };
};

consteval bool IsBigEndian() noexcept { return std::endian::native == std::endian::big; }

using RangeLabelType = std::conditional_t<IsBigEndian(), RangeLabelBE, RangeLabelLE>;

// We inherit from annonymous union to be OpenFST compliant.
struct RangeLabel : RangeLabelType {
    static constexpr RangeLabel fromRange(uint32_t min, uint32_t max) noexcept { return RangeLabel{min, max}; }
    static constexpr RangeLabel fromRange(uint32_t min) noexcept { return fromRange(min, min); }
    static constexpr RangeLabel fromLabel(int64_t label) noexcept { return RangeLabel{label}; }

    constexpr RangeLabel() noexcept : RangeLabel{kNoLabel} {}

    constexpr RangeLabel(uint32_t min, uint32_t max) noexcept : RangeLabelType{min, max} {}

    constexpr explicit RangeLabel(int64_t ilabel) noexcept : RangeLabelType{ilabel} {}

    // TODO(MBkkt) should use std::bit_cast
    constexpr operator int64_t() const noexcept { return ilabel; }

    friend std::ostream &operator<<(std::ostream &strm, const RangeLabel &l) {
        strm << '[' << l.min << ".." << l.max << ']';
        return strm;
    }
};

template <typename W = BooleanWeight>
struct Transition : RangeLabel {
    using Weight = W;
    using Label = int64_t;
    using StateId = int32_t;

    static const std::string &Type() {
        static const std::string kType = "Transition";
        return kType;
    }

    union {
        StateId nextstate{kNoStateId};
        EmptyLabel<Label> olabel;
        EmptyWeight<Weight> weight; // all arcs are trivial
    };

    constexpr Transition() = default;

    constexpr Transition(RangeLabel ilabel, StateId nextstate) : RangeLabel{ilabel}, nextstate(nextstate) {}

    constexpr Transition(Label ilabel, StateId nextstate) : RangeLabel{ilabel}, nextstate{nextstate} {}

    // satisfy openfst API
    constexpr Transition(Label ilabel, Label /*unused*/, Weight /*unused*/, StateId nextstate) : RangeLabel{ilabel}, nextstate{nextstate} {}

    // satisfy openfst API
    constexpr Transition(Label ilabel, Label /*unused*/, StateId nextstate) : RangeLabel{ilabel}, nextstate{nextstate} {}
};

template <typename W>
std::uint64_t ComputeProperties(const Fst<Transition<W>> &fst, std::uint64_t mask, std::uint64_t *known) {
    using Arc = Transition<W>;
    using Label = typename Arc::Label;
    using StateId = typename Arc::StateId;
    using Weight = typename Arc::Weight;
    const auto fst_props = fst.Properties(kFstProperties, false); // FST-stored.
    // Computes (trinary) properties explicitly.
    // Initialize with binary properties (already known).
    std::uint64_t comp_props = fst_props & kBinaryProperties;
    // Computes these trinary properties with a DFS. We compute only those that
    // need a DFS here, since we otherwise would like to avoid a DFS since its
    // stack could grow large.
    constexpr std::uint64_t kDfsProps =
        kCyclic | kAcyclic | kInitialCyclic | kInitialAcyclic | kAccessible | kNotAccessible | kCoAccessible | kNotCoAccessible;
    std::vector<StateId> scc;
    if (mask & (kDfsProps | kWeightedCycles | kUnweightedCycles)) {
        SccVisitor<Arc> scc_visitor(&scc, nullptr, nullptr, &comp_props);
        DfsVisit(fst, &scc_visitor);
    }
    // Computes any remaining trinary properties via a state and arcs iterations
    if (mask & ~(kBinaryProperties | kDfsProps)) {
        comp_props |= kAcceptor | kNoEpsilons | kNoIEpsilons | kNoOEpsilons | kILabelSorted | kOLabelSorted | kUnweighted | kTopSorted | kString;
        if (mask & (kIDeterministic | kNonIDeterministic)) {
            comp_props |= kIDeterministic;
        }
        if (mask & (kODeterministic | kNonODeterministic)) {
            comp_props |= kODeterministic;
        }
        if (mask & (kDfsProps | kWeightedCycles | kUnweightedCycles)) {
            comp_props |= kUnweightedCycles;
        }

        struct RangeLabelComparer {
            bool operator()(Label lhs, Label rhs) const noexcept {
                RangeLabel lhs_range{lhs};
                RangeLabel rhs_range{rhs};

                return lhs_range.min < rhs_range.min && lhs_range.max < rhs_range.min;
            }
        };

        using Labels = std::set<Label, RangeLabelComparer>;

        std::unique_ptr<Labels> ilabels;
        std::unique_ptr<Labels> olabels;
        StateId nfinal = 0;
        for (StateIterator<Fst<Arc>> siter(fst); !siter.Done(); siter.Next()) {
            StateId s = siter.Value();
            Arc prev_arc;
            // Creates these only if we need to.
            if (mask & (kIDeterministic | kNonIDeterministic)) {
                ilabels = std::make_unique<Labels>();
            }
            if (mask & (kODeterministic | kNonODeterministic)) {
                olabels = std::make_unique<Labels>();
            }
            bool first_arc = true;
            for (ArcIterator<Fst<Arc>> aiter(fst, s); !aiter.Done(); aiter.Next()) {
                const auto &arc = aiter.Value();
                if (ilabels && ilabels->find(arc.ilabel) != ilabels->end()) {
                    comp_props |= kNonIDeterministic;
                    comp_props &= ~kIDeterministic;
                }
                if (olabels && olabels->find(arc.olabel) != olabels->end()) {
                    comp_props |= kNonODeterministic;
                    comp_props &= ~kODeterministic;
                }
                if (arc.ilabel != arc.olabel) {
                    comp_props |= kNotAcceptor;
                    comp_props &= ~kAcceptor;
                }
                if (arc.ilabel == 0 && arc.olabel == 0) {
                    comp_props |= kEpsilons;
                    comp_props &= ~kNoEpsilons;
                }
                if (arc.ilabel == 0) {
                    comp_props |= kIEpsilons;
                    comp_props &= ~kNoIEpsilons;
                }
                if (arc.olabel == 0) {
                    comp_props |= kOEpsilons;
                    comp_props &= ~kNoOEpsilons;
                }
                if (!first_arc) {
                    if (arc.ilabel < prev_arc.ilabel) {
                        comp_props |= kNotILabelSorted;
                        comp_props &= ~kILabelSorted;
                    }
                    if (arc.olabel < prev_arc.olabel) {
                        comp_props |= kNotOLabelSorted;
                        comp_props &= ~kOLabelSorted;
                    }
                }
                if (arc.weight != Weight::One() && arc.weight != Weight::Zero()) {
                    comp_props |= kWeighted;
                    comp_props &= ~kUnweighted;
                    if ((comp_props & kUnweightedCycles) && scc[s] == scc[arc.nextstate]) {
                        comp_props |= kWeightedCycles;
                        comp_props &= ~kUnweightedCycles;
                    }
                }
                if (arc.nextstate <= s) {
                    comp_props |= kNotTopSorted;
                    comp_props &= ~kTopSorted;
                }
                if (arc.nextstate != s + 1) {
                    comp_props |= kNotString;
                    comp_props &= ~kString;
                }
                prev_arc = arc;
                first_arc = false;
                if (ilabels) {
                    ilabels->insert(arc.ilabel);
                }
                if (olabels) {
                    olabels->insert(arc.olabel);
                }
            }

            if (nfinal > 0) { // Final state not last.
                comp_props |= kNotString;
                comp_props &= ~kString;
            }
            const auto final_weight = fst.Final(s);
            if (final_weight != Weight::Zero()) { // Final state.
                if (final_weight != Weight::One()) {
                    comp_props |= kWeighted;
                    comp_props &= ~kUnweighted;
                }
                ++nfinal;
            } else { // Non-final state.
                if (fst.NumArcs(s) != 1) {
                    comp_props |= kNotString;
                    comp_props &= ~kString;
                }
            }
        }
        if (fst.Start() != kNoStateId && fst.Start() != 0) {
            comp_props |= kNotString;
            comp_props &= ~kString;
        }
    }
    if (known) {
        *known = internal::KnownProperties(comp_props);
    }
    return comp_props;
}

} // namespace fst

// clang-format off
#include <fst/vector-fst.h>
// clang-format on
#include <fst/matcher.h>
