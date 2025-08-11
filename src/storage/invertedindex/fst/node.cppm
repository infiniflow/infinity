// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module;

#include <cassert>

export module infinity_core:fst.node;

import :stl;
import third_party;
import :fst.common_inputs;
import :fst.writer;
import :fst.bytes;

namespace infinity {

/// CompiledAddr is the type used to address nodes in a finite state
/// transducer.
using CompiledAddr = SizeT;

/// FstType is a convention used to indicate the type of the underlying
/// transducer.
export using FstType = u64;

/// This version number is written to every finite state transducer created by
/// this crate. When a finite state transducer is read, its version number is
/// checked against this value.
const u64 VERSION = 3;

/// A sentinel value used to indicate an empty final state.
const CompiledAddr EMPTY_ADDRESS = 0;

/// A sentinel value used to indicate an invalid state.
/// This is never the address of a node in a serialized transducer.
const CompiledAddr NONE_ADDRESS = 1;

/// The threshold (in number of transitions) at which an index is created for
/// a node's transitions. This speeds up lookup time at the expense of FST
/// size.
const SizeT TRANS_INDEX_THRESHOLD = 32;

/// An output is a value that is associated with a key in a finite state
/// transducer.
///
/// Note that outputs must satisfy an algebra. Namely, it must have an additive
/// identity and the following binary operations defined: `prefix`,
/// `concatenation` and `subtraction`. `prefix` and `concatenation` are
/// commutative while `subtraction` is not. `subtraction` is only defined on
/// pairs of operands where the first operand is greater than or equal to the
/// second operand.
///
/// Currently, output values must be `u64`. However, in theory, an output value
/// can be anything that satisfies the above algebra. Future versions of this
/// crate may make outputs generic on this algebra.
export struct Output {
    u64 val_;

    /// Create a new output from a `u64`.
    Output(u64 val = 0) : val_(val) {}

    bool operator==(const Output &other) const { return val_ == other.val_; }

    bool operator!=(const Output &other) const { return !(*this == other); }

    /// Create a zero output.
    static Output Zero() { return Output(0); }

    /// Retrieve the value inside this output.
    u64 Value() const { return val_; }

    /// Returns true if this is a zero output.
    bool IsZero() const { return val_ == 0; }

    /// Returns the prefix of this output and `o`.
    Output Prefix(Output o) const { return Output(std::min(val_, o.val_)); }

    /// Returns the concatenation of this output and `o`.
    Output Cat(Output o) const { return Output(val_ + o.val_); }

    /// Returns the subtraction of `o` from this output.
    /// This function panics if `self < o`.
    Output Sub(Output o) const {
        assert(val_ >= o.val_);
        return Output(val_ - o.val_);
    }
};

/// A transition from one note to another.
struct Transition {
    /// The byte input associated with this transition.
    u8 inp_;
    /// The output associated with this transition.
    Output out_;
    /// The address of the node that this transition points to.
    CompiledAddr addr_;

    Transition() : inp_(0), out_(Output::Zero()), addr_(NONE_ADDRESS) {}
    Transition(u8 inp, Output out, CompiledAddr addr) : inp_(inp), out_(out), addr_(addr) {}
};

std::ostream &operator<<(std::ostream &os, const Transition &trans) {
    if (trans.out_.IsZero()) {
        os << fmt::format("{} -> {}", char(trans.inp_), trans.addr_).c_str();
    } else {
        os << fmt::format("({}, {}) -> {}", char(trans.inp_), trans.out_.Value(), trans.addr_).c_str();
    }
    return os;
}

// high 4 bits is transition address packed size.
// low 4 bits is output value packed size.
//
// `0` is a legal value which means there are no transitions/outputs.
struct PackSizes {
    u8 val_;

