module;

module segment_term_posting;

import stl;
import file_writer;
import index_defines;
import index_config;
import posting_decoder;
import term_meta;
import segment;
import column_index_iterator;

namespace infinity {

SegmentTermPosting::SegmentTermPosting(segmentid_t segment_id, docid_t base_doc) : segment_id_(segment_id), base_doc_id_(base_doc) {}

bool SegmentTermPosting::HasNext() {
    if (column_index_iterator_->Next(term_, posting_decoder_)) {
        return true;
    }
    return false;
}

SegmentTermPostingQueue::SegmentTermPostingQueue(const InvertedIndexConfig &index_config, u64 column_id)
    : index_config_(index_config), column_id_(column_id) {}

SegmentTermPostingQueue::~SegmentTermPostingQueue() {
    while (!segment_term_postings_.empty()) {
        SegmentTermPosting *segment_term_posting = segment_term_postings_.top();
        segment_term_postings_.pop();
        delete segment_term_posting;
    }
    for (u32 i = 0; i < merging_term_postings_.size(); ++i) {
        delete merging_term_postings_[i];
    }
}

void SegmentTermPostingQueue::Init(const Vector<Segment> &segments) {
    for (u32 i = 0; i < segments.size(); ++i) {
        SegmentTermPosting *segment_term_posting = new SegmentTermPosting(segments[i].GetSegmentId(), 0);
        segment_term_posting->column_index_iterator_ = CreateIndexIterator(segments[i].GetSegmentId());
        if (segment_term_posting->HasNext()) {
            segment_term_postings_.push(segment_term_posting);
        } else
            delete segment_term_posting;
    }
}

SharedPtr<ColumnIndexIterator> SegmentTermPostingQueue::CreateIndexIterator(segmentid_t segment_id) {
    SharedPtr<ColumnIndexIterator> index_iterator = MakeShared<ColumnIndexIterator>(index_config_, column_id_);
    index_iterator->Init(segment_id);
    return index_iterator;
}

const Vector<SegmentTermPosting *> &SegmentTermPostingQueue::GetCurrentMerging(String &term) {
    SegmentTermPosting *term_posting = segment_term_postings_.top();
    term = term_posting->term_;
    segment_term_postings_.pop();
    merging_term_postings_.push_back(term_posting);
    term_posting = segment_term_postings_.size() > 0 ? segment_term_postings_.top() : nullptr;
    while (term_posting != nullptr && (term_posting->term_.compare(term) == 0)) {
        merging_term_postings_.push_back(term_posting);
        segment_term_postings_.pop();
        term_posting = segment_term_postings_.size() > 0 ? segment_term_postings_.top() : nullptr;
    }
    return merging_term_postings_;
}

void SegmentTermPostingQueue::MoveToNextTerm() {
    for (u32 i = 0; i < merging_term_postings_.size(); ++i) {
        SegmentTermPosting *term_posting = merging_term_postings_[i];
        merging_term_postings_[i] = nullptr;
        if (term_posting->HasNext()) {
            segment_term_postings_.push(term_posting);
        } else {
            delete term_posting;
        }
    }
    merging_term_postings_.clear();
}

} // namespace infinity