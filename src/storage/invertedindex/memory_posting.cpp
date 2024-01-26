module;

module memory_posting;
import stl;
import memory_pool;
import file_writer;
import posting_list_format;
import index_defines;
import lock_free_vector;
import third_party;

namespace infinity {

template <bool REALTIME>
MemoryPosting<REALTIME>::MemoryPosting(MemoryPool *pool, PostingFormatOption posting_option) : doc_ids_(pool), term_freqs_(pool), positions_(pool) {}

template <bool REALTIME>
MemoryPosting<REALTIME>::~MemoryPosting() {}

template class MemoryPosting<true>;
template class MemoryPosting<false>;
} // namespace infinity