    PackSizes() : val_(0) {}
    PackSizes(u8 val) : val_(val) {}
    void SetTransitionPackSize(u8 size) {
        assert(size <= 8);
        val_ = (val_ & 0b00001111) | (size << 4);
    }
    SizeT TransitionPackSize() const { return SizeT(val_ >> 4); }
    void SetOutputPackSize(u8 size) {
        assert(size <= 8);
        val_ = (val_ & 0b11110000) | size;
    }
    SizeT OutputPackSize() const { return SizeT(val_ & 0b00001111); }
};

struct BuilderNode {
    bool is_final_;
    Output final_output_;
    Vector<Transition> trans_;

    BuilderNode() : is_final_(0), final_output_(0) {}

    BuilderNode(bool is_final, Output final_output, Vector<Transition> &&trans)
        : is_final_(is_final), final_output_(final_output), trans_(std::move(trans)) {}

    BuilderNode(const BuilderNode &source) : is_final_(source.is_final_), final_output_(source.final_output_), trans_(source.trans_) {}

    BuilderNode(BuilderNode &&source) : is_final_(source.is_final_), final_output_(source.final_output_), trans_(std::move(source.trans_)) {}

    BuilderNode &operator=(BuilderNode &&source) noexcept {
        is_final_ = source.is_final_;
        final_output_ = source.final_output_;
        trans_ = std::move(source.trans_);
        return *this;
    }

    // Need to provide a hash function and operator== for BuilderNode so that it can be used as a key in a std::unordered_map.
    SizeT Hash() const {
        // Basic FNV-1a hash as described:
        // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
        //
        // In unscientific experiments, this provides the same compression
        // as `std::hash::SipHasher` but is much much faster.
        const u64 FNV_PRIME = 1099511628211ULL;
        u64 h = 14695981039346656037ULL;
        h = (h ^ (u64)is_final_) * FNV_PRIME;
        h = (h ^ final_output_.Value()) * FNV_PRIME;
        SizeT ntrans = trans_.size();
        for (SizeT i = 0; i < ntrans; i++) {
            const Transition &t = trans_[i];
            h = (h ^ (u64)t.inp_) * FNV_PRIME;
            h = (h ^ t.out_.Value()) * FNV_PRIME;
            h = (h ^ (u64)t.addr_) * FNV_PRIME;
        }
        return h;
    }

    bool operator==(const BuilderNode &other) const {
        if (is_final_ != other.is_final_ || final_output_ != other.final_output_ || trans_.size() != other.trans_.size())
            return false;
        SizeT ntrans = trans_.size();
        for (SizeT i = 0; i < ntrans; i++) {
            const Transition &t1 = trans_[i];
            const Transition &t2 = other.trans_[i];
            if (t1.inp_ != t2.inp_ || t1.out_ != t2.out_ || t1.addr_ != t2.addr_)
                return false;
        }
        return true;
    }

    bool operator!=(const BuilderNode &other) const { return !(*this == other); }
};

class State;

/// Node represents a single state in a finite state transducer.
/// Nodes are very cheap to construct.
struct Node {
    u8 state_;
    bool is_final_;
    u8 ntrans_;
    u8 *data_ptr_;
    SizeT data_len_;
    CompiledAddr start_;
    SizeT end_;
    PackSizes sizes_;
    Output final_output_;

    Node(const Node &source) = default;

    Node &operator=(const Node &source) = default;

    /// Creates a new note at the address given.
    /// `data` should be a slice to an entire FST.
    Node(CompiledAddr addr, u8 *data_ptr);

    static void Compile(Writer &wtr, CompiledAddr last_addr, CompiledAddr addr, BuilderNode &node);

    /// Returns the transition at index `i`.
    Transition TransAt(SizeT i);

    /// Returns the transition address of the `i`th transition.
    CompiledAddr TransAddr(SizeT i);

    /// Finds the `i`th transition corresponding to the given input byte.
    bool FindInput(u8 b, SizeT &ti);

    /// If this node is final and has a terminal output value, then it is
    /// returned. Otherwise, a zero output is returned.
    Output FinalOutput() { return is_final_ ? final_output_ : Output::Zero(); }

    /// Returns true if and only if this node corresponds to a final or "match"
    /// state in the finite state transducer.
    bool IsFinal() { return is_final_; }

