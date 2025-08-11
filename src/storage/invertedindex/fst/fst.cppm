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

export module infinity_core:fst.fst;

import :infinity_exception;
import third_party;
import :stl;
import :crc;
import :fst.bytes;
import :fst.node;

/// An acyclic deterministic finite state transducer.
///
/// # How does it work?
///
/// The short answer: it's just like a prefix trie, which compresses keys
/// based only on their prefixes, except that a automaton/transducer also
/// compresses suffixes.

namespace infinity {

struct Meta {
    u64 version_;
    CompiledAddr root_addr_;
    FstType ty_;
    SizeT len_;
    /// A checksum is missing when the FST version is <= 2. (Checksums were
    /// added in version 3.)
    Optional<u32> checksum_;
};

export class Fst {
private:
    Meta meta_;
    u8 *data_ptr_;
    SizeT data_len_;

    friend class FstStream;

public:
    /// Creates a transducer from its representation as a raw byte sequence.
    ///
    /// This operation is intentionally very cheap (no allocations and no
    /// copies). In particular, no verification on the integrity of the
    /// FST is performed. Callers may opt into integrity checks via the
    /// [`Fst::verify`](struct.Fst.html#method.verify) method.
    ///
    /// The fst must have been written with a compatible finite state
    /// transducer builder (`Builder` qualifies). If the format is invalid or
    /// if there is a mismatch between the API version of this library and the
    /// fst, then an error is returned.
    Fst(u8 *data_ptr, SizeT data_len) : data_ptr_(data_ptr), data_len_(data_len) {
        if (data_len < 36) {
            UnrecoverableError(fmt::format("FST invalid fst size, data_len {}", data_len));
        }
        u64 version = ReadU64LE(data_ptr);
        if (version != VERSION) {
            UnrecoverableError(fmt::format("FST version mismatch, expected {}, got {}", VERSION, version));
        }
        u64 ty = ReadU64LE(data_ptr + 8);
        SizeT end;
        Optional<u32> checksum;
        end = data_len - 4;
        checksum = ReadU32LE(data_ptr + data_len - 4);
        SizeT root_addr = ReadU64LE(data_ptr + end - 8);
        SizeT len = ReadU64LE(data_ptr + end - 16);
        // The root node is always the last node written, so its address should
        // be near the end. After the root node is written, we still have to
        // write the root *address* and the number of keys in the FST, along
        // with the checksum. That's 20 bytes. The extra byte used below (21
        // and not 20) comes from the fact that the root address points to
        // the last byte in the root node, rather than the byte immediately
        // following the root node.
        //
        // If this check passes, it is still possible that the FST is invalid
        // but probably unlikely. If this check reports a false positive, then
        // the program will probably panic. In the worst case, the FST will
        // operate but be subtly wrong. (This would require the bytes to be in
        // a format expected by an FST, which is incredibly unlikely.)
        //
        // The special check for EMPTY_ADDRESS is needed since an empty FST
        // has a root node that is empty and final, which means it has the
        // special address `0`. In that case, the FST is the smallest it can
        // be: the version, type, root address and number of nodes. That's
        // 36 bytes (8 byte u64 each).
        //
        // And finally, our calculation changes somewhat based on version.
        // If the FST version is less than 3, then it does not have a checksum.
        SizeT empty_total, addr_offset;
        empty_total = 36;
        addr_offset = 21;
        if ((root_addr == EMPTY_ADDRESS && data_len != empty_total) || (root_addr != EMPTY_ADDRESS && root_addr + addr_offset != data_len)) {
            UnrecoverableError(fmt::format("FST invalid fst size, root_addr{}, data_len {}", root_addr, data_len));
        }
        meta_.version_ = version;
        meta_.root_addr_ = root_addr;
        meta_.ty_ = ty;
        meta_.len_ = len;
        meta_.checksum_ = checksum;
    }

    /// Returns the number of keys in this fst.
    SizeT Len() const { return meta_.len_; }

    /// Returns true if and only if this fst has no keys.
    bool IsEmpty() const { return meta_.len_ == 0; }

    /// Returns the number of bytes used by this fst.
    SizeT Size() const { return data_len_; }

