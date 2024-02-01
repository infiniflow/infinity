module;

module column_indexer;

import stl;
import memory_pool;
import index_defines;
import index_config;
import parser;
import column_vector;
import third_party;
import index_builder;

namespace infinity {

ColumnIndexer::ColumnIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool) {}

ColumnIndexer::~ColumnIndexer() {}

} // namespace infinity
