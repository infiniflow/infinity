module;

export module column_index_merger;

import stl;
import memory_pool;
import file_writer;
import posting_decoder;
import posting_merger;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import column_index_iterator;
import segment_term_posting;
import local_file_system;

namespace infinity {
export class ColumnIndexMerger {
public:
    ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id, MemoryPool *memory_pool, RecyclePool *buffer_pool);
    ~ColumnIndexMerger();

    void Merge(const Vector<Segment> &source_segments, const Segment &target_segment);

private:
    SharedPtr<PostingMerger> CreatePostingMerger(const Segment &target_segment);

    void MergeTerm(const String &term, TermMeta &term_meta, const Vector<SegmentTermPosting *> &merging_term_postings, const Segment &target_segment);

    InvertedIndexConfig index_config_;
    u64 column_id_;
    MemoryPool *memory_pool_{nullptr};
    RecyclePool *buffer_pool_{nullptr};
    SharedPtr<FileWriter> posting_file_;
    LocalFileSystem fs_;
};
} // namespace infinity