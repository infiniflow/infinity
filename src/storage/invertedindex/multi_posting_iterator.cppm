module;

import stl;
import posting_iterator;
import index_defines;
import term_meta;
export module multi_posting_iterator;

namespace infinity {
export class MultiPostingIterator {
public:
    MultiPostingIterator() : cursor_(0) {}

    ~MultiPostingIterator() {}

    void Init(Vector<Pair<i64, SharedPtr<PostingIterator>>> posting_iterators, Vector<u32> segment_ids);

    docid_t SeekDoc(docid_t doc_id);

    TermMeta *GetTermMeta() const { return posting_iterators_[cursor_].second->GetTermMeta(); }

    void Reset();

private:
    Vector<Pair<i64, SharedPtr<PostingIterator>>> posting_iterators_;
    i64 cursor_;
    i64 size_;
};
} // namespace infinity