module;

#include <cassert>
#include <fstream>
#include <string>

module column_index_merger;

import stl;
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
import file_system;
import file_system_type;
import infinity_exception;
import vector_with_lock;
import logger;

namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const String &index_dir, optionflag_t flag) : index_dir_(index_dir), flag_(flag) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

SharedPtr<PostingMerger> ColumnIndexMerger::CreatePostingMerger() { return MakeShared<PostingMerger>(flag_, column_lengths_); }

void ColumnIndexMerger::Merge(const Vector<String> &base_names, const Vector<RowID> &base_rowids, const String &dst_base_name) {
    assert(base_names.size() == base_rowids.size());
    if (base_rowids.empty()) {
        return;
    }
    Path path = Path(index_dir_) / dst_base_name;
    String index_prefix = path.string();
    String dict_file = index_prefix + DICT_SUFFIX;
    String fst_file = dict_file + ".fst";
    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs_, dict_file, 1024);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    String posting_file = index_prefix + POSTING_SUFFIX;
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

    {
        // prepare column length info
        // the indexes to be merged should be from the same segment
        // otherwise the range of row_id will be very large ( >= 2^32)
        Vector<u32> &unsafe_column_lengths = column_lengths_.UnsafeVec();
        unsafe_column_lengths.clear();
        for (u32 i = 0; i < base_names.size(); ++i) {
            String column_len_file = (Path(index_dir_) / base_names[i]).string() + LENGTH_SUFFIX;
            RowID base_row_id = base_rowids[i];
            u32 id_offset = base_row_id - merge_base_rowid;
            auto [file_handler, status] = fs_.OpenFile(column_len_file, FileFlags::READ_FLAG, FileLockType::kNoLock);
            if(!status.ok()) {
                UnrecoverableError(status.message());
            }

            const u32 file_size = fs_.GetFileSize(*file_handler);
            u32 file_read_array_len = file_size / sizeof(u32);
            unsafe_column_lengths.resize(id_offset + file_read_array_len);
            const i64 read_count = fs_.Read(*file_handler, unsafe_column_lengths.data() + id_offset, file_size);
            file_handler->Close();
            if (read_count != file_size) {
                String error_message = "ColumnIndexMerger: when loading column length file, read_count != file_size";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
        }

        String column_length_file = index_prefix + LENGTH_SUFFIX;
        auto [file_handler, status] =
            fs_.OpenFile(column_length_file, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kNoLock);
        if(!status.ok()) {
            UnrecoverableError(status.message());
        }
        fs_.Write(*file_handler, &unsafe_column_lengths[0], sizeof(unsafe_column_lengths[0]) * unsafe_column_lengths.size());
        fs_.Close(*file_handler);
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
}

void ColumnIndexMerger::MergeTerm(const String &term,
                                  TermMeta &term_meta,
                                  const Vector<SegmentTermPosting *> &merging_term_postings,
                                  const RowID &merge_base_rowid) {
    SharedPtr<PostingMerger> posting_merger = CreatePostingMerger();
    posting_merger->Merge(merging_term_postings, merge_base_rowid);

    posting_merger->Dump(posting_file_writer_, term_meta);
}

} // namespace infinity