    /// Returns the number of transitions in this node.
    ///
    /// The maximum number of transitions is 256.
    SizeT Len() { return ntrans_; }

    /// Returns true if and only if this node has zero transitions.
    bool IsEmpty() { return ntrans_ == 0; }

    /// Return the address of this node.
    CompiledAddr Addr() { return start_; }
};

class State {
private:
    // MSB 2 bits of val_ is the type of state.
    // 11: StateOneTransNext
    // 10: StateOneTrans
    // others: StateAnyTrans
    u8 val_;

public:
    State() = delete;
    State(u8 val) : val_(val) {}
    State(u8 *data_ptr, SizeT addr) : val_(data_ptr[addr]) { assert(addr != EMPTY_ADDRESS); }
    State(const State &source) : val_(source.val_) {}
    State &operator=(const State &source) {
        val_ = source.val_;
        return *this;
    }

    /// Returns the transition at index `i`.
    struct Transition TransAt(const Node &node, SizeT i) const;

    /// Returns the transition address of the `i`th transition.
    CompiledAddr TransAddr(const Node &node, SizeT i) const;

    /// FindInput returns the index of the transition with the given input.
    bool FindInput(const Node &node, u8 b, SizeT &ti) const;

    /// CommonIdx translate a byte to an index in the COMMON_INPUTS_INV array.
    static u8 CommonIdx(u8 input, u8 max) {
        u8 val = u8((u32(COMMON_INPUTS[SizeT(input)]) + 1) % 256);
        if (val > max) {
            return 0;
        } else {
            return val;
        }
    }

    /// CommonInput translates a common input index stored in a serialized FST
    /// to the corresponding byte.
    static bool CommonInput(u8 idx, u8 &common_input) {
        if (idx == 0) {
            return false;
        } else {
            common_input = COMMON_INPUTS_INV[SizeT(idx - 1)];
            return true;
        }
    }

    static u8 PackDelta(Writer &wtr, CompiledAddr node_addr, CompiledAddr trans_addr) {
        u8 nbytes = PackDeltaSize(node_addr, trans_addr);
        PackDeltaIn(wtr, node_addr, trans_addr, nbytes);
        return nbytes;
    }

    static void PackDeltaIn(Writer &wtr, CompiledAddr node_addr, CompiledAddr trans_addr, u8 nbytes) {
        SizeT delta_addr;
        if (trans_addr == EMPTY_ADDRESS) {
            delta_addr = EMPTY_ADDRESS;
        } else {
            delta_addr = node_addr - trans_addr;
        }
        PackUintIn(wtr, delta_addr, nbytes);
    }

    static u8 PackDeltaSize(CompiledAddr node_addr, CompiledAddr trans_addr) {
        SizeT delta_addr;
        if (trans_addr == EMPTY_ADDRESS) {
            delta_addr = EMPTY_ADDRESS;
        } else {
            delta_addr = node_addr - trans_addr;
        }
        return PackSize(delta_addr);
    }

    static CompiledAddr UnpackDelta(u8 *slice_ptr, SizeT trans_pack_size, SizeT node_addr) {
        u64 delta = UnpackUint(slice_ptr, trans_pack_size);
        SizeT delta_addr = SizeT(delta);
        if (delta_addr == EMPTY_ADDRESS) {
            return EMPTY_ADDRESS;
        } else {
            return node_addr - delta_addr;
        }
    }
};

class StateOneTransNext {
private:
    u8 val_;

public:
    StateOneTransNext() : val_(0b11000000) {}
    StateOneTransNext(u8 val) : val_(val) {}

    static void Compile(Writer &wtr, u8 input) {
        StateOneTransNext state;
        state.SetCommonInput(input);
        if (state.InputLen()) {
            wtr.Write(&input, 1);
        }
        wtr.Write(&state.val_, 1);
    }

    void SetCommonInput(u8 input) { val_ = (val_ & 0b11000000) | State::CommonIdx(input, 0b111111); }

    bool CommonInput(u8 &common_input) const { return State::CommonInput(val_ & 0b00111111, common_input); }

