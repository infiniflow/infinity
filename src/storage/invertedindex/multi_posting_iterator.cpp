#include "multi_posting_iterator.h"
#include "common/utility/builtin.h"
#include "index_defines.h"

namespace infinity {

void MultiPostingIterator::Init(std::vector<std::pair<int64_t, std::shared_ptr<PostingIterator>>> posting_iterators,
                                std::vector<uint32_t> segment_ids) {
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
    for (auto &posting_iterator : posting_iterators_) {
        posting_iterator.second->Reset();
    }
    cursor_ = 0;
}
} // namespace infinity