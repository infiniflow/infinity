export module infinity_core:segment_term_posting;

import :file_writer;
import :posting_decoder;
import :index_defines;
import :term_meta;
import :column_index_iterator;
import :index_defines;

import internal_types;

namespace infinity {
// Utility class for posting merging
export class SegmentTermPosting {
public:
    SegmentTermPosting();

    SegmentTermPosting(const std::string &index_dir, const std::string &base_name, RowID base_row_id, optionflag_t flag);

    RowID GetBeginRowID() const { return base_row_id_; }

    bool HasNext();

    PostingDecoder *GetPostingDecoder() { return posting_decoder_; }

public:
    RowID base_row_id_{};
    std::string term_{};
    PostingDecoder *posting_decoder_{nullptr};
    std::shared_ptr<ColumnIndexIterator> column_index_iterator_{};
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
    SegmentTermPostingQueue(const std::string &index_dir,
                            const std::vector<std::string> &base_names,
                            const std::vector<RowID> &base_rowids,
                            optionflag_t flag);

    ~SegmentTermPostingQueue();

    bool Empty() const { return segment_term_postings_.empty(); }

    const std::vector<SegmentTermPosting *> &GetCurrentMerging(std::string &term);

    void MoveToNextTerm();

private:
    using PriorityQueue = std::priority_queue<SegmentTermPosting *, std::vector<SegmentTermPosting *>, SegmentTermPostingComparator>;
    const std::string &index_dir_;
    const std::vector<std::string> &base_names_;
    const std::vector<RowID> &base_rowids_;

    PriorityQueue segment_term_postings_;
    std::vector<SegmentTermPosting *> merging_term_postings_;
};

} // namespace infinity