    SizeT InputLen() const {
        if ((val_ & 0b00111111) != 0)
            return 0;
        else
            return 1;
    }

    SizeT EndAddr(SizeT data_len) const { return data_len - 1 - InputLen(); }

    u8 Input(const Node &node) const {
        u8 inp;
        bool found = CommonInput(inp);
        if (found) {
            return inp;
        } else {
            return node.data_ptr_[node.start_ - 1];
        }
    }

    CompiledAddr TransAddr(const Node &node) const { return CompiledAddr(node.end_) - 1; }

    /// Returns the transition at index `i`.
    Transition TransAt(const Node &node, SizeT i) const {
        assert(i == 0);
        return Transition(Input(node), Output::Zero(), TransAddr(node));
    }

    /// Returns the transition address of the `i`th transition.
    CompiledAddr TransAddr(const Node &node, SizeT i) const {
        assert(i == 0);
        return TransAddr(node);
    }

    /// FindInput returns the index of the transition with the given input.
    bool FindInput(const Node &node, u8 b, SizeT &ti) const {
        if (Input(node) == b) {
            ti = 0;
            return true;
        } else {
            return false;
        }
    }
};

class StateOneTrans {
private:
    u8 val_;

public:
    StateOneTrans() : val_(0b10000000) {}
    StateOneTrans(u8 val) : val_(val) {}

    static void Compile(Writer &wtr, CompiledAddr addr, Transition trans) {
        u64 out = trans.out_.Value();
        u8 output_pack_size = 0;
        if (out != 0) {
            output_pack_size = PackUint(wtr, out);
        }
        u8 trans_pack_size = State::PackDelta(wtr, addr, trans.addr_);

        PackSizes pack_sizes;
        pack_sizes.SetOutputPackSize(output_pack_size);
        pack_sizes.SetTransitionPackSize(trans_pack_size);
        wtr.Write(&pack_sizes.val_, 1);

        StateOneTrans state;
        state.SetCommonInput(trans.inp_);
        if (state.InputLen()) {
            wtr.Write(&trans.inp_, 1);
        }
        wtr.Write(&state.val_, 1);
    }

    void SetCommonInput(u8 input) { val_ = (val_ & 0b10000000) | State::CommonIdx(input, 0b111111); }

    bool CommonInput(u8 &common_input) const { return State::CommonInput(val_ & 0b00111111, common_input); }

    SizeT InputLen() const {
        if ((val_ & 0b00111111) != 0)
            return 0;
        else
            return 1;
    }

    PackSizes Sizes(u8 *data_ptr, SizeT data_len) const {
        PackSizes pack_sizes;
        pack_sizes.val_ = data_ptr[data_len - 1 - InputLen() - 1];
        return pack_sizes;
    }

    SizeT EndAddr(SizeT data_len, PackSizes sizes) const {
        return data_len - 1 - InputLen() - 1 - sizes.TransitionPackSize() - sizes.OutputPackSize();
    }

    u8 Input(const Node &node) const {
        u8 inp;
        bool found = CommonInput(inp);
        if (found) {
            return inp;
        } else {
            return node.data_ptr_[node.start_ - 1];
        }
    }

    Output OutputOf(const Node &node) const {
        SizeT osize = node.sizes_.OutputPackSize();
        if (osize == 0)
            return Output::Zero();
        SizeT tsize = node.sizes_.TransitionPackSize();
        SizeT i = node.start_ - InputLen() - 1 - tsize - osize;
        return Output(UnpackUint(node.data_ptr_ + i, osize));
    }

    CompiledAddr TransAddr(const Node &node) const {
        SizeT tsize = node.sizes_.TransitionPackSize();
        SizeT i = node.start_ - InputLen() - 1 - tsize;
        return State::UnpackDelta(node.data_ptr_ + i, tsize, node.end_);
    }

    /// Returns the transition at index `i`.
    Transition TransAt(const Node &node, SizeT i) const {
        assert(i == 0);
        return Transition(Input(node), OutputOf(node), TransAddr(node));
    }

