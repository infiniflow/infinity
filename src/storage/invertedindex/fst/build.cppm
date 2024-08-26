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
import stl;
export module fst:build;
import :bytes;
import :error;
import :node;
import :registry;
import :writer;

namespace infinity {

struct LastTransition {
    bool present_{false};
    u8 inp_{0};
    Output out_{0};
    LastTransition() = default;
    LastTransition(u8 inp, Output out) : present_(true), inp_(inp), out_(out){};
};

struct BuilderNodeUnfinished {
    BuilderNode node_;
    LastTransition last_;

    void LastCompiled(CompiledAddr addr) {
        if (last_.present_) {
            node_.trans_.emplace_back(last_.inp_, last_.out_, addr);
            last_.present_ = false;
        }
    }

    void AddOutputPrefix(Output prefix) {
        if (node_.is_final_) {
            node_.final_output_ = prefix.Cat(node_.final_output_);
        }
        SizeT ntrans = node_.trans_.size();
        for (SizeT i = 0; i < ntrans; i++) {
            Transition &t = node_.trans_[i];
            t.out_ = prefix.Cat(t.out_);
        }
        if (last_.present_)
            last_.out_ = prefix.Cat(last_.out_);
    }
};

struct UnfinishedNodes {
    Vector<BuilderNodeUnfinished> stack_;

    UnfinishedNodes() {
        stack_.reserve(256);
        PushEmpty(false);
    }

    SizeT Len() { return stack_.size(); }

    void PushEmpty(bool is_final) {
        stack_.resize(stack_.size() + 1);
        stack_.back().node_.is_final_ = is_final;
    }

    BuilderNode &TopNode() { return stack_.back().node_; }

    void Pop() { return stack_.pop_back(); }

    void SetRootOutput(Output out) {
        assert(stack_.size() >= 1);
        auto &unfinished = stack_[0];
        unfinished.node_.is_final_ = true;
        unfinished.node_.final_output_ = out;
    }

    void TopLastFreeze(CompiledAddr addr) {
        assert(stack_.size() >= 1);
        auto &unfinished = stack_.back();
        unfinished.LastCompiled(addr);
    }

    void AddSuffix(u8 *bs_ptr, SizeT bs_len, Output out) {
        if (bs_len == 0)
            return;
        auto &unfinished = stack_.back();
        assert(!unfinished.last_.present_);
        unfinished.last_ = LastTransition(bs_ptr[0], out);
        SizeT len = Len();
        stack_.resize(len + bs_len - 1);
        for (SizeT i = 1; i < bs_len; i++) {
            LastTransition &t = stack_[len + i - 1].last_;
            t.present_ = true;
            t.inp_ = bs_ptr[i];
        }
        PushEmpty(true);
    }

    SizeT FindCommonPrefix(u8 *bs_ptr, SizeT bs_len) {
        SizeT i = 0;
        for (; i < bs_len; i++) {
            if (i >= stack_.size())
                break;
            auto &t = stack_[i].last_;
            if (!t.present_ || t.inp_ != bs_ptr[i])
                break;
        }
        return i;
    }

