module;

export module column_indexer;
import stl;
import memory_pool;
import index_defines;
import index_config;
import data_block;
import parser;
import column_vector;
import third_party;
import memory_indexer;
import segment;

namespace infinity {

class Indexer;
export class ColumnIndexer {
public:
    ColumnIndexer(Indexer *indexer,
                  u64 column_id,
                  const InvertedIndexConfig &index_config,
                  SharedPtr<MemoryPool> byte_slice_pool,
                  SharedPtr<RecyclePool> buffer_pool);

    ~ColumnIndexer();

    MemoryIndexer *GetMemoryIndexer() { return active_memory_indexer_.get(); }
    // realtime insert
    void Insert(RowID row_id, String &data);

    void Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids);

    void Commit();

    void Dump();

    void GetSegments(Vector<Segment> &segments) { segments = segments_; }

private:
    UniquePtr<MemoryIndexer> active_memory_indexer_;
    String index_name_;
    Vector<Segment> segments_;
    u32 current_segment_id_{0};
};

} // namespace infinity
