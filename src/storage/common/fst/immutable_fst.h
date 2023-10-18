////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2020 ArangoDB GmbH, Cologne, Germany
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

// clang-format off
#include <fst/fst.h>
#include <fst/vector-fst.h>
#include <fst/expanded-fst.h>
// clang-format on

#include "common/utility/builtin.h"
#include "storage/io/file_reader.h"
#include "storage/io/file_writer.h"

namespace fst {

template <typename Arc>
class ImmutableFst;

template <typename A>
class ImmutableFstImpl : public internal::FstImpl<A> {
public:
    using Arc = A;
    using StateId = typename Arc::StateId;
    using Weight = typename Arc::Weight;

    using internal::FstImpl<A>::SetInputSymbols;
    using internal::FstImpl<A>::SetOutputSymbols;
    using internal::FstImpl<A>::SetType;
    using internal::FstImpl<A>::SetProperties;
    using internal::FstImpl<A>::Properties;

    static constexpr const char kTypePrefix[] = "immutable";
    static constexpr size_t kMaxArcs = 1 + std::numeric_limits<uint8_t>::max();
    static constexpr size_t kMaxStateWeight = std::numeric_limits<size_t>::max() >> 1;

    ImmutableFstImpl() : narcs_(0), nstates_(0), start_(kNoStateId) {
        SetType(std::string(kTypePrefix) + "<" + Arc::Type() + ">");
        SetProperties(kNullProperties | kStaticProperties);
    }

    ~ImmutableFstImpl() {
        states_.reset();
        arcs_.reset();
        weights_.reset();
    }

    StateId Start() const noexcept { return start_; }

    Weight Final(StateId s) const noexcept { return states_[s].weight; }

    const Weight &FinalRef(StateId s) const noexcept { return states_[s].weight; }

    StateId NumStates() const noexcept { return nstates_; }

    size_t NumArcs(StateId s) const noexcept { return states_[s].narcs; }

    size_t NumInputEpsilons(StateId) const noexcept { return 0; }

    size_t NumOutputEpsilons(StateId) const noexcept { return 0; }

    static std::shared_ptr<ImmutableFstImpl<Arc>> Read(infinity::FileReader &strm);

    const Arc *Arcs(StateId s) const noexcept { return states_[s].arcs; }

    // Provide information needed for generic state iterator.
    void InitStateIterator(StateIteratorData<Arc> *data) const noexcept {
        data->base = nullptr;
        data->nstates = nstates_;
    }

    // Provide information needed for the generic arc iterator.
    void InitArcIterator(StateId s, ArcIteratorData<Arc> *data) const noexcept {
        data->base = nullptr;
        data->arcs = states_[s].arcs;
        data->narcs = states_[s].narcs;
        data->ref_count = nullptr;
    }

private:
    friend class ImmutableFst<Arc>;

    enum class Version : uint8_t { MIN = 0 };

    struct State {
        const Arc *arcs; // Start of state's arcs in *arcs_.
        size_t narcs;    // Number of arcs (per state).
        Weight weight;   // Final weight.
    };

    // Properties always true of this FST class.
    static constexpr std::uint64_t kStaticProperties = kExpanded;

    std::unique_ptr<State[]> states_;
    std::unique_ptr<Arc[]> arcs_;
    std::unique_ptr<uint8_t[]> weights_;
    size_t narcs_; // Number of arcs.
    size_t weights_size_;
    StateId nstates_; // Number of states.
    StateId start_;   // Initial state.

