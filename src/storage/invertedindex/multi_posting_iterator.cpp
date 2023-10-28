module;

import stl;
import posting_iterator;
import index_defines;
import term_meta;

module multi_posting_iterator;

#include "common/utility/builtin.h"

namespace infinity {

void MultiPostingIterator::Init(Vector<Pair<i64, SharedPtr<PostingIterator>>> posting_iterators, Vector<u32> segment_ids) {
    posting_iterators_ = posting_iterators;
    size_ = posting_iterators.size();
}

docid_t MultiPostingIterator::SeekDoc(docid_t doc_id) {
    if (unlikely(cursor_ >= size_)) {
        return INVALID_DOCID;
    }
    while (cursor_ < size_) {
        if (cursor_ + 1 < size_ && posting_iterators_[cursor_ + 1].first <= doc_id) {
            ++cursor_;
            continue;
        }

        docid_t local_doc_id = doc_id - posting_iterators_[cursor_].first;
        docid_t doc_id = posting_iterators_[cursor_].second->SeekDoc(local_doc_id);
        if (doc_id != INVALID_DOCID) {
            return posting_iterators_[cursor_].first + doc_id;
        }
        cursor_++;
    }
    return INVALID_DOCID;
}

void MultiPostingIterator::Reset() {
    for (u32 i = 0; i < posting_iterators_.size(); ++i) {
        posting_iterators_[i].second->Reset();
    }
    cursor_ = 0;
}
} // namespace infinity