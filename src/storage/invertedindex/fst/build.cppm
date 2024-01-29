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
    u8 inp_;
    Output out_;
};

struct BuilderNodeUnfinished {
    UniquePtr<BuilderNode> node_;
    Optional<LastTransition> last_;

    void LastCompiled(CompiledAddr addr) {
        if (last_.has_value()) {
            node_->trans_.push_back(Transition{last_->inp_, last_->out_, addr});
            last_.reset();
        }
    }

    void AddOutputPrefix(Output prefix) {
        if (node_->is_final_) {
            node_->final_output_ = prefix.Cat(node_->final_output_);
        }
        SizeT ntrans = node_->trans_.size();
        for (SizeT i = 0; i < ntrans; i++) {
            Transition &t = node_->trans_[i];
            t.out_ = prefix.Cat(t.out_);
        }
        if (last_.has_value())
            last_->out_ = prefix.Cat(last_->out_);
    }
};

struct UnfinishedNodes {
    Vector<UniquePtr<BuilderNodeUnfinished>> stack_;

    UnfinishedNodes() {
        stack_.reserve(64);
        PushEmpty(false);
    }

    SizeT Len() { return stack_.size(); }

    void PushEmpty(bool is_final) {
        auto node = MakeUnique<BuilderNode>(is_final, Output::Zero(), Vector<Transition>{});
        auto unfinished = MakeUnique<BuilderNodeUnfinished>(std::move(node), None);
        stack_.push_back(std::move(unfinished));
    }
    UniquePtr<BuilderNode> PopRoot() {
        assert(stack_.size() == 1);
        auto unfinished = std::move(stack_.back());
        stack_.pop_back();
        assert(!unfinished->last_.has_value());
        return std::move(unfinished->node_);
    }

    UniquePtr<BuilderNode> PopFreeze(CompiledAddr addr) {
        auto unfinished = std::move(stack_.back());
        stack_.pop_back();
        unfinished->LastCompiled(addr);
        return std::move(unfinished->node_);
    }

    UniquePtr<BuilderNode> PopEmpty() {
        auto unfinished = std::move(stack_.back());
        stack_.pop_back();
        assert(!unfinished->last_.has_value());
        return std::move(unfinished->node_);
    }

    void SetRootOutput(Output out) {
        assert(stack_.size() >= 1);
        auto &unfinished = stack_[0];
        unfinished->node_->is_final_ = true;
        unfinished->node_->final_output_ = out;
    }

    void TopLastFreeze(CompiledAddr addr) {
        assert(stack_.size() >= 1);
        auto &unfinished = stack_.back();
        unfinished->LastCompiled(addr);
    }

    void AddSuffix(u8 *bs_ptr, SizeT bs_len, Output out) {
        if (bs_len == 0)
            return;
        auto &unfinished = stack_.back();
        assert(!unfinished->last_.has_value());
        unfinished->last_ = LastTransition{bs_ptr[0], out};
        for (SizeT i = 1; i < bs_len; i++) {
            auto unfinished = MakeUnique<BuilderNodeUnfinished>(MakeUnique<BuilderNode>(), LastTransition{bs_ptr[i], Output::Zero()});
            stack_.push_back(std::move(unfinished));
        }
        PushEmpty(true);
    }

    SizeT FindCommonPrefix(u8 *bs_ptr, SizeT bs_len) {
        SizeT i = 0;
        for (; i < bs_len; i++) {
            if (i >= stack_.size())
                break;
            auto &t = stack_[i]->last_;
            if (!t.has_value() || t->inp_ != bs_ptr[i])
                break;
        }
        return i;
    }

    SizeT FindCommonPrefixAndSetOutput(u8 *bs_ptr, SizeT bs_len, Output &out) {
        assert(stack_.size() >= 1);
        SizeT i = 0;
        SizeT common_len = std::min(bs_len, stack_.size() - 1);
        for (; i < common_len; i++) {
            auto &t = stack_[i]->last_;
            assert(t.has_value());
            if (t->inp_ != bs_ptr[i])
                break;
            Output common_pre = t->out_.Prefix(out);
            Output add_prefix = t->out_.Sub(common_pre);
            out = out.Sub(common_pre);
            if (!add_prefix.IsZero()) {
                t->out_ = common_pre;
                stack_[i + 1]->AddOutputPrefix(add_prefix);
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
    Registry<BuilderNode> registry_;
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
    FstBuilder(Writer &wtr, FstType ty = 0) : wtr_(wtr), registry_(10000), last_addr_(NONE_ADDRESS), len_(0) {
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