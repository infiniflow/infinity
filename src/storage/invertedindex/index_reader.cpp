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
import dict_reader;
module index_reader;

namespace infinity {
void IndexReader::Open(const InvertedIndexConfig &index_config) {
    index_config_ = index_config;
    root_dir_ = index_config.GetIndexName();
    Vector<Segment> segments;
    GetSegments(index_config.GetIndexName(), segments);
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

void IndexReader::GetSegments(const String &directory, Vector<Segment> &segments) {}

SharedPtr<DiskIndexSegmentReader> IndexReader::CreateDiskSegmentReader(const Segment &segment) {
    // dict_reader TODO
    SharedPtr<DictionaryReader> dict_reader = MakeShared<DictionaryReader>(root_dir_);
    return MakeShared<DiskIndexSegmentReader>(root_dir_, segment, index_config_, dict_reader);
}

SharedPtr<InMemIndexSegmentReader> IndexReader::CreateInMemSegmentReader(Segment &segment) {
    SharedPtr<Indexer> index_writer = segment.GetIndexWriter();
    return index_writer->CreateInMemSegmentReader(segment.GetColumnID());
}

PostingIterator *IndexReader::Lookup(const String &term, MemoryPool *session_pool) {
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
