module;

#include <vector>

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import segment;
import disk_index_segment_reader;
import inmem_index_segment_reader;
import indexer;
import column_indexer;
import dict_reader;
module column_index_reader;

namespace infinity {
ColumnIndexReader::ColumnIndexReader(u64 column_id, Indexer *indexer) : column_id_(column_id), indexer_(indexer) {}

void ColumnIndexReader::Open(const InvertedIndexConfig &index_config) {
    index_config_ = index_config;
    root_dir_ = index_config.GetIndexName();
    Vector<Segment> segments;
    indexer_->GetSegments(segments);
    for (auto &segment : segments) {
        if (segment.GetSegmentStatus() == Segment::BUILT) {
            SharedPtr<DiskIndexSegmentReader> segment_reader = CreateDiskSegmentReader(segment);
            segment_readers_.push_back(segment_reader);
            base_doc_ids_.push_back(segment.GetBaseDocId());
        } else {
            SharedPtr<InMemIndexSegmentReader> segment_reader = CreateInMemSegmentReader(segment);
            segment_readers_.push_back(segment_reader);
            base_doc_ids_.push_back(segment.GetBaseDocId());
        }
    }
}

SharedPtr<DiskIndexSegmentReader> ColumnIndexReader::CreateDiskSegmentReader(const Segment &segment) {
    return MakeShared<DiskIndexSegmentReader>(column_id_, segment, index_config_);
}

SharedPtr<InMemIndexSegmentReader> ColumnIndexReader::CreateInMemSegmentReader(Segment &segment) {
    ColumnIndexer *column_Indexer = indexer_->GetColumnIndexer(column_id_);
    return MakeShared<InMemIndexSegmentReader>(column_Indexer->GetMemoryIndexer());
}

PostingIterator *ColumnIndexReader::Lookup(const String &term, MemoryPool *session_pool) {
    SharedPtr<Vector<SegmentPosting>> seg_postings = MakeShared<Vector<SegmentPosting>>();
    for (u32 i = 0; i < segment_readers_.size(); ++i) {
        SegmentPosting seg_posting;
        auto ret = segment_readers_[i]->GetSegmentPosting(term, base_doc_ids_[i], seg_posting, session_pool);
        if (ret) {
            seg_postings->push_back(seg_posting);
        }
    }
    PostingIterator *iter =
        new ((session_pool)->Allocate(sizeof(PostingIterator))) PostingIterator(index_config_.GetPostingFormatOption(), session_pool);
    u32 state_pool_size = 0; // TODO
    iter->Init(seg_postings, state_pool_size);
    return iter;
}
} // namespace infinity
