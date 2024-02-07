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
import data_block;
import column_indexer;
import third_party;
import segment;
import internal_types;
export module indexer;

namespace infinity {

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

    void Add(DataBlock *data_block);

    void Insert(RowID row_id, String &data);

    void Commit();

    void TryDump();

    void Dump();

    SharedPtr<InMemIndexSegmentReader> CreateInMemSegmentReader(u64 column_id);

    String GetDirectory() { return directory_; }

    void GetSegments(Vector<Segment> &segments);

    segmentid_t GetNextSegmentID() { return id_generator_->GetNextSegmentID(); }

    ColumnIndexer *GetColumnIndexer(u64 column_id) { return column_indexers_[column_id].get(); }

private:
    void UpdateSegment(RowID row_id) {
        if (segments_.back().GetBaseDocId() == INVALID_DOCID) {
            segments_.back().SetBaseDocId(RowID2DocID(row_id));
        }
    }

    InvertedIndexConfig index_config_;
    String directory_;
    Vector<u64> column_ids_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    FlatHashMap<u64, UniquePtr<ColumnIndexer>, detail::Hash<u64>> column_indexers_;
    SharedPtr<IDGenerator> id_generator_;
    Vector<Segment> segments_;
    Atomic<u32> dump_ref_count_;
};
} // namespace infinity
