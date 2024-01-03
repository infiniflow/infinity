module;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import index_segment_reader;
import posting_writer;
import data_block;
import column_indexer;
export module indexer;

namespace infinity {
export class Indexer {
public:
    using PostingTable = HashMap<String, PostingWriter *>;

    Indexer();

    ~Indexer();

    void Open(const InvertedIndexConfig &index_config, const String &directory);

    void Add(DataBlock *data_block);

    void Commit();

    SharedPtr<IndexSegmentReader> CreateInMemSegmentReader();

private:
    void DoAddPosting(const String &term);

private:
    InvertedIndexConfig index_config_;
    String directory_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    PostingTable *posting_table_{nullptr};
};
} // namespace infinity
