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

#include <fst/vector-fst.h>

#include "fst_states_map.h"
#include "fst_string_types.h"
#include "fst_string_weight.h"

#include <functional>
#include <string>

namespace infinity {

struct FSTStats {
    size_t num_states{}; // total number of states
    size_t num_arcs{};   // total number of arcs

    template <typename Weight>
    void operator()(const Weight &) noexcept {}
};

//////////////////////////////////////////////////////////////////////////////
/// @class FSTBuilder
/// @brief helper class for building minimal acyclic subsequential transducers
///        algorithm is described there:
///        http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.24.3698
//////////////////////////////////////////////////////////////////////////////
template <typename Char, typename Fst, typename Stats = FSTStats>
class FSTBuilder {
public:
    typedef Fst fst_t;
    typedef Char char_t;
    typedef basic_string_view<char_t> key_t;
    typedef Stats stats_t;
    typedef typename fst_t::Weight weight_t;
    typedef typename fst_t::Arc arc_t;
    typedef typename fst_t::StateId stateid_t;
    typedef typename arc_t::Label label_t;

    static constexpr stateid_t final = 0;

    explicit FSTBuilder(fst_t &fst) : states_map_(16, StateEmplace(stats_)), fst_(fst) { Reset(); }

    void Add(const key_t &in, const weight_t &out) {
        // inputs should be sorted
        assert(last_.empty() || last_ < in);

        if (in.empty()) {
            start_out_ = fst::Times(start_out_, out);
            return;
        }

        const auto size = in.size();

        // determine common prefix
        const size_t pref = 1 + CommonPrefixLength(last_, in);

        // add states for current input
        AddStates(size);

        // minimize last word suffix
        Minimize(pref);

        // add current word suffix
        for (size_t i = pref; i <= size; ++i) {
            states_[i - 1].arcs.emplace_back(in[i - 1], &states_[i]);
        }

        const bool is_final = last_.size() != size || pref != (size + 1);

        decltype(fst::DivideLeft(out, out)) output = out;

        for (size_t i = 1; i < pref; ++i) {
            State &s = states_[i];
            State &p = states_[i - 1];

            assert(!p.arcs.empty() && p.arcs.back().label == in[i - 1]);

            auto &last_out = p.arcs.back().out;

            if (last_out != weight_t::One()) {
                auto prefix = fst::Plus(last_out, output);
                const auto suffix = fst::DivideLeft(last_out, prefix);
                output = fst::DivideLeft(output, prefix);

                for (Arc &a : s.arcs) {
                    a.out = fst::Times(suffix, a.out);
                }

                if (s.final) {
                    s.out = fst::Times(suffix, s.out);
                }

                if constexpr (std::is_same_v<decltype(prefix), bytes_view>) {
                    last_out.Resize(prefix.size());
                } else {
                    last_out = std::move(prefix);
                }
            }
        }

        if (is_final) {
            // set final state
            {
                State &s = states_[size];
                s.final = true;
            }

            // set output
            {
                State &s = states_[pref - 1];
                assert(!s.arcs.empty() && s.arcs.back().label == in[pref - 1]);
                s.arcs.back().out = std::move(output);
            }
        } else {
            State &s = states_[size];
            assert(s.arcs.size());
            assert(s.arcs.back().label == in[pref - 1]);
            s.arcs.back().out = fst::Times(s.arcs.back().out, output);
        }

        last_ = in;
    }

    stats_t Finish() {
        stateid_t start = FSTBuilder::final;

        if (!states_.empty()) {
            // minimize last word suffix
            Minimize(1);

            auto &root = states_[0];

            if (!root.arcs.empty() || !root.final) {
                start = states_map_.Insert(root, fst_);
            }
        }

        // set the start state
        fst_.SetStart(start);
        fst_.SetFinal(start, start_out_);

        // count start state
        stats_(start_out_);

        return stats_;
    }

    void Reset() {
        // remove states
        fst_.DeleteStates();

        // initialize final state
        fst_.AddState();
        fst_.SetFinal(final, weight_t::One());

        // reset stats
        stats_ = {};
        stats_.num_states = 1;
        stats_.num_arcs = 0;
        stats_(weight_t::One());

        states_.clear();
        states_map_.Reset();
        last_ = {};
        start_out_ = weight_t{};
    }

private:
    struct State;

