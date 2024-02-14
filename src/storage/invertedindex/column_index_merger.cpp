module;

module column_index_merger;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import column_index_iterator;
import segment_term_posting;
namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id, MemoryPool *memory_pool, RecyclePool *buffer_pool)
    : index_config_(index_config), column_id_(column_id), memory_pool_(memory_pool), buffer_pool_(buffer_pool) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

SharedPtr<PostingMerger> ColumnIndexMerger::CreatePostingMerger(const Vector<Segment> &segments) {
    return MakeShared<PostingMerger>(memory_pool_, buffer_pool_, segments);
}

void ColumnIndexMerger::Merge(const Vector<Segment> &segments) {
    SegmentTermPostingQueue term_posting_queue(index_config_, column_id_);
    term_posting_queue.Init(segments);
    String term;
    while (!term_posting_queue.Empty()) {
        const Vector<SegmentTermPosting *> &merging_term_postings = term_posting_queue.GetCurrentMerging(term);
        MergeTerm(term, merging_term_postings, segments);
        term_posting_queue.MoveToNextTerm();
    }
}

void ColumnIndexMerger::MergeTerm(const String &term, const Vector<SegmentTermPosting *> &merging_term_postings, const Vector<Segment> &segments) {
    SharedPtr<PostingMerger> posting_merger = CreatePostingMerger(segments);
    posting_merger->Merge(merging_term_postings);
    memory_pool_->Reset();
    buffer_pool_->Reset();
}

} // namespace infinity