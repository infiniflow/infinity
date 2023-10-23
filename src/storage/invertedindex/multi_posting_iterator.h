#pragma once

#include "posting_iterator.h"

#include <vector>

namespace infinity {
class MultiPostingIterator {
public:
    MultiPostingIterator() : cursor_(0) {}

    ~MultiPostingIterator() {}

    void Init(std::vector<std::pair<int64_t, std::shared_ptr<PostingIterator>>> posting_iterators, std::vector<uint32_t> segment_ids);

    docid_t SeekDoc(docid_t doc_id);

    TermMeta *GetTermMeta() const { return posting_iterators_[cursor_].second->GetTermMeta(); }

    void Reset();

private:
    std::vector<std::pair<int64_t, std::shared_ptr<PostingIterator>>> posting_iterators_;
    int64_t cursor_;
    int64_t size_;
};
} // namespace infinity