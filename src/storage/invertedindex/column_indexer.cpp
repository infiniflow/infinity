module;

module column_indexer;

import stl;
import memory_pool;
import index_defines;
import index_config;

import column_vector;
import third_party;
import index_builder;
import indexer;
import data_type;

namespace infinity {

ColumnIndexer::ColumnIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool) {
    active_memory_indexer_ = MakeUnique<MemoryIndexer>(indexer, column_id, index_config, byte_slice_pool, buffer_pool);
}

ColumnIndexer::~ColumnIndexer() {}

void ColumnIndexer::Insert(RowID row_id, String &data) { active_memory_indexer_->Insert(row_id, data); }

void ColumnIndexer::Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) { active_memory_indexer_->Insert(column_vector, row_ids); }

void ColumnIndexer::Commit() { active_memory_indexer_->Commit(); }

void ColumnIndexer::Dump(IndexBuilder &index_builder) { active_memory_indexer_->Dump(index_builder); }

} // namespace infinity
