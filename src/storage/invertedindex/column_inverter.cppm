module;

export module column_inverter;
import stl;
import analyzer;
import parser;
import column_vector;
import memory_pool;
import pool_allocator;
import term;
import string_ref;

namespace infinity {

class RefCount {
    std::mutex lock_;
    std::condition_variable cv_;
    u32 ref_count_;

public:
    RefCount();
    virtual ~RefCount();
    void Retain() noexcept;
    void Release() noexcept;
    void WaitForZeroRefCount();
    bool ZeroRefCount();
};

class ColumnIndexer;
export class ColumnInverter {
public:
    ColumnInverter(ColumnIndexer *column_indexer);
    ColumnInverter(const ColumnInverter &) = delete;
    ColumnInverter(const ColumnInverter &&) = delete;
    ColumnInverter &operator=(const ColumnInverter &) = delete;
    ColumnInverter &operator=(const ColumnInverter &&) = delete;
    ~ColumnInverter();

    void InvertColumn(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    void InvertColumn(u32 doc_id, const String &val);

    void Commit();

    void Retain() { ref_count_.Retain(); }

    void Release() { ref_count_.Release(); }

    bool ZeroRefCount() { return ref_count_.ZeroRefCount(); }

    void WaitForZeroRefCount() { return ref_count_.WaitForZeroRefCount(); }

    RefCount &GetRefCount() { return ref_count_; }

    struct PosInfo {
        u32 term_num_{0};
        u32 doc_id_{0};
        u32 term_pos_{0};

        bool operator<(const PosInfo &rhs) const {
            if (term_num_ != rhs.term_num_) {
                return term_num_ < rhs.term_num_;
            }
            if (doc_id_ != rhs.doc_id_) {
                return doc_id_ < rhs.doc_id_;
            }
            return term_pos_ < rhs.term_pos_;
        }
    };

private:
    using TermBuffer = Vector<char, PoolAllocator<char>>;
    using PosInfoVec = Vector<PosInfo, PoolAllocator<PosInfo>>;
    using U32Vec = Vector<u32, PoolAllocator<u32>>;

    struct CompareTermRef {
        const char *const term_buffer_;

        CompareTermRef(const TermBuffer &term_buffer) : term_buffer_(&term_buffer[0]) {}

        const char *GetTerm(u32 term_ref) const { return &term_buffer_[term_ref << 2]; }

        bool operator()(const u32 lhs, const u32 rhs) const;
    };

    const char *GetTermFromRef(u32 term_ref) const { return &terms_[term_ref << 2]; }

    const char *GetTermFromNum(u32 term_num) const { return GetTermFromRef(term_refs_[term_num]); }

    u32 GetTermNum(u32 term_ref) const {
        const char *p = &terms_[(term_ref - 1) << 2];
        return *reinterpret_cast<const u32 *>(p);
    }

    void UpdateTermNum(u32 term_ref, u32 term_num) {
        char *p = &terms_[(term_ref - 1) << 2];
        *reinterpret_cast<u32 *>(p) = term_num;
    }

    u32 AddTerm(StringRef term);

    void SortTerms();

    void DoInsert();

    void DoRTInsert();

    ColumnIndexer *column_indexer_{nullptr};
    Analyzer *analyzer_{nullptr};
    bool jieba_specialize_{false};
    PoolAllocator<char> alloc_;
    TermBuffer terms_;
    PosInfoVec positions_;
    U32Vec term_refs_;
    TermList terms_once_;
    RefCount ref_count_;
};
} // namespace infinity
