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

template <bool REALTIME>
void MemoryPosting<REALTIME>::AddPosition(pos_t pos) {
    positions_.PushBack(pos);
    cur_tf_++;
}

template <bool REALTIME>
void MemoryPosting<REALTIME>::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    term_freqs_.PushBack(cur_tf_);
    doc_ids_.PushBack(doc_id);
    cur_tf_ = 0;
}

template class MemoryPosting<true>;
template class MemoryPosting<false>;
} // namespace infinity