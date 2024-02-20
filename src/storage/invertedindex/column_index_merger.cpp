module;

#include <fstream>
#include <string>

module column_index_merger;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import column_index_iterator;
import segment_term_posting;
import fst;

namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id, MemoryPool *memory_pool, RecyclePool *buffer_pool)
    : index_config_(index_config), column_id_(column_id), memory_pool_(memory_pool), buffer_pool_(buffer_pool) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

SharedPtr<PostingMerger> ColumnIndexMerger::CreatePostingMerger(const Segment &target_segment) {
    return MakeShared<PostingMerger>(memory_pool_, buffer_pool_, target_segment);
}

void ColumnIndexMerger::Merge(const Vector<Segment> &source_segments, const Segment &target_segment) {
    String root_dir = index_config_.GetIndexName();
    Path path = Path(root_dir) / std::to_string(target_segment.GetSegmentId());
    String dict_file = path.string();
    String fst_file = dict_file + DICT_SUFFIX + ".fst";
    dict_file.append(DICT_SUFFIX);
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs_, dict_file, 1024);
    TermMetaDumper term_meta_dumpler(index_config_.GetPostingFormatOption());
    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);
    posting_file_ = MakeShared<FileWriter>(fs_, posting_file, 1024);

    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    SegmentTermPostingQueue term_posting_queue(index_config_, column_id_);
    term_posting_queue.Init(source_segments);
    String term;
    TermMeta term_meta;
    SizeT term_meta_offset = 0;

    while (!term_posting_queue.Empty()) {
        const Vector<SegmentTermPosting *> &merging_term_postings = term_posting_queue.GetCurrentMerging(term);
        MergeTerm(term, term_meta, merging_term_postings, target_segment);
        term_meta_dumpler.Dump(dict_file_writer, term_meta);
        fst_builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
        term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_posting_queue.MoveToNextTerm();
    }
    memory_pool_->Release();
    buffer_pool_->Release();
}

void ColumnIndexMerger::MergeTerm(const String &term,
                                  TermMeta &term_meta,
                                  const Vector<SegmentTermPosting *> &merging_term_postings,
                                  const Segment &target_segment) {
    SharedPtr<PostingMerger> posting_merger = CreatePostingMerger(target_segment);
    posting_merger->Merge(merging_term_postings);
    posting_merger->Dump(posting_file_, term_meta);
    memory_pool_->Reset();
    buffer_pool_->Reset();
}

} // namespace infinity