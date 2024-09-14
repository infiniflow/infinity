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
import persistence_manager;
import infinity_context;
import defer_op;
import utility;

namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const String &index_dir, optionflag_t flag) : index_dir_(index_dir), flag_(flag) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

SharedPtr<PostingMerger> ColumnIndexMerger::CreatePostingMerger() { return MakeShared<PostingMerger>(flag_, column_lengths_); }

void ColumnIndexMerger::Merge(const Vector<String> &base_names, const Vector<RowID> &base_rowids, const String &dst_base_name) {
    assert(base_names.size() == base_rowids.size());
    if (base_rowids.empty()) {
        return;
    }
    Path path = Path(InfinityContext::instance().config()->DataDir()) / index_dir_ / dst_base_name;
    String index_prefix = path.string();
    String dict_file = index_prefix + DICT_SUFFIX;
    String fst_file = dict_file + ".fst";
    String posting_file = index_prefix + POSTING_SUFFIX;
    String column_length_file = index_prefix + LENGTH_SUFFIX;

    String tmp_dict_file(dict_file);
    String tmp_posting_file(posting_file);
    String tmp_column_length_file(column_length_file);
    String tmp_fst_file(fst_file);

    // handle persistence obj_addrs
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        Path temp_dir = Path(InfinityContext::instance().config()->TempDir());
        tmp_dict_file = temp_dir / StringTransform(tmp_dict_file, "/", "_");
        tmp_posting_file = temp_dir / StringTransform(tmp_posting_file, "/", "_");
        tmp_column_length_file = temp_dir / StringTransform(tmp_column_length_file, "/", "_");
        tmp_fst_file = temp_dir / StringTransform(tmp_fst_file, "/", "_");
    }

    SharedPtr<FileWriter> dict_file_writer = MakeShared<FileWriter>(fs_, tmp_dict_file, 1024);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    posting_file_writer_ = MakeShared<FileWriter>(fs_, tmp_posting_file, 1024);
    std::ofstream ofs(tmp_fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    SegmentTermPostingQueue term_posting_queue(index_dir_, base_names, base_rowids, flag_);
    String term;
    TermMeta term_meta;
    SizeT term_meta_offset = 0;

    auto merge_base_rowid = base_rowids[0];
    for (auto &row_id : base_rowids) {
        merge_base_rowid = std::min(merge_base_rowid, row_id);
    }

    {
        // prepare column length info
        // the indexes to be merged should be from the same segment
        // otherwise the range of row_id will be very large ( >= 2^32)
        Vector<u32> &unsafe_column_lengths = column_lengths_.UnsafeVec();
        unsafe_column_lengths.clear();
        for (u32 i = 0; i < base_names.size(); ++i) {
            String column_len_file = Path(InfinityContext::instance().config()->DataDir()) / index_dir_ / (base_names[i] + LENGTH_SUFFIX);
            RowID base_row_id = base_rowids[i];
            u32 id_offset = base_row_id - merge_base_rowid;

            if (use_object_cache) {
                column_len_file = pm->GetObjPath(pm->GetObjCache(column_len_file).obj_key_);
            }

            auto [file_handler, status] = fs_.OpenFile(column_len_file, FileFlags::READ_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            const u32 file_size = fs_.GetFileSize(*file_handler);
            u32 file_read_array_len = file_size / sizeof(u32);
            if (unsafe_column_lengths.size() < id_offset + file_read_array_len) {
                unsafe_column_lengths.resize(id_offset + file_read_array_len);
            }
            const i64 read_count = fs_.Read(*file_handler, unsafe_column_lengths.data() + id_offset, file_size);
            file_handler->Close();
            if (read_count != file_size) {
                String error_message = "ColumnIndexMerger: when loading column length file, read_count != file_size";
                UnrecoverableError(error_message);
            }

            if (use_object_cache) {
                column_len_file = Path(InfinityContext::instance().config()->DataDir()) / index_dir_ / (base_names[i] + LENGTH_SUFFIX);
                pm->PutObjCache(column_len_file);
            }
        }

        auto [file_handler, status] = fs_.OpenFile(tmp_column_length_file, FileFlags::WRITE_FLAG | FileFlags::TRUNCATE_CREATE, FileLockType::kNoLock);
        if (!status.ok()) {
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
    fs_.AppendFile(tmp_dict_file, tmp_fst_file);
    fs_.DeleteFile(tmp_fst_file);
    if (use_object_cache) {
        pm->Persist(dict_file, tmp_dict_file, false);
        pm->Persist(posting_file, tmp_posting_file, false);
        pm->Persist(column_length_file, tmp_column_length_file, false);
    }
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