module;

#include <filesystem>
#include <string>

module column_indexer;

import stl;
import memory_pool;
import index_defines;
import index_config;
import parser;
import column_vector;
import third_party;
import indexer;

namespace infinity {

ColumnIndexer::ColumnIndexer(Indexer *indexer,
                             u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool) {
    active_memory_indexer_ = MakeUnique<MemoryIndexer>(indexer, this, column_id, index_config, byte_slice_pool, buffer_pool);
    index_name_ = indexer->GetDirectory();
    Path path = Path(index_name_) / std::to_string(column_id);
    index_name_ = path.string();
    std::error_code ec;
    bool path_exists = std::filesystem::exists(path);
    if (!path_exists) {
        std::filesystem::create_directories(path, ec);
    }
}

ColumnIndexer::~ColumnIndexer() {}

void ColumnIndexer::Insert(RowID row_id, String &data) { active_memory_indexer_->Insert(row_id, data); }

void ColumnIndexer::Insert(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) { active_memory_indexer_->Insert(column_vector, row_ids); }

void ColumnIndexer::Commit() { active_memory_indexer_->Commit(); }

void ColumnIndexer::Dump() {}

} // namespace infinity