    /// Returns the transition address of the `i`th transition.
    CompiledAddr TransAddr(const Node &node, SizeT i) const {
        assert(i == 0);
        return TransAddr(node);
    }

    /// FindInput returns the index of the transition with the given input.
    bool FindInput(const Node &node, u8 b, SizeT &ti) const {
        if (Input(node) == b) {
            ti = 0;
            return true;
        } else {
            return false;
        }
    }
};

class StateAnyTrans {
private:
    u8 val_;

public:
    StateAnyTrans() : val_(0b00000000) {}
    StateAnyTrans(u8 val) : val_(val) {}

    static void Compile(Writer &wtr, CompiledAddr addr, const BuilderNode &node) {
        SizeT ntrans = node.trans_.size();
        assert(ntrans <= 256);
        u8 tsize = 0;
        u8 osize = PackSize(node.final_output_.Value());
        bool any_outs = !node.final_output_.IsZero();
        for (SizeT i = 0; i < ntrans; i++) {
            const Transition &t = node.trans_[i];
            tsize = std::max(tsize, State::PackDeltaSize(addr, t.addr_));
            osize = std::max(osize, PackSize(t.out_.Value()));
            any_outs = any_outs || !t.out_.IsZero();
        }

        PackSizes pack_sizes;
        if (any_outs) {
            pack_sizes.SetOutputPackSize(osize);
        }
        pack_sizes.SetTransitionPackSize(tsize);

        StateAnyTrans state;
        state.SetFinalState(node.is_final_);
        state.SetStateNtrans(ntrans);

        if (any_outs) {
            if (node.is_final_) {
                PackUintIn(wtr, node.final_output_.Value(), osize);
            }
            for (int i = int(ntrans) - 1; i >= 0; i--) {
                const Transition &t = node.trans_[i];
                PackUintIn(wtr, t.out_.Value(), osize);
            }
        }
        for (int i = int(ntrans) - 1; i >= 0; i--) {
            const Transition &t = node.trans_[i];
            State::PackDeltaIn(wtr, addr, t.addr_, tsize);
        }
        for (int i = int(ntrans) - 1; i >= 0; i--) {
            const Transition &t = node.trans_[i];
            wtr.Write(&t.inp_, 1);
        }
        if (ntrans > TRANS_INDEX_THRESHOLD) {
            // A value of 255 indicates that no transition exists for the byte
            // at that index. (Except when there are 256 transitions.) Namely,
            // any value greater than or equal to the number of transitions in
            // this node indicates an absent transition.
            u8 index[256];
            std::memset(index, 255, 256);
            for (SizeT i = 0; i < ntrans; i++) {
                index[SizeT(node.trans_[i].inp_)] = u8(i);
            }
            wtr.Write(index, 256);
        }

        wtr.Write(&pack_sizes.val_, 1);
        if (state.StateNtrans() == 0) {
            if (ntrans == 256) {
                // 256 can't be represented in a u8, so we abuse the fact that
                // the # of transitions can never be 1 here, since 1 is always
                // encoded in the state byte.
                ntrans = 1;
            }
            PackUintIn(wtr, ntrans, 1);
        }
        wtr.Write(&state.val_, 1);
    }

    void SetFinalState(bool yes) {
        if (yes) {
            val_ |= 0b01000000;
        } else {
            val_ &= 0b10111111;
        }
    }

    bool IsFinalState() const { return val_ & 0b01000000; }

    void SetStateNtrans(u8 ntrans) {
        val_ &= 0b11000000;
        if (ntrans <= 0b00111111) {
            val_ |= ntrans;
        }
    }

    SizeT StateNtrans() const { return val_ & 0b00111111; }

    PackSizes Sizes(u8 *data_ptr, SizeT data_len) const {
        PackSizes pack_sizes;
        pack_sizes.val_ = data_ptr[data_len - 1 - NtransLen() - 1];
        return pack_sizes;
    }

    SizeT TotalTransSize(PackSizes sizes, SizeT ntrans) const {
        SizeT index_size = TransIndexSize(ntrans);
        return ntrans + (ntrans * sizes.TransitionPackSize()) + index_size;
    }

