module;

#include <string.h>
#include <string_view>

export module parallel_column_inverter;
import stl;
import analyzer;
import parser;
import column_vector;
import memory_pool;
import pool_allocator;
import term;
import index_defines;
import third_party;
import column_inverter;

namespace infinity {

struct Hasher {
    SizeT operator()(const StringView &value) const { return std::hash<StringView>{}(value); }
};

export inline bool StringViewCompare(StringView lhs, StringView rhs) {
    const SizeT size = std::min(lhs.size(), rhs.size());
    const auto res = ::memcmp(lhs.data(), rhs.data(), size);

    if (0 == res) {
        return lhs.size() < rhs.size();
    }

    return res < 0;
}

export struct HashedStringView : public StringView {
    explicit HashedStringView(StringView ref, const Hasher &hasher = Hasher{}) : HashedStringView{ref, hasher(ref)} {}

    HashedStringView(StringView ref, SizeT hash) : StringView(ref), hash_(hash) {}

    SizeT hash_;
};

export struct ValueRef {
    explicit ValueRef(SizeT ref, SizeT hash) : ref_{ref}, hash_{hash} {}

    SizeT ref_;
    SizeT hash_;
};

struct ValueRefHash {
    using is_transparent = void;

    SizeT operator()(const ValueRef &value) const noexcept { return value.hash_; }

    SizeT operator()(const HashedStringView &value) const noexcept { return value.hash_; }
};

export struct TermPosting {
    TermPosting(const char *data, u32 size) : term_(data, size) {}

    struct PosInfo {
        u32 doc_id_{0};
        u32 term_pos_{0};

        bool operator<(const PosInfo &rhs) const {
            if (doc_id_ != rhs.doc_id_) {
                return doc_id_ < rhs.doc_id_;
            }
            return term_pos_ < rhs.term_pos_;
        }
    };
    StringView term_;
    Vector<PosInfo> values_;
};

class MemoryIndexer;

export class TermPostings {
public:
    TermPostings(const TermPostings &) = delete;
    TermPostings(const TermPostings &&) = delete;
    TermPostings &operator=(const TermPostings &) = delete;
    TermPostings &operator=(const TermPostings &&) = delete;
    explicit TermPostings(MemoryIndexer *memory_indexer);
    ~TermPostings() {}

    void Clear() {
        terms_.clear();
        postings_.clear();
    }

    void GetSorted(Vector<const TermPosting *> &term_postings);

    TermPosting *Emplace(StringView term);

    bool Empty() const { return terms_.empty(); }

    SizeT Size() const { return terms_.size(); }

    struct TermEq {
        using is_transparent = void;
        explicit TermEq(const Vector<TermPosting> &data) : data_{&data} {}
        bool operator()(const ValueRef &lhs, const ValueRef &rhs) const noexcept { return lhs.ref_ == rhs.ref_; }
        bool operator()(const ValueRef &lhs, const HashedStringView &rhs) const { return (*data_)[lhs.ref_].term_ == rhs; }
        bool operator()(const HashedStringView &lhs, const ValueRef &rhs) const { return this->operator()(rhs, lhs); }

        const Vector<TermPosting> *data_;
    };
    MemoryIndexer *memory_indexer_{nullptr};
    Vector<TermPosting> postings_;
    FlatHashSet<ValueRef, ValueRefHash, TermEq> terms_;
};

export class ParallelColumnInverter : public ColumnInverter {
public:
    explicit ParallelColumnInverter(MemoryIndexer *memory_indexer);
    ParallelColumnInverter(const ParallelColumnInverter &) = delete;
    ParallelColumnInverter(const ParallelColumnInverter &&) = delete;
    ParallelColumnInverter &operator=(const ParallelColumnInverter &) = delete;
    ParallelColumnInverter &operator=(const ParallelColumnInverter &&) = delete;
    virtual ~ParallelColumnInverter();

    void InvertColumn(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) override;

    void InvertColumn(u32 doc_id, const String &val) override;

    void Flush() override;

    TermPostings *GetTermPostings() { return term_postings_.get(); }

private:
    MemoryIndexer *memory_indexer_{nullptr};
    UniquePtr<TermPostings> term_postings_;
    Analyzer *analyzer_{nullptr};
    bool jieba_specialize_{false};
    PoolAllocator<char> alloc_;
    TermList terms_once_;
};

export class ParallelColumnInverters : public InverterReference {
public:
    ParallelColumnInverters(MemoryIndexer *memory_indexer, u32 size);
    virtual ~ParallelColumnInverters() {}

    u32 Size() { return size_; }

    void Commit() override;

    Vector<UniquePtr<ParallelColumnInverter>> inverters_;

private:
    MemoryIndexer *memory_indexer_{nullptr};
    u32 size_;
};

} // namespace infinity