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
import third_party;
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

    void Commit();

    SharedPtr<IndexSegmentReader> CreateInMemSegmentReader();

private:
    InvertedIndexConfig index_config_;
    String directory_;
    Vector<u64> column_ids_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    FlatHashMap<u64, UniquePtr<ColumnIndexer>, detail::Hash<u64>> column_indexers_;
};
} // namespace infinity
