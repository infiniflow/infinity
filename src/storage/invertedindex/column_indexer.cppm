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
import parser;
import column_vector;
import analyzer;
export module column_indexer;

namespace infinity {
export class ColumnIndexer {
    using PostingTable = HashMap<String, PostingWriter *>;

public:
    ColumnIndexer(u64 column_id, const InvertedIndexConfig &index_config, SharedPtr<MemoryPool> byte_slice_pool, SharedPtr<RecyclePool> buffer_pool);
    ~ColumnIndexer();

    void Add(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

private:
    void SetAnalyzer();

    PostingWriter *DoAddPosting(const String &term);

private:
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    PostingTable *posting_table_{nullptr};
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};
    bool is_real_time_{false};
};
} // namespace infinity
