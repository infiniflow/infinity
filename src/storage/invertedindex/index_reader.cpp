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
    Vector<SharedPtr<IndexSegmentReader>> segment_readers;
    for (auto &segment : segments) {
        if (segment.GetSegmentStatus() == Segment::BUILT) {
            SharedPtr<DiskIndexSegmentReader> segment_reader = CreateDiskSegmentReader(segment);
            segment_readers.push_back(segment_reader);
        } else {
            SharedPtr<IndexSegmentReader> segment_reader = CreateInMemSegmentReader(segment);
            segment_readers.push_back(segment_reader);
        }
    }
}

void IndexReader::GetSegments(const String &directory, Vector<Segment> &segments) {}

SharedPtr<DiskIndexSegmentReader> IndexReader::CreateDiskSegmentReader(const Segment &segment) {
    // dict_reader TODO
    SharedPtr<DictionaryReader> dict_reader = MakeShared<DictionaryReader>(root_dir_);
    return MakeShared<DiskIndexSegmentReader>(root_dir_, segment, index_config_, dict_reader);
}

SharedPtr<IndexSegmentReader> IndexReader::CreateInMemSegmentReader(Segment &segment) {
    SharedPtr<Indexer> index_writer = segment.GetIndexWriter();
    return index_writer->CreateInMemSegmentReader();
}

} // namespace infinity
