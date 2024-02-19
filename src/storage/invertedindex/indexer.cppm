module;

import stl;
import memory_pool;
import segment_posting;
import posting_iterator;
import index_defines;
import index_config;
import index_segment_reader;
import inmem_index_segment_reader;
import posting_writer;
import block_entry;
import buffer_manager;
import column_indexer;
import third_party;
import segment;
import internal_types;
export module indexer;

namespace infinity {

struct TableEntry;

namespace detail {
template <class T>
struct Hash {
    inline SizeT operator()(const T &val) const { return val; }
};
} // namespace detail

export class Indexer {
public:
    Indexer();

    ~Indexer();

    void Open(const InvertedIndexConfig &index_config, const String &directory);

    void BatchInsert(const BlockEntry *block_entry, u32 row_offset, u32 row_count, BufferManager *buffer_mgr);

    void Insert(RowID row_id, String &data);

    void Commit();

    void TryDump();

    void Dump();

    SharedPtr<InMemIndexSegmentReader> CreateInMemSegmentReader(u64 column_id);

    String GetDirectory() { return directory_; }

    void GetSegments(Vector<Segment> &segments);

    segmentid_t GetNextSegmentID() { return id_generator_->GetNextSegmentID(); }

    ColumnIndexer *GetColumnIndexer(u64 column_id) { return column_indexers_[column_id].get(); }

    std::shared_mutex &GetMutex() { return flush_mutex_; }

private:
    void AddSegment();

    void UpdateSegment(RowID row_id, u64 inc_count = 1);

    InvertedIndexConfig index_config_;
    String directory_;
    Vector<u64> column_ids_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    FlatHashMap<u64, UniquePtr<ColumnIndexer>, detail::Hash<u64>> column_indexers_;
    SharedPtr<IDGenerator> id_generator_;
    Vector<Segment> segments_;
    Atomic<Segment *> active_segment_;
    Atomic<u32> dump_ref_count_;
    mutable std::shared_mutex flush_mutex_;
};
} // namespace infinity