    struct Arc {
        Arc(label_t label, State *target) : target(target), label(label) {}

        Arc(Arc &&rhs) noexcept : target(rhs.target), label(rhs.label), out(std::move(rhs.out)) {}

        bool operator==(const arc_t &rhs) const noexcept { return label == rhs.ilabel && id == rhs.nextstate && out == rhs.weight; }

        union {
            State *target;
            stateid_t id;
        };
        label_t label;
        weight_t out{weight_t::One()};
    };

    struct State {
        explicit State(bool final = false) : final(final) {}

        State(State &&rhs) noexcept : arcs(std::move(rhs.arcs)), out(std::move(rhs.out)), final(rhs.final) {}

        void clear() noexcept {
            arcs.clear();
            out = weight_t::One();
            final = false;
        }

        std::vector<Arc> arcs;
        weight_t out{weight_t::One()};
        bool final{false};
    };

    static_assert(std::is_nothrow_move_constructible_v<State>);

    struct StateEqual {
        bool operator()(const State &lhs, stateid_t rhs, const fst_t &fst) const {
            if (lhs.arcs.size() != fst.NumArcs(rhs)) {
                return false;
            }

            fst::ArcIterator<fst_t> rhs_arc(fst, rhs);

            for (auto &lhs_arc : lhs.arcs) {
                if (lhs_arc != rhs_arc.Value()) {
                    return false;
                }

                rhs_arc.Next();
            }

            return true;
        }
    };

    struct StateHash {
        template <typename SizeT>
        inline void HashCombine(SizeT &seed, SizeT value) const noexcept {
            std::hash<SizeT> hasher;
            seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        size_t operator()(const State &s, const fst_t & /*fst*/) const noexcept {
            size_t hash = 0;

            for (auto &a : s.arcs) {
                HashCombine<size_t>(hash, a.label);
                HashCombine<size_t>(hash, a.id);
                HashCombine<size_t>(hash, a.out.Hash());
            }

            return hash;
        }

        size_t operator()(stateid_t id, const fst_t &fst) const noexcept {
            size_t hash = 0;

            for (fst::ArcIterator<fst_t> it(fst, id); !it.Done(); it.Next()) {
                const arc_t &a = it.Value();
                HashCombine<size_t>(hash, a.ilabel);
                HashCombine<size_t>(hash, a.nextstate);
                HashCombine<size_t>(hash, a.weight.Hash());
            }

            return hash;
        }
    };

    class StateEmplace {
    public:
        explicit StateEmplace(stats_t &stats) noexcept : stats_(&stats) {}

        stateid_t operator()(const State &s, fst_t &fst) const {
            const stateid_t id = fst.AddState();

            if (s.final) {
                fst.SetFinal(id, s.out);
                (*stats_)(s.out);
            }

            for (const Arc &a : s.arcs) {
                fst.EmplaceArc(id, a.label, a.label, a.out, a.id);
                (*stats_)(a.out);
            }

            ++stats_->num_states;
            stats_->num_arcs += s.arcs.size();

            return id;
        }

    private:
        stats_t *stats_;
    };

    using states_map = FSTStatesMap<fst_t, State, StateEmplace, StateHash, StateEqual, fst::kNoStateId>;

    void AddStates(size_t size) {
        // reserve size + 1 for root state
        if (states_.size() < ++size) {
            states_.resize(size);
        }
    }

    void Minimize(size_t pref) {
        assert(pref > 0);

        for (size_t i = last_.size(); i >= pref; --i) {
            State &s = states_[i];
            State &p = states_[i - 1];

            assert(!p.arcs.empty());
            p.arcs.back().id = s.arcs.empty() && s.final ? FSTBuilder::final : states_map_.Insert(s, fst_);

            s.clear();
        }
    }

    stats_t stats_;
    states_map states_map_;
    std::vector<State> states_; // current states
    weight_t start_out_;        // output for "empty" input
    key_t last_;
    fst_t &fst_;
};

} // namespace infinity
