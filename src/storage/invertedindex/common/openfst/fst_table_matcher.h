////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
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
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>

#include "fst_automaton.h"
#include "fst_bits_utils.h"
#include "fst_string_types.h"

namespace fst {

template <typename F,             // automaton
          size_t CacheSize = 256, // size of a table for cached label offsets
          bool MatchInput = true, // label to match
          bool ByteLabel = false  // byte automaton is defined over alphabet {0..256, Rho}
          >
class TableMatcher final : public MatcherBase<typename F::Arc> {
public:
    using FST = F;
    using Arc = typename FST::Arc;
    using Label = typename Arc::Label;
    using StateId = typename Arc::StateId;
    using Weight = typename Arc::Weight;

    using MatcherBase<Arc>::Flags;
    using MatcherBase<Arc>::Properties;

    static constexpr MatchType MATCH_TYPE = MatchInput ? MATCH_INPUT : MATCH_OUTPUT;

    // expected FST properties
    static constexpr auto FST_PROPERTIES =
        (MATCH_TYPE == MATCH_INPUT ? kILabelSorted : kOLabelSorted) | (MATCH_TYPE == MATCH_INPUT ? kIDeterministic : kODeterministic) | kAcceptor;

    explicit TableMatcher(const FST &fst, bool test_props)
        : start_labels_(this->GetStartLabels(fst)), num_labels_(start_labels_.size()), transitions_(fst.NumStates() * num_labels_, kNoStateId),
          arc_(kNoLabel, kNoLabel, Weight::NoWeight(), kNoStateId), fst_(&fst),
          error_(test_props && (fst.Properties(FST_PROPERTIES, true) != FST_PROPERTIES)) {

        if (error_) {
            return;
        }

        // initialize transition table
        ArcIteratorData<Arc> data;
        for (StateIterator<FST> siter(fst); !siter.Done(); siter.Next()) {
            const auto state = siter.Value();

            fst.InitArcIterator(state, &data);

            if (!data.narcs) {
                if (!fst.Final(state)) {
                    sink_ = state;
                }
                continue;
            }

            // fill existing transitions
            auto arc = data.arcs;
            auto arc_end = data.arcs + data.narcs;
            auto label = start_labels_.begin();
            auto *state_transitions = transitions_.data() + state * num_labels_;

            for (; arc != arc_end && label != start_labels_.end(); ++arc) {
                const RangeLabel range{GetLabel(*arc)};

                label = std::find(label, start_labels_.end(), range.min);
                assert(label != start_labels_.end());

                auto *label_transitions = state_transitions + std::distance(start_labels_.begin(), label);
                for (; label != start_labels_.end() && range.max >= *label; ++label) {
                    *label_transitions++ = arc->nextstate;
                }
            }
        }

        // initialize lookup table for first CacheSize labels,
        // code below is the optimized version of:
        // for (size_t i = 0; i < CacheSize; ++i) {
        //   cached_label_offsets_[i] = FindLabelOffset(i);
        // }
        auto begin = start_labels_.begin() + 1;
        auto end = start_labels_.end();
        size_t i = 0;
        size_t offset = 0;
        for (; i < std::size(cached_label_offsets_); ++i) {
            if (begin != end && size_t(*begin) == i) {
                ++offset;
                ++begin;
            }
            cached_label_offsets_[i] = offset;
        }
        std::fill(cached_label_offsets_ + i, std::end(cached_label_offsets_), offset);
        transitions_begin_ = transitions_.data();
    }

    TableMatcher *Copy(bool) const final { return new TableMatcher(*this); }

    MatchType Type(bool test) const final {
        if constexpr (MATCH_TYPE == MATCH_NONE) {
            return MATCH_TYPE;
        }

        constexpr const auto true_prop = (MATCH_TYPE == MATCH_INPUT) ? kILabelSorted : kOLabelSorted;

        constexpr const auto false_prop = (MATCH_TYPE == MATCH_INPUT) ? kNotILabelSorted : kNotOLabelSorted;

        const auto props = fst_->Properties(true_prop | false_prop, test);

        if (props & true_prop) {
            return MATCH_TYPE;
        } else if (props & false_prop) {
            return MATCH_NONE;
        } else {
            return MATCH_UNKNOWN;
        }
    }

    StateId Peek(StateId s, Label label) noexcept {
        size_t label_offset;
        if constexpr (ByteLabel && CacheSize > std::numeric_limits<infinity::byte_type>::max()) {
            label_offset = cached_label_offsets_[size_t(label)];
        } else {
            label_offset = (size_t(label) < std::size(cached_label_offsets_) ? cached_label_offsets_[size_t(label)] : FindLabelOffset(label));
        }

        assert(label_offset < num_labels_);
        return (transitions_begin_ + s * num_labels_)[label_offset];
    }

    void SetState(StateId s) noexcept final {
        assert(s * num_labels_ < transitions_.size());
        state_begin_ = transitions_begin_ + s * num_labels_;
        state_ = state_begin_;
        state_end_ = state_begin_ + num_labels_;
    }

