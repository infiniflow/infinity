module;

export module memory_posting;
import stl;
import memory_pool;
import file_writer;
import posting_list_format;
import index_defines;
import lock_free_vector;

namespace infinity {

template <bool REALTIME>
struct PostingTraits;

template <>
struct PostingTraits<false> {
    using DocIDVector = SPLockFreeVector<u32>;
    using TermFreqVector = SPLockFreeVector<u32>;
    using PositionVector = SPLockFreeVector<u32>;
};

template <>
struct PostingTraits<true> {
    using DocIDVector = MPLockFreeVector<u32>;
    using TermFreqVector = MPLockFreeVector<u32>;
    using PositionVector = MPLockFreeVector<u32>;
};

export template <bool REALTIME = false>
class MemoryPosting : public PostingTraits<REALTIME> {
public:
    MemoryPosting(MemoryPool *pool, PostingFormatOption posting_option);

    ~MemoryPosting();

    void Add() {}

private:
    typename PostingTraits<REALTIME>::DocIDVector doc_ids_;
    typename PostingTraits<REALTIME>::TermFreqVector term_freqs_;
    typename PostingTraits<REALTIME>::PositionVector positions_;
};

} // namespace infinity