    SizeT TransIndexSize(SizeT ntrans) const {
        if (ntrans > TRANS_INDEX_THRESHOLD) {
            return 256;
        } else {
            return 0;
        }
    }

    SizeT NtransLen() const {
        if (this->StateNtrans() == 0) {
            return 1;
        } else {
            return 0;
        }
    }

    SizeT Ntrans(u8 *data_ptr, SizeT data_len) const {
        SizeT ntrans = this->StateNtrans();
        if (ntrans != 0) {
            return ntrans;
        } else {
            ntrans = data_ptr[data_len - 2];
            if (ntrans == 1) {
                // "1" is never a normal legal value here, because if there
                // is only 1 transition, then it is encoded in the state byte.
                ntrans = 256;
            }
            return ntrans;
        }
    }

    Output FinalOutput(u8 *data_ptr, SizeT data_len, PackSizes sizes, SizeT ntrans) const {
        SizeT osize = sizes.OutputPackSize();
        if (osize == 0 || !this->IsFinalState()) {
            return Output::Zero();
        }
        SizeT at = data_len - 1 - NtransLen() - 1                     // pack size
                   - TotalTransSize(sizes, ntrans) - (ntrans * osize) // output values
                   - osize;                                           // the desired output value
        return Output(UnpackUint(data_ptr + at, u8(osize)));
    }

    SizeT EndAddr(SizeT data_len, PackSizes sizes, SizeT ntrans) const {
        SizeT osize = sizes.OutputPackSize();
        SizeT final_osize = 0;
        if (IsFinalState()) {
            final_osize = osize;
        }
        return data_len - 1 - NtransLen() - 1                     // pack size
               - TotalTransSize(sizes, ntrans) - (ntrans * osize) // output values
               - final_osize;                                     // final output
    }

    /// Returns the transition address of the `i`th transition.
    CompiledAddr TransAddr(const Node &node, SizeT i) const {
        assert(i < node.ntrans_);
        SizeT tsize = node.sizes_.TransitionPackSize();
        SizeT at = node.start_ - NtransLen() - 1                 // pack size
                   - TransIndexSize(node.ntrans_) - node.ntrans_ // inputs
                   - (i * tsize)                                 // the previous transition addresses
                   - tsize;                                      // the desired transition address
        return State::UnpackDelta(node.data_ptr_ + at, tsize, node.end_);
    }

    /// InputAt returns the input byte for the transition at the given index.
    u8 InputAt(const Node &node, SizeT i) const {
        SizeT at = node.start_ - NtransLen() - 1           // pack size
                   - TransIndexSize(node.ntrans_) - i - 1; // the input byte
        return node.data_ptr_[at];
    }

    /// FindInput returns the index of the transition with the given input.
    bool FindInput(const Node &node, u8 b, SizeT &ti) const {
        if (node.ntrans_ > TRANS_INDEX_THRESHOLD) {
            SizeT start = node.start_ - NtransLen() - 1 - 256;
            SizeT i = node.data_ptr_[start + SizeT(b)];
            if (i > node.ntrans_) {
                return false;
            } else {
                ti = i;
                return true;
            }
        } else {
            SizeT start = node.start_ - NtransLen() - 1 // pack size
                          - node.ntrans_;               // inputs
            SizeT end = start + node.ntrans_;
            for (SizeT i = start; i < end; i++) {
                if (node.data_ptr_[i] == b) {
                    ti = node.ntrans_ - (i - start) - 1;
                    return true;
                }
            }
            return false;
        }
    }

    /// OutputAt returns the output for the transition at the given index.
    Output OutputAt(const Node &node, SizeT i) const {
        SizeT osize = node.sizes_.OutputPackSize();
        if (osize == 0) {
            return Output::Zero();
        }
        SizeT at = node.start_ - NtransLen() - 1                             // pack size
                   - TotalTransSize(node.sizes_, node.ntrans_) - (i * osize) // the previous output values
                   - osize;                                                  // the desired output value
        return Output(UnpackUint(node.data_ptr_ + at, osize));
    }

