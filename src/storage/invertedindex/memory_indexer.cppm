module;

export module memory_indexer;
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

import column_vector;
import analyzer;
import sequential_column_inverter;
import parallel_column_inverter;
import task_executor;
import memory_posting;
import third_party;
import index_builder;
import data_type;

namespace vespalib::alloc {
class MemoryPoolAllocator;
}

namespace infinity {
class Indexer;
export class MemoryIndexer {
public:
    using TermKey = String;
    // using PostingPtr = MemoryPosting<false> *;
    using PostingPtr = SharedPtr<PostingWriter>;
    using RTPostingPtr = SharedPtr<MemoryPosting<true>>;
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

    MemoryIndexer(Indexer *indexer,
                  u64 column_id,
                  const InvertedIndexConfig &index_config,
                  SharedPtr<MemoryPool> byte_slice_pool,
                  SharedPtr<RecyclePool> buffer_pool);

    ~MemoryIndexer();

    void SetIndexMode(IndexMode index_mode);

    bool IsRealTime() { return index_mode_ == REAL_TIME; }
    // realtime insert
    void Insert(RowID row_id, String &data);

    void Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    void Commit();

    bool NeedDump();

    void Dump(IndexBuilder &index_builder);

    Analyzer *GetAnalyzer() { return analyzer_.get(); }

    bool IsJiebaSpecialize() { return jieba_specialize_; }

    MemoryPool *GetPool() { return byte_slice_pool_.get(); }

    PostingTable *GetPostingTable() { return posting_store_.get(); }

    RTPostingTable *GetRTPostingTable() { return rt_posting_store_.get(); }

    PostingPtr GetOrAddPosting(const TermKey &term);

    RTPostingPtr GetOrAddRTPosting(const TermKey &term);

    void ReclaimMemory();

    void Reset();

private:
    void SetAnalyzer();

    void SwitchActiveInverter();

private:
    Indexer *indexer_{nullptr};
    IndexMode index_mode_{NEAR_REAL_TIME};
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    SharedPtr<vespalib::alloc::MemoryPoolAllocator> memory_allocator_;
    GenerationHandler generation_handler_;
    UniquePtr<PostingTable> posting_store_;
    UniquePtr<RTPostingTable> rt_posting_store_;
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};
    Vector<UniquePtr<SequentialColumnInverter>> free_inverters_;
    Deque<UniquePtr<SequentialColumnInverter>> inflight_inverters_;
    UniquePtr<SequentialColumnInverter> inverter_;
    u32 num_inverters_;
    u32 max_inverters_;
    UniquePtr<SequencedTaskExecutor> invert_executor_;
    UniquePtr<SequencedTaskExecutor> commit_executor_;
};
} // namespace infinity
