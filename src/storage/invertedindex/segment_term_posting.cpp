module;

module segment_term_posting;

import stl;
import memory_pool;
import file_writer;
import doc_list_encoder;
import inmem_posting_decoder;
import pos_list_encoder;
import posting_list_format;
import index_defines;
import index_config;
import posting_writer;
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
}

void SegmentTermPostingQueue::Init(const Vector<Segment> &segments) {
    for (u32 i = 0; i < segments.size(); ++i) {
        SharedPtr<ColumnIndexIterator> index_iterator = CreateIndexIterator(segments[i].GetSegmentId());
        SegmentTermPosting *segment_term_posting = new SegmentTermPosting(segments[i].GetSegmentId(), 0);
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

} // namespace infinity