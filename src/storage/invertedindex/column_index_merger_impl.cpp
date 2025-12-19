module;

#include <cassert>

module infinity_core:column_index_merger.impl;

import :column_index_merger;
import :byte_slice;
import :byte_slice_reader;
import :file_reader;
import :posting_decoder;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :index_full_text;
import :column_index_iterator;
import :segment_term_posting;
import :fst.fst;
import :posting_byte_slice_reader;
import :posting_merger;
import :virtual_store;
import :local_file_handle;
import :infinity_exception;
import :vector_with_lock;
import :logger;
import :persistence_manager;
import :infinity_context;
import :defer_op;
import :utility;
import :persist_result_handler;
import :fst.build;

import std;

import internal_types;

namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const std::string &index_dir, optionflag_t flag) : index_dir_(index_dir), flag_(flag) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

std::shared_ptr<PostingMerger> ColumnIndexMerger::CreatePostingMerger() { return std::make_shared<PostingMerger>(flag_, column_lengths_); }

void ColumnIndexMerger::Merge(const std::vector<std::string> &base_names, const std::vector<RowID> &base_rowids, const std::string &dst_base_name) {
    assert(base_names.size() == base_rowids.size());
    if (base_rowids.empty()) {
        return;
    }
    std::filesystem::path path = std::filesystem::path(InfinityContext::instance().config()->TempDir()) / index_dir_ / dst_base_name;
    auto index_prefix = path.string();
    auto dict_file = index_prefix + DICT_SUFFIX;
    auto fst_file = dict_file + ".fst";
    auto posting_file = index_prefix + POSTING_SUFFIX;
    auto column_length_file = index_prefix + LENGTH_SUFFIX;

    auto tmp_dict_file(dict_file);
    auto tmp_posting_file(posting_file);
    auto tmp_column_length_file(column_length_file);
    auto tmp_fst_file(fst_file);

    auto dict_file_writer = std::make_shared<FileWriter>(tmp_dict_file, 1024);
    TermMetaDumper term_meta_dumpler((PostingFormatOption(flag_)));
    posting_file_writer_ = std::make_shared<FileWriter>(tmp_posting_file, 1024);
    std::ofstream ofs(tmp_fst_file.c_str(), std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder fst_builder(wtr);

    SegmentTermPostingQueue term_posting_queue(index_dir_, base_names, base_rowids, flag_);
    std::string term;
    TermMeta term_meta;
    size_t term_meta_offset = 0;

    auto merge_base_rowid = base_rowids[0];
    for (auto &row_id : base_rowids) {
        merge_base_rowid = std::min(merge_base_rowid, row_id);
    }

    {
        // prepare column length info
        // the indexes to be merged should be from the same segment
        // otherwise the range of row_id will be very large ( >= 2^32)
        std::vector<u32> &unsafe_column_lengths = column_lengths_.UnsafeVec();
        unsafe_column_lengths.clear();
        for (u32 i = 0; i < base_names.size(); ++i) {
            std::string column_len_file =
                std::filesystem::path(InfinityContext::instance().config()->TempDir()) / index_dir_ / (base_names[i] + LENGTH_SUFFIX);
            RowID base_row_id = base_rowids[i];
            u32 id_offset = base_row_id - merge_base_rowid;

            auto [file_handle, open_status] = VirtualStore::Open(column_len_file, FileAccessMode::kRead);
            if (!open_status.ok()) {
                UnrecoverableError(open_status.message());
            }

            const u32 file_size = file_handle->FileSize();
            if (u32 file_read_array_len = file_size / sizeof(u32); unsafe_column_lengths.size() < id_offset + file_read_array_len) {
                unsafe_column_lengths.resize(id_offset + file_read_array_len);
            }
            auto [read_count, read_status] = file_handle->Read(unsafe_column_lengths.data() + id_offset, file_size);
            if (!read_status.ok()) {
                UnrecoverableError(read_status.message());
            }
            if (read_count != file_size) {
                UnrecoverableError("ColumnIndexMerger: when loading column length file, read_count != file_size");
            }
        }

        auto [file_handle, status] = VirtualStore::Open(tmp_column_length_file, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle->Append(&unsafe_column_lengths[0], sizeof(unsafe_column_lengths[0]) * unsafe_column_lengths.size());
    }

    while (!term_posting_queue.Empty()) {
        const std::vector<SegmentTermPosting *> &merging_term_postings = term_posting_queue.GetCurrentMerging(term);

        MergeTerm(term, term_meta, merging_term_postings, merge_base_rowid);

        term_meta_dumpler.Dump(dict_file_writer, term_meta);

        fst_builder.Insert((u8 *)term.c_str(), term.length(), term_meta_offset);
        term_meta_offset = dict_file_writer->TotalWrittenBytes();
        term_posting_queue.MoveToNextTerm();
    }
    dict_file_writer->Sync();
    posting_file_writer_->Sync();
    fst_builder.Finish();

    LOG_INFO(fmt::format("Merge from FST file: {}, to DICT file: {}", tmp_fst_file, tmp_dict_file));
    VirtualStore::Merge(tmp_dict_file, tmp_fst_file);

    LOG_INFO(fmt::format("Delete FST file: {}", tmp_fst_file));
    VirtualStore::DeleteFile(tmp_fst_file);
}

void ColumnIndexMerger::MergeTerm(const std::string &term,
                                  TermMeta &term_meta,
                                  const std::vector<SegmentTermPosting *> &merging_term_postings,
                                  const RowID &merge_base_rowid) {
    std::shared_ptr<PostingMerger> posting_merger = CreatePostingMerger();
    posting_merger->Merge(merging_term_postings, merge_base_rowid);
    posting_merger->Dump(posting_file_writer_, term_meta);
    LOG_TRACE(fmt::format("ColumnIndexMerger::MergeTerm term: {}, term_meta.doc_freq_: {}", term, term_meta.doc_freq_));
}

} // namespace infinity