    SizeT FindCommonPrefixAndSetOutput(u8 *bs_ptr, SizeT bs_len, Output &out) {
        assert(stack_.size() >= 1);
        SizeT i = 0;
        SizeT common_len = std::min(bs_len, static_cast<SizeT>(stack_.size() - 1));
        for (; i < common_len; i++) {
            auto &t = stack_[i].last_;
            assert(t.present_);
            if (t.inp_ != bs_ptr[i])
                break;
            Output common_pre = t.out_.Prefix(out);
            Output add_prefix = t.out_.Sub(common_pre);
            out = out.Sub(common_pre);
            if (!add_prefix.IsZero()) {
                t.out_ = common_pre;
                stack_[i + 1].AddOutputPrefix(add_prefix);
            }
        }
        return i;
    }
};

/// A builder for creating a finite state transducer.
///
/// This is not your average everyday builder. It has two important qualities
/// that make it a bit unique from what you might expect:
///
/// 1. All keys must be added in lexicographic order. Adding a key out of order
///    will result in an error. Additionally, adding a duplicate key with an
///    output value will also result in an error. That is, once a key is
///    associated with a value, that association can never be modified or
///    deleted.
/// 2. The representation of an fst is streamed to *any* `io::Write` as it is
///    built. For an in memory representation, this can be a `Vec<u8>`.
///
/// Point (2) is especially important because it means that an fst can be
/// constructed *without storing the entire fst in memory*. Namely, since it
/// works with any `io::Write`, it can be streamed directly to a file.
///
/// With that said, the builder does use memory, but **memory usage is bounded
/// to a constant size**. The amount of memory used trades off with the
/// compression ratio. Currently, the implementation hard codes this trade off
/// which can result in about 5-20MB of heap usage during construction. (N.B.
/// Guaranteeing a maximal compression ratio requires memory proportional to
/// the size of the fst, which defeats some of the benefit of streaming
/// it to disk. In practice, a small bounded amount of memory achieves
/// close-to-minimal compression ratios.)
///
/// The algorithmic complexity of fst construction is `O(n)` where `n` is the
/// number of elements added to the fst.
export class FstBuilder {
private:
    /// The FST raw data is written directly to `wtr`.
    ///
    /// No internal buffering is done.
    CountingWriter wtr_;
    /// The stack of unfinished nodes.
    ///
    /// An unfinished node is a node that could potentially have a new
    /// transition added to it when a new word is added to the dictionary.
    UnfinishedNodes unfinished_;
    /// A map of finished nodes.
    ///
    /// A finished node is one that has been compiled and written to `wtr`.
    /// After this point, the node is considered immutable and will never
    /// change.
    Registry<BuilderNode, CompiledAddr> registry_;
    /// The last word added.
    ///
    /// This is used to enforce the invariant that words are added in sorted
    /// order.
    Vector<u8> last_;
    /// The address of the last compiled node.
    ///
    /// This is used to optimize states with one transition that point
    /// to the previously compiled node. (The previously compiled node in
    /// this case actually corresponds to the next state for the transition,
    /// since states are compiled in reverse.)
    CompiledAddr last_addr_;
    /// The number of keys added.
    SizeT len_;

public:
    FstBuilder(Writer &wtr, FstType ty = 0) : wtr_(wtr), registry_(16), last_addr_(NONE_ADDRESS), len_(0) {
        IoWriteU64LE(VERSION, wtr_);
        IoWriteU64LE(ty, wtr_);
    }

    /// Adds a byte string to this FST with a zero output value.
    void Add(u8 *bs_ptr, SizeT bs_len) {
        CheckLastKey(bs_ptr, bs_len, false);
        InsertOutput(bs_ptr, bs_len, 0);
    }

    /// Insert a new key-value pair into the fst.
    ///
    /// Keys must be convertible to byte strings. Values must be a `u64`, which
    /// is a restriction of the current implementation of finite state
    /// transducers. (Values may one day be expanded to other types.)
    ///
    /// If a key is inserted that is less than or equal to any previous key
    /// added, then an error is returned. Similarly, if there was a problem
    /// writing to the underlying writer, an error is returned.
    void Insert(u8 *bs_ptr, SizeT bs_len, u64 val) {
        CheckLastKey(bs_ptr, bs_len, true);
        InsertOutput(bs_ptr, bs_len, val);
    }

    Writer &GetWriter() { return wtr_.GetRef(); }

    u64 BytesWritten() { return wtr_.Count(); }

    /// Finishes the construction of the fst and flushes the underlying
    /// writer. After completion, the data written to `W` may be read using
    /// one of `Fst`'s constructor methods.
    void Finish();

private:
    void InsertOutput(u8 *bs_ptr, SizeT bs_len, u64 val);
    void CompileFrom(SizeT istate);
    CompiledAddr Compile(BuilderNode &node);
    void CheckLastKey(u8 *bs_ptr, SizeT bs_len, bool check_dupe);
};

} // namespace infinity