    bool Find(Label label) noexcept final {
        size_t label_offset;
        if constexpr (ByteLabel && CacheSize > std::numeric_limits<infinity::byte_type>::max()) {
            label_offset = cached_label_offsets_[size_t(label)];
        } else {
            label_offset = (size_t(label) < std::size(cached_label_offsets_) ? cached_label_offsets_[size_t(label)] : FindLabelOffset(label));
        }

        state_ = state_begin_ + label_offset;
        assert(state_ < state_end_);

        arc_.nextstate = *state_;
        return arc_.nextstate != kNoStateId;
    }

    bool Done() const noexcept final { return state_ == state_end_; }

    const Arc &Value() const noexcept final { return arc_; }

    void Next() noexcept final {
        if (Done()) {
            return;
        }

        ++state_;

        for (; !Done(); ++state_) {
            if (*state_ != kNoLabel) {
                assert(state_ > state_begin_ && state_ < state_end_);
                const auto label = start_labels_[size_t(std::distance(state_begin_, state_))];
                if constexpr (MATCH_TYPE == MATCH_INPUT) {
                    arc_.ilabel = label;
                } else {
                    arc_.olabel = label;
                }
                arc_.nextstate = *state_;
                return;
            }
        }
    }

    Weight Final(StateId s) const final { return MatcherBase<Arc>::Final(s); }

    ssize_t Priority(StateId s) final { return MatcherBase<Arc>::Priority(s); }

    const FST &GetFst() const noexcept final { return *fst_; }

    std::uint64_t Properties(std::uint64_t inprops) const noexcept final { return inprops | (error_ ? kError : 0); }

    StateId Sink() const noexcept { return sink_; }

private:
    template <typename Arc>
    static typename Arc::Label GetLabel(Arc &arc) {
        if constexpr (MATCH_TYPE == MATCH_INPUT) {
            return arc.ilabel;
        }

        return arc.olabel;
    }

    size_t FindLabelOffset(Label label) const noexcept {
        const auto it = std::lower_bound(start_labels_.rbegin(), start_labels_.rend(), label, std::greater<>());
        return size_t(std::distance(start_labels_.begin(), it.base())) - 1;
    }

    //    template <typename F, bool MatchInput, bool ByteLabel>
    std::vector<typename F::Arc::Label> GetStartLabels(const F &fst) {
        using Label = typename F::Arc::Label;

        if constexpr (ByteLabel) {
            size_t bits[256 / BitsRequired<size_t>()]{};

            for (StateIterator<F> siter(fst); !siter.Done(); siter.Next()) {
                const auto state = siter.Value();
                for (ArcIterator<F> aiter(fst, state); !aiter.Done(); aiter.Next()) {
                    const auto &arc = aiter.Value();
                    RangeLabel range{MatchInput ? arc.ilabel : arc.olabel};
                    range.max += decltype(range.max)(range.max < std::numeric_limits<char>::max());

                    SetBit(bits[range.min / BitsRequired<size_t>()], range.min % BitsRequired<size_t>());
                    SetBit(bits[range.max / BitsRequired<size_t>()], range.max % BitsRequired<size_t>());
                }
            }

            const size_t size = std::popcount(bits[0]) + std::popcount(bits[1]) + std::popcount(bits[2]) + std::popcount(bits[3]);

            std::vector<Label> labels(1 + size);
            auto begin = labels.begin();
            *begin = 0;
            ++begin;

            Label offset = 0;

            std::for_each(std::begin(bits), std::end(bits), [this, &offset, &begin](size_t word) {
                for (size_t j = 0; j < BitsRequired<size_t>(); ++j) {
                    if (CheckBit(word, j)) {
                        *begin = offset + static_cast<Label>(j);
                        ++begin;
                    }
                }
                offset += BitsRequired<size_t>();
            });

            return labels;
        } else {
            std::set<Label> labels{0};

            for (StateIterator<F> siter(fst); !siter.Done(); siter.Next()) {
                const auto state = siter.Value();
                for (ArcIterator<F> aiter(fst, state); !aiter.Done(); aiter.Next()) {
                    const auto &arc = aiter.Value();
                    RangeLabel range{MatchInput ? arc.ilabel : arc.olabel};
                    range.max += decltype(range.max)(range.max < std::numeric_limits<uint32_t>::max());

                    labels.emplace(range.min);
                    labels.emplace(range.max);
                }
            }
            return {labels.begin(), labels.end()};
        }
    }

    size_t cached_label_offsets_[CacheSize]{};
    std::vector<Label> start_labels_;
    size_t num_labels_;
    std::vector<StateId> transitions_;
    Arc arc_;
    StateId sink_{fst::kNoStateId}; // sink state
    const FST *fst_;                // FST for matching
    const StateId *transitions_begin_;
    const StateId *state_begin_{}; // Matcher state begin
    const StateId *state_end_{};   // Matcher state end
    const StateId *state_{};       // Matcher current state
    bool error_;                   // Matcher validity
};

} // namespace fst
