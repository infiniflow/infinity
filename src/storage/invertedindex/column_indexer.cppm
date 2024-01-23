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
import column_inverter;
import task_executor;
export module column_indexer;

namespace infinity {
export class ColumnIndexer {
    using PostingTable = HashMap<String, PostingWriter *>;

    struct KeyComp {
        bool operator()(const String &lhs, const String &rhs) const;
    };

public:
    ColumnIndexer(u64 column_id, const InvertedIndexConfig &index_config, SharedPtr<MemoryPool> byte_slice_pool, SharedPtr<RecyclePool> buffer_pool);
    ~ColumnIndexer();

    void Add(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    void Commit();

    Analyzer *GetAnalyzer() { return analyzer_.get(); }

    bool IsJiebaSpecialize() { return jieba_specialize_; }

    MemoryPool *GetPool() { return byte_slice_pool_.get(); }

private:
    void SetAnalyzer();

    PostingWriter *DoAddPosting(const String &term);

    void SwitchActiveInverter();

private:
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    PostingTable *posting_table_{nullptr};
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};
    bool is_real_time_{false};
    Vector<UniquePtr<ColumnInverter>> free_inverters_;
    Deque<UniquePtr<ColumnInverter>> inflight_inverters_;
    UniquePtr<ColumnInverter> inverter_;
    u32 num_inverters_;
    u32 max_inverters_;
    UniquePtr<SequencedTaskExecutor> invert_executor_;
    UniquePtr<SequencedTaskExecutor> commit_executor_;
};
} // namespace infinity
