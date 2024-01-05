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
import analyzer_pool;
import term;
module column_indexer;

namespace infinity {

ColumnIndexer::ColumnIndexer(u64 column_id,
                             const InvertedIndexConfig &index_config,
                             SharedPtr<MemoryPool> byte_slice_pool,
                             SharedPtr<RecyclePool> buffer_pool)
    : column_id_(column_id), index_config_(index_config), byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool) {
    posting_table_ = new PostingTable;
    SetAnalyzer();
}

ColumnIndexer::~ColumnIndexer() {
    if (posting_table_) {
        for (PostingTable::iterator it = posting_table_->begin(); it != posting_table_->end(); ++it) {
            PostingWriter *posting_writer = it->second;
            posting_writer->~PostingWriter();
            buffer_pool_->Deallocate((void *)posting_writer, sizeof(PostingWriter));
        }
        posting_table_->clear();
    }
}

void ColumnIndexer::SetAnalyzer() {
    String analyzer = index_config_.GetAnalyzer(column_id_);
    analyzer_ = AnalyzerPool::instance().Get(analyzer);
    jieba_specialize_ = analyzer.compare("chinese") == 0 ? true : false;
}

void ColumnIndexer::Add(SharedPtr<ColumnVector> column_vector, Vector<RowID> &row_ids) {
    for (SizeT i = 0; i < column_vector->Size(); ++i) {
        String data = column_vector->ToString(i);
        TermList results;
        analyzer_->Analyze(data, results, jieba_specialize_);
        for (SizeT i = 0; i < results.size(); ++i) {
            if (is_real_time_) {
                PostingWriter *posting_writer = DoAddPosting(results[i].text_);
                /// TODO
                posting_writer->AddPosition(results[i].word_offset_);
            } else {
            }
        }
    }
}

PostingWriter *ColumnIndexer::DoAddPosting(const String &term) {
    PostingTable::iterator it = posting_table_->find(term);
    if (it == posting_table_->end()) {
        PostingWriter *posting_writer = new (buffer_pool_->Allocate(sizeof(PostingWriter)))
            PostingWriter(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        posting_table_->emplace(term, posting_writer);
        return posting_writer;
    }
    return it->second;
}

} // namespace infinity