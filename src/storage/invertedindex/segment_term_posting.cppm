module;

export module segment_term_posting;

import stl;
import file_writer;
import posting_decoder;
import index_defines;
import term_meta;
import column_index_iterator;
import index_defines;
import internal_types;

namespace infinity {
// Utility class for posting merging
export class SegmentTermPosting {
public:
    SegmentTermPosting();

    SegmentTermPosting(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag);

    RowID GetBaseRowId() const { return base_row_id_; }

    bool HasNext();

    PostingDecoder *GetPostingDecoder() { return posting_decoder_; }

public:
    RowID base_row_id_{};
    String term_{};
    PostingDecoder *posting_decoder_{nullptr};
    SharedPtr<ColumnIndexIterator> column_index_iterator_{};
};

class SegmentTermPostingComparator {
public:
    bool operator()(const SegmentTermPosting *item1, const SegmentTermPosting *item2) {
        int ret = item1->term_.compare(item2->term_);
        if (ret != 0)
            return ret > 0;
        return item1->base_row_id_ > item2->base_row_id_;
    }
};

export class SegmentTermPostingQueue {
public:
    SegmentTermPostingQueue(const String &index_dir, const Vector<String> &base_names, const Vector<RowID> &base_rowids, optionflag_t flag);

    ~SegmentTermPostingQueue();

    bool Empty() const { return segment_term_postings_.empty(); }

    const Vector<SegmentTermPosting *> &GetCurrentMerging(String &term);

    void MoveToNextTerm();

private:
    using PriorityQueue = Heap<SegmentTermPosting *, SegmentTermPostingComparator>;
    const String &index_dir_;
    const Vector<String> &base_names_;
    const Vector<RowID> &base_rowids_;

    PriorityQueue segment_term_postings_;
    Vector<SegmentTermPosting *> merging_term_postings_;
};

} // namespace infinity