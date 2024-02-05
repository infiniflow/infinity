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
import local_file_system;
import file_writer;
import posting_writer;
import term_meta;

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

void ColumnIndexer::Dump() {
    Path path = Path(index_name_) / std::to_string(current_segment_id_);
    String index_prefix = path.string();
    LocalFileSystem fs;
    String posting_file = index_prefix + ".pos";
    SharedPtr<FileWriter> posting_file_writer = MakeShared<FileWriter>(fs, posting_file, 128);
    String dict_file = index_prefix + ".dic";
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs, posting_file, 128);
    MemoryIndexer::PostingTable *posting_table = active_memory_indexer_->GetPostingTable();
    TermMetaDumper term_meta_dumpler(active_memory_indexer_->index_config_.GetPostingFormatOption());

    if (posting_table) {
        for (auto it = posting_table->begin(); it.valid(); ++it) {
            const MemoryIndexer::PostingPtr posting_writer = it.getData();
            TermMeta term_meta(posting_writer->GetDF(), posting_writer->GetTotalTF());
            posting_writer->Write(posting_file_writer, term_meta);
            /// TODO dict writer
            term_meta_dumpler.Dump(dict_file_writer, term_meta);
        }
    }
    active_memory_indexer_->Reset();
}

} // namespace infinity