    /// Attempts to verify this FST by computing its checksum.
    ///
    /// This will scan over all of the bytes in the underlying FST, so this
    /// may be an expensive operation depending on the size of the FST.
    ///
    /// This returns an error in two cases:
    ///
    /// 1. When a checksum does not exist, which is the case for FSTs that were
    ///    produced by the `fst` crate before version `0.4`.
    /// 2. When the checksum in the FST does not match the computed checksum
    ///    performed by this procedure.
    void Verify() {
        if (!meta_.checksum_.has_value()) {
            UnrecoverableError("FST checksum is missing");
        }
        u32 expected = meta_.checksum_.value();
        u32 sum = CRC32IEEE::makeCRC(data_ptr_, data_len_ - 4);
        u32 got = ((sum >> 15) | (sum << 17)) + 0xa282ead8ul;
        if (expected != got) {
            UnrecoverableError(fmt::format("FST checksum mismatch, expected {}, got {}is missing", expected, got));
        }
    }

    /// Returns the underlying type of this fst.
    ///
    /// FstType is a convention used to indicate the type of the underlying
    /// transducer.
    FstType FstType() { return meta_.ty_; }

    /// Returns the address of the root node of this fst.
    CompiledAddr RootAddr() { return meta_.root_addr_; }

    /// Retrieves the value associated with a key.
    bool Get(u8 *key_ptr, SizeT key_len, u64 &val) {
        Output out;
        SizeT ti;
        Node node = Root();
        for (SizeT i = 0; i < key_len; i++) {
            bool found = node.FindInput(key_ptr[i], ti);
            if (!found) {
                return false;
            }
            Transition t = node.TransAt(ti);
            node = NodeAt(t.addr_);
            out = out.Cat(t.out_);
        }
        if (!node.IsFinal()) {
            return false;
        }
        out = out.Cat(node.FinalOutput());
        val = out.Value();
        return true;
    }

    /// Returns true if and only if the given key is in this FST.
    bool ContainsKey(u8 *key_ptr, SizeT key_len) {
        u64 val;
        return Get(key_ptr, key_len, val);
    }

private:
    /// Returns the root node of this fst.
    Node Root() { return Node(meta_.root_addr_, data_ptr_); }

    /// Returns the node at the given address.
    ///
    /// Node addresses can be obtained by reading transitions on `Node` values.
    Node NodeAt(CompiledAddr addr) { return Node(addr, data_ptr_); }
};

export struct Bound {
    enum BoundType {
        kIncluded,
        kExcluded,
        kUnbounded,
    } ty_ = kUnbounded;
    Vector<u8> key_;

    Bound() : ty_(kUnbounded) {};
    Bound(BoundType ty) : ty_(ty) {};
    Bound(BoundType ty, u8 *key_ptr, SizeT key_len) : ty_(ty), key_(key_ptr, key_ptr + key_len) {}

    bool ExceededBy(u8 *inp_ptr, SizeT inp_len) {
        if (ty_ == kUnbounded) {
            return false;
        }
        int cmp = CompareBytes(inp_ptr, inp_len, key_.data(), key_.size());
        if (ty_ == kIncluded) {
            return cmp > 0;
        } else {
            return cmp >= 0;
        }
    }

    bool IsEmpty() const { return ty_ == kUnbounded || key_.empty(); }

    bool IsInclusive() { return ty_ != kExcluded; }
};

struct StreamState {
    Node node_;
    SizeT trans_;
    Output out_;
    StreamState(const Node &node, SizeT trans, Output out) : node_(node), trans_(trans), out_(out) {}
    StreamState(const StreamState &other) : node_(other.node_), trans_(other.trans_), out_(other.out_) {}
};

/// A lexicographically ordered stream of key-value pairs from an fst.
export class FstStream {
private:
    Fst &fst_;
    Bound end_at_;
    Vector<u8> inp_;
    Vector<StreamState> stack_;

public:
    FstStream(Fst &fst, Bound min = Bound(), Bound max = Bound()) : fst_(fst) { Reset(min, max); }
    FstStream(Fst &fst, u8 *prefix_ptr, SizeT prefix_len) : fst_(fst) { Reset(prefix_ptr, prefix_len); }

    void Reset(Bound min = Bound(), Bound max = Bound()) {
        end_at_ = max;
        SeekMin(min);
    }

