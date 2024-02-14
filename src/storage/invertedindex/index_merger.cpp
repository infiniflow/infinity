module;

module index_merger;

import stl;
import memory_pool;
import index_defines;
import index_config;
import posting_writer;
import segment;
import column_index_merger;

namespace infinity {

IndexMerger::IndexMerger(const InvertedIndexConfig &index_config,
                         const Vector<u64> &column_ids,
                         MemoryPool *byte_slice_pool,
                         RecyclePool *buffer_pool)
    : index_config_(index_config), column_ids_(column_ids), byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool) {
    for (u32 i = 0; i < column_ids_.size(); ++i) {
        SharedPtr<ColumnIndexMerger> column_index_merger =
            MakeShared<ColumnIndexMerger>(index_config_, column_ids_[i], byte_slice_pool_, buffer_pool_);
        column_index_mergers_.push_back(column_index_merger);
    }
}

IndexMerger::~IndexMerger() {}

void IndexMerger::Merge(const Vector<Segment> &source_segments, const Segment &target_segment) {
    for (u32 i = 0; i < column_index_mergers_.size(); ++i) {
        column_index_mergers_[i]->Merge(source_segments, target_segment);
    }
}

} // namespace infinity