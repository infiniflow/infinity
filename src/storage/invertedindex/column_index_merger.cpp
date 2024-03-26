module;

#include <fstream>
#include <string>
#include <cassert>

module column_index_merger;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import index_full_text;
import column_index_iterator;
import segment_term_posting;
import fst;
import internal_types;
import posting_byte_slice_reader;
import posting_merger;
import third_party;

namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const String &index_dir, optionflag_t flag, MemoryPool *memory_pool, RecyclePool *buffer_pool)
    : index_dir_(index_dir), flag_(flag), memory_pool_(memory_pool), buffer_pool_(buffer_pool) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

SharedPtr<PostingMerger> ColumnIndexMerger::CreatePostingMerger() {
    return MakeShared<PostingMerger>(memory_pool_, buffer_pool_, flag_, column_length_mutex_, column_length_array_);
}

void ColumnIndexMerger::Merge(const Vector<String> &base_names, const Vector<RowID> &base_rowids, const String &dst_base_name) {
    assert(base_names.size() == base_rowids.size());
    if (base_rowids.empty()) {
        return;
    }
    Path path = Path(index_dir_) / dst_base_name;
    String index_prefix = path.string();
    String dict_file = index_prefix + DICT_SUFFIX;
    String fst_file = dict_file + DICT_SUFFIX + ".fst";
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs_, dict_file, 1024);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);
    posting_file_writer_ = MakeShared<FileWriter>(fs_, posting_file, 1024);

    std::ofstream ofs(fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    SegmentTermPostingQueue term_posting_queue(index_dir_, base_names, base_rowids, flag_);
    String term;
    TermMeta term_meta;
    SizeT term_meta_offset = 0;

    auto merge_base_rowid = base_rowids[0];
    for (auto& row_id : base_rowids) {
        merge_base_rowid = std::min(merge_base_rowid, row_id);
    }

    while (!term_posting_queue.Empty()) {
        const Vector<SegmentTermPosting *> &merging_term_postings = term_posting_queue.GetCurrentMerging(term);

        MergeTerm(term, term_meta, merging_term_postings, merge_base_rowid);

        term_meta_dumpler.Dump(dict_file_writer, term_meta);

        fst_builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
        term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_posting_queue.MoveToNextTerm();
    }
    dict_file_writer->Sync();
    posting_file_writer_->Sync();
    fst_builder.Finish();
    fs_.AppendFile(dict_file, fst_file);
    fs_.DeleteFile(fst_file);
    memory_pool_->Release();
    buffer_pool_->Release();
}

void ColumnIndexMerger::MergeTerm(const String &term,
                                  TermMeta &term_meta,
                                  const Vector<SegmentTermPosting *> &merging_term_postings,
                                  const RowID &merge_base_rowid) {
    SharedPtr<PostingMerger> posting_merger = CreatePostingMerger();
    posting_merger->Merge(merging_term_postings, merge_base_rowid);

    posting_merger->Dump(posting_file_writer_, term_meta);
//    memory_pool_->Reset();
//    buffer_pool_->Reset();
}

} // namespace infinity