module;

module segment_term_posting;

import stl;
import file_writer;
import index_defines;
import posting_decoder;
import term_meta;
import column_index_iterator;

namespace infinity {

SegmentTermPosting::SegmentTermPosting(docid_t base_doc_id) : base_doc_id_(base_doc_id) {}

bool SegmentTermPosting::HasNext() {
    if (column_index_iterator_->Next(term_, posting_decoder_)) {
        return true;
    }
    return false;
}

SegmentTermPostingQueue::SegmentTermPostingQueue(const String &index_dir,
                                                 const Vector<String> &base_names,
                                                 const Vector<docid_t> &base_docids,
                                                 optionflag_t flag)
    : index_dir_(index_dir), base_names_(base_names), base_docids_(base_docids) {
    for (u32 i = 0; i < base_names.size(); ++i) {
        SegmentTermPosting *segment_term_posting = new SegmentTermPosting(base_docids[i]);
        segment_term_posting->column_index_iterator_ = CreateIndexIterator(index_dir, base_names[i], flag);
        if (segment_term_posting->HasNext()) {
            segment_term_postings_.push(segment_term_posting);
        } else
            delete segment_term_posting;
    }
}

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

SharedPtr<ColumnIndexIterator> SegmentTermPostingQueue::CreateIndexIterator(const String &index_dir, const String &base_name, optionflag_t flag) {
    SharedPtr<ColumnIndexIterator> index_iterator = MakeShared<ColumnIndexIterator>(index_dir, base_name, flag);
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