    /// Returns the transition at index `i`.
    Transition TransAt(const Node &node, SizeT i) const { return Transition(InputAt(node, i), OutputAt(node, i), TransAddr(node, i)); }
};

/// Returns the transition at index `i`.
struct Transition State::TransAt(const Node &node, SizeT i) const {
    switch (val_ & 0b11000000) {
        case 0b11000000:
            return StateOneTransNext(val_).TransAt(node, i);
        case 0b10000000:
            return StateOneTrans(val_).TransAt(node, i);
        default:
            return StateAnyTrans(val_).TransAt(node, i);
    }
}

/// Returns the transition address of the `i`th transition.
CompiledAddr State::TransAddr(const Node &node, SizeT i) const {
    switch (val_ & 0b11000000) {
        case 0b11000000:
            return StateOneTransNext(val_).TransAddr(node, i);
        case 0b10000000:
            return StateOneTrans(val_).TransAddr(node, i);
        default:
            return StateAnyTrans(val_).TransAddr(node, i);
    }
}

/// FindInput returns the index of the transition with the given input.
bool State::FindInput(const Node &node, u8 b, SizeT &ti) const {
    switch (val_ & 0b11000000) {
        case 0b11000000:
            return StateOneTransNext(val_).FindInput(node, b, ti);
        case 0b10000000:
            return StateOneTrans(val_).FindInput(node, b, ti);
        default:
            return StateAnyTrans(val_).FindInput(node, b, ti);
    }
}

Node::Node(CompiledAddr addr, u8 *data_ptr) : data_ptr_(data_ptr), data_len_(addr + 1), final_output_(Output::Zero()) {
    if (addr == EMPTY_ADDRESS) {
        data_len_ = 0;
        state_ = 0;
        start_ = EMPTY_ADDRESS;
        end_ = EMPTY_ADDRESS;
        is_final_ = true;
        ntrans_ = 0;
        sizes_ = PackSizes();
        return;
    }
    start_ = addr;
    state_ = data_ptr[addr];
    switch (state_ & 0b11000000) {
        case 0b11000000: {
            StateOneTransNext state(state_);
            end_ = state.EndAddr(data_len_);
            is_final_ = false;
            ntrans_ = 1;
            sizes_ = PackSizes();
            break;
        }
        case 0b10000000: {
            StateOneTrans state(state_);
            sizes_ = state.Sizes(data_ptr, data_len_);
            end_ = state.EndAddr(data_len_, sizes_);
            is_final_ = false;
            ntrans_ = 1;
            break;
        }
        default: {
            StateAnyTrans state(state_);
            SizeT ntrans = state.Ntrans(data_ptr, data_len_);
            sizes_ = state.Sizes(data_ptr, data_len_);
            end_ = state.EndAddr(data_len_, sizes_, ntrans);
            is_final_ = state.IsFinalState();
            ntrans_ = ntrans;
            final_output_ = state.FinalOutput(data_ptr, data_len_, sizes_, ntrans);
        }
    }
}

void Node::Compile(Writer &wtr, CompiledAddr last_addr, CompiledAddr addr, BuilderNode &node) {
    assert(node.trans_.size() <= 256);
    if (node.trans_.empty() && node.is_final_ && node.final_output_.IsZero()) {
        return;
    } else if (node.trans_.size() != 1 || node.is_final_) {
        StateAnyTrans::Compile(wtr, addr, node);
    } else {
        if (node.trans_[0].addr_ == last_addr && node.trans_[0].out_.IsZero()) {
            StateOneTransNext::Compile(wtr, node.trans_[0].inp_);
        } else {
            StateOneTrans::Compile(wtr, addr, node.trans_[0]);
        }
    }
}

Transition Node::TransAt(SizeT i) { return State(state_).TransAt(*this, i); }

CompiledAddr Node::TransAddr(SizeT i) { return State(state_).TransAddr(*this, i); }

bool Node::FindInput(u8 b, SizeT &ti) { return State(state_).FindInput(*this, b, ti); }

} // namespace infinity
