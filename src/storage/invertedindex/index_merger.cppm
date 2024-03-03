module;

export module index_merger;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_config;
import posting_writer;
import segment;
import column_index_merger;

namespace infinity {
export class IndexMerger {
public:
    IndexMerger(const InvertedIndexConfig &index_config, const Vector<u64> &column_ids, MemoryPool *byte_slice_pool, RecyclePool *buffer_pool);

    ~IndexMerger();

    void Merge(const Vector<Segment> &source_segments, const Segment &target_segment);

private:
    InvertedIndexConfig index_config_;
    Vector<u64> column_ids_;
    Vector<SharedPtr<ColumnIndexMerger>> column_index_mergers_;
    MemoryPool *byte_slice_pool_{nullptr};
    RecyclePool *buffer_pool_{nullptr};
};
}; // namespace infinity