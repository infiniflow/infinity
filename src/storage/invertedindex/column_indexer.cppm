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
import memory_posting;
import third_party;

export module column_indexer;

namespace vespalib::alloc {
class MemoryPoolAllocator;
}

namespace infinity {
export class ColumnIndexer {
public:
    using TermKey = String;
    using PostingPtr = MemoryPosting<false> *;
    using RTPostingPtr = MemoryPosting<true> *;
    using PostingTable = Btree<TermKey, PostingPtr>;
    using RTPostingTable = Btree<TermKey, RTPostingPtr>;

    struct KeyComp {
        bool operator()(const String &lhs, const String &rhs) const;
        // bool operator()(const TermKey &lhs, const TermKey &rhs) const;
    };

    enum IndexMode {
        REAL_TIME,
        NEAR_REAL_TIME,
        OFFLINE,
    };

    ColumnIndexer(u64 column_id, const InvertedIndexConfig &index_config, SharedPtr<MemoryPool> byte_slice_pool, SharedPtr<RecyclePool> buffer_pool);

    ~ColumnIndexer();

    void SetIndexMode(IndexMode index_mode);

    bool IsRealTime() { return index_mode_ == REAL_TIME; }
    // realtime insert
    void Insert(RowID row_id, String &data);

    void Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    void Commit();

    Analyzer *GetAnalyzer() { return analyzer_.get(); }

    bool IsJiebaSpecialize() { return jieba_specialize_; }

    MemoryPool *GetPool() { return byte_slice_pool_.get(); }

    PostingTable *GetPostingTable() { return posting_store_.get(); }

    RTPostingTable *GetRTPostingTable() { return rt_posting_store_.get(); }

    PostingPtr GetOrAddPosting(const TermKey &term);

    RTPostingPtr GetOrAddRTPosting(const TermKey &term);

private:
    void SetAnalyzer();

    void SwitchActiveInverter();

private:
    IndexMode index_mode_{NEAR_REAL_TIME};
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    SharedPtr<vespalib::alloc::MemoryPoolAllocator> memory_allocator_;
    UniquePtr<PostingTable> posting_store_;
    UniquePtr<RTPostingTable> rt_posting_store_;
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};
    Vector<UniquePtr<ColumnInverter>> free_inverters_;
    Deque<UniquePtr<ColumnInverter>> inflight_inverters_;
    UniquePtr<ColumnInverter> inverter_;
    u32 num_inverters_;
    u32 max_inverters_;
    UniquePtr<SequencedTaskExecutor> invert_executor_;
    UniquePtr<SequencedTaskExecutor> commit_executor_;
};
} // namespace infinity