    void Reset(u8 *min_ptr, SizeT min_len, u8 *max_ptr, SizeT max_len) {
        Bound min(Bound::kIncluded, min_ptr, min_len);
        Bound max(Bound::kIncluded, max_ptr, max_len);
        Reset(min, max);
    }

    void Reset(u8 *prefix_ptr, SizeT prefix_len) {
        Bound min(Bound::kIncluded, prefix_ptr, prefix_len);
        Bound max(Bound::kExcluded, prefix_ptr, prefix_len);
        int i;
        for (i = prefix_len - 1; i >= 0; i--) {
            prefix_ptr[i]++;
            if (prefix_ptr[i] != 0x00) {
                break;
            }
        }
        if (i < 0) {
            max = Bound();
        }
        Reset(min, max);
    }

    /// @brief Get next key-value pair per lexicographical order
    /// @param key Stores the key of the pair when found
    /// @param val Stores the value of the pair when found
    /// @return true if found next pair, false if not
    bool Next(Vector<u8> &key, u64 &val) {
        while (!stack_.empty()) {
            StreamState &state = stack_.back();
            if (state.trans_ >= state.node_.Len()) {
                if (state.node_.Addr() != fst_.RootAddr()) {
                    inp_.pop_back();
                }
                stack_.pop_back();
                continue;
            }
            Transition trans = state.node_.TransAt(state.trans_);
            Output out = state.out_.Cat(trans.out_);
            Node next_node = fst_.NodeAt(trans.addr_);
            inp_.push_back(trans.inp_);
            if (end_at_.ExceededBy(inp_.data(), inp_.size())) {
                // We are done, forever.
                stack_.clear();
                return false;
            }
            bool is_final = next_node.IsFinal();
            if (is_final) {
                key = inp_;
                val = out.Cat(next_node.FinalOutput()).Value();
            }
            state.trans_++;
            stack_.emplace_back(next_node, 0, out);
            if (is_final)
                return true;
        }
        return false;
    }

private:
    /// Seeks the underlying stream such that the next key to be read is the
    /// smallest key in the underlying fst that satisfies the given minimum
    /// bound.
    ///
    /// This theoretically should be straight-forward, but we need to make
    /// sure our stack is correct, which includes accounting for automaton
    /// states.
    void SeekMin(Bound min) {
        inp_.clear();
        stack_.clear();
        if (min.IsEmpty()) {
            stack_.emplace_back(fst_.Root(), 0, Output());
            return;
        }
        Vector<u8> &key = min.key_;
        bool inclusive = min.IsInclusive();
        // At this point, we need to find the starting location of `min` in
        // the FST. However, as we search, we need to maintain a stack of
        // reader states so that the reader can pick up where we left off.
        // N.B. We do not necessarily need to stop in a final state, unlike
        // the one-off `find` method. For the example, the given bound might
        // not actually exist in the FST.
        Node node = fst_.Root();
        Output out;
        SizeT ti;
        for (SizeT i = 0; i < key.size(); i++) {
            u8 b = key[i];
            bool found = node.FindInput(b, ti);
            if (found) {
                Transition t = node.TransAt(ti);
                inp_.push_back(b);
                stack_.emplace_back(node, ti + 1, out);
                out = out.Cat(t.out_);
                node = fst_.NodeAt(t.addr_);
            } else {
                // This is a little tricky. We're in this case if the
                // given bound is not a prefix of any key in the FST.
                // Since this is a minimum bound, we need to find the
                // first transition in this node that proceeds the current
                // input byte.
                SizeT j = 0;
                for (; j < node.ntrans_; j++) {
                    Transition t = node.TransAt(j);
                    if (t.inp_ > b) {
                        break;
                    }
                }
                stack_.emplace_back(node, j, out);
                return;
            }
        }
        if (!stack_.empty()) {
            StreamState &last_state = stack_.back();
            if (inclusive) {
                last_state.trans_--;
                inp_.pop_back();
            } else {
                CompiledAddr addr = last_state.node_.TransAt(last_state.trans_ - 1).addr_;
                stack_.emplace_back(fst_.NodeAt(addr), 0, out);
            }
        }
    }
};

} // namespace infinity