    ImmutableFstImpl(const ImmutableFstImpl &) = delete;
    ImmutableFstImpl &operator=(const ImmutableFstImpl &) = delete;
};

template <typename Arc>
std::shared_ptr<ImmutableFstImpl<Arc>> ImmutableFstImpl<Arc>::Read(infinity::FileReader &stream) {
    auto impl = std::make_shared<ImmutableFstImpl<Arc>>();

    // read header
    if (Version(stream.ReadByte()) != Version::MIN) {
        return nullptr;
    }

    const uint64_t props = stream.ReadLong();
    const size_t total_weight_size = stream.ReadLong();
    const StateId nstates = stream.ReadInt();
    const StateId start = nstates - stream.ReadVInt();
    uint64_t v = stream.ReadVLong();
    const size_t narcs = ((v >> 1) ^ -(v & 1)) + nstates;

    auto states = std::make_unique<State[]>(nstates);
    auto arcs = std::make_unique<Arc[]>(narcs);
    auto weights = std::make_unique<uint8_t[]>(total_weight_size);

    // read states & arcs
    auto *weight = weights.get();
    auto *arc = arcs.get();
    for (auto state = states.get(), end = state + nstates; state != end; ++state) {
        state->arcs = arc;

        size_t weight_size = stream.ReadVLong();
        const bool has_arcs = !static_cast<bool>(weight_size & 1);
        weight_size = weight_size >> 1;
        state->weight = {weight, weight_size};
        weight += weight_size;

        if (has_arcs) {
            state->narcs = static_cast<uint32_t>(stream.ReadByte()) + 1;

            for (auto *end = arc + state->narcs; arc != end; ++arc) {
                arc->ilabel = stream.ReadByte();
                arc->nextstate = stream.ReadVInt();
                const size_t weight_size = stream.ReadVLong();
                arc->weight = {weight, weight_size};
                weight += weight_size;
            }
        } else {
            state->narcs = 0;
        }
    }

    // read weights
    stream.Read((char *)weights.get(), total_weight_size);

    // noexcept block
    impl->properties_ = props;
    impl->start_ = start;
    impl->nstates_ = nstates;
    impl->narcs_ = narcs;
    impl->states_ = std::move(states);
    impl->arcs_ = std::move(arcs);
    impl->weights_ = std::move(weights);
    impl->weights_size_ = total_weight_size;
    return impl;
}

template <typename A>
class ImmutableFst : public ImplToExpandedFst<ImmutableFstImpl<A>> {
public:
    using Arc = A;
    using StateId = typename Arc::StateId;

    using Impl = ImmutableFstImpl<A>;

    friend class StateIterator<ImmutableFst<Arc>>;
    friend class ArcIterator<ImmutableFst<Arc>>;

    template <typename F, typename G>
    void friend Cast(const F &, G *);

    ImmutableFst() : ImplToExpandedFst<Impl>(std::make_shared<Impl>()) {}

    explicit ImmutableFst(const ImmutableFst<A> &fst, [[maybe_unused]] bool safe = false) : ImplToExpandedFst<Impl>(fst) {}

    // Gets a copy of this ConstFst. See Fst<>::Copy() for further doc.
    ImmutableFst<A> *Copy(bool safe = false) const final { return new ImmutableFst<A>(*this, safe); }

    static ImmutableFst<A> *Read(infinity::FileReader &strm) {
        auto impl = Impl::Read(strm);
        return impl ? new ImmutableFst<A>(std::move(impl)) : nullptr;
    }

    template <typename FST, typename Stats>
    static bool Write(const FST &fst, infinity::FileWriter &strm, const Stats &stats);

    void InitStateIterator(StateIteratorData<Arc> *data) const final { GetImpl()->InitStateIterator(data); }

    void InitArcIterator(StateId s, ArcIteratorData<Arc> *data) const final { GetImpl()->InitArcIterator(s, data); }

    using ImplToFst<Impl, ExpandedFst<Arc>>::GetImpl;

private:
    explicit ImmutableFst(std::shared_ptr<Impl> impl) : ImplToExpandedFst<Impl>(impl) {}

    using ImplToExpandedFst<ImmutableFstImpl<A>>::Write;

    ImmutableFst(const ImmutableFst &) = delete;
    ImmutableFst &operator=(const ImmutableFst &) = delete;
};

template <typename A>
template <typename FST, typename Stats>
bool ImmutableFst<A>::Write(const FST &fst, infinity::FileWriter &stream, const Stats &stats) {
    static_assert(sizeof(StateId) == sizeof(uint32_t));

    auto *impl = fst.GetImpl();

    const auto properties = fst.Properties(kCopyProperties, true) | Impl::kStaticProperties;

    // write header
    stream.WriteByte(static_cast<uint8_t>(Impl::Version::MIN));
    stream.WriteLong(properties);
    stream.WriteLong(stats.total_weight_size);
    stream.WriteInt(static_cast<StateId>(stats.num_states));
    assert(stats.num_states >= static_cast<size_t>(fst.Start()));
    stream.WriteVInt(stats.num_states - fst.Start());
    uint64_t v = stats.num_arcs - stats.num_states;
    v = (v >> 63) ^ (uint64_t(v) << 1);
    stream.WriteVLong(v);
    // write states & arcs
    for (StateIterator<FST> siter(fst); !siter.Done(); siter.Next()) {
        const StateId s = siter.Value();

        size_t weight_size;
        weight_size = impl->FinalRef(s).Size();

        if (likely(weight_size <= Impl::kMaxStateWeight)) {
            const size_t narcs = impl->NumArcs(s);

            stream.WriteVLong((weight_size << 1 | (uint32_t)!narcs));
            if (narcs) {
                // -1 to fit byte_type
                stream.WriteByte(static_cast<uint8_t>((narcs - 1) & 0xFF));

                for (ArcIterator<FST> aiter(fst, s); !aiter.Done(); aiter.Next()) {
                    const auto &arc = aiter.Value();

                    assert(arc.ilabel <= std::numeric_limits<uint8_t>::max());
                    stream.WriteByte(static_cast<uint8_t>(arc.ilabel & 0xFF));
                    stream.WriteVInt(arc.nextstate);
                    stream.WriteVLong(arc.weight.Size());
                }
            }
        } else {
            return false;
        }
    }

    // write weights
    for (StateIterator<FST> siter(fst); !siter.Done(); siter.Next()) {
        const StateId s = siter.Value();

        const auto &weight = impl->FinalRef(s);
        if (!weight.Empty()) {
            stream.Write(weight.c_str(), weight.Size());
        }

        for (ArcIterator<FST> aiter(fst, s); !aiter.Done(); aiter.Next()) {
            const auto &weight = aiter.Value().weight;

            if (!weight.Empty()) {
                stream.Write(weight.c_str(), weight.Size());
            }
        }
    }

    return true;
}

// Specialization for ConstFst; see generic version in fst.h for sample usage
// (but use the ConstFst type instead). This version should inline.
template <typename Arc>
class StateIterator<ImmutableFst<Arc>> {
public:
    using StateId = typename Arc::StateId;

    explicit StateIterator(const ImmutableFst<Arc> &fst) : nstates_(fst.GetImpl()->NumStates()), s_(0) {}

    bool Done() const noexcept { return s_ >= nstates_; }

    StateId Value() const noexcept { return s_; }

    void Next() noexcept { ++s_; }

    void Reset() noexcept { s_ = 0; }

private:
    const StateId nstates_;
    StateId s_;
};

// Specialization for ConstFst; see generic version in fst.h for sample usage
// (but use the ConstFst type instead). This version should inline.
template <typename Arc>
class ArcIterator<ImmutableFst<Arc>> {
public:
    using StateId = typename Arc::StateId;

    ArcIterator(const ImmutableFst<Arc> &fst, StateId s) : arcs_(fst.GetImpl()->Arcs(s)), begin_(arcs_), end_(arcs_ + fst.GetImpl()->NumArcs(s)) {}

    bool Done() const noexcept { return begin_ >= end_; }

    const Arc &Value() const noexcept { return *begin_; }

    void Next() noexcept { ++begin_; }

    size_t Position() const noexcept { return size_t(std::distance(arcs_, begin_)); }

    void Reset() noexcept { begin_ = arcs_; }

    void Seek(size_t a) noexcept { begin_ = arcs_ + a; }

    constexpr std::uint32_t Flags() const { return kArcValueFlags; }

    void SetFlags(std::uint32_t, std::uint32_t) {}

private:
    const Arc *arcs_;
    const Arc *begin_;
    const Arc *end_;
};

} // namespace fst
