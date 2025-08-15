module;

export module infinity_core:column_index_merger;

import :stl;
import :file_writer;
import :posting_decoder;
import :posting_merger;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :index_full_text;
import :column_index_iterator;
import :segment_term_posting;
import :vector_with_lock;

import internal_types;

namespace infinity {
export class ColumnIndexMerger {
public:
    ColumnIndexMerger(const std::string &index_dir, optionflag_t flag);
    ~ColumnIndexMerger();

    void Merge(const std::vector<std::string> &base_names, const std::vector<RowID> &base_rowids, const std::string &dst_base_name);

private:
    std::shared_ptr<PostingMerger> CreatePostingMerger();

    void MergeTerm(const std::string &term, TermMeta &term_meta, const std::vector<SegmentTermPosting *> &merging_term_postings, const RowID &merge_base_rowid);

    std::string index_dir_;
    optionflag_t flag_;
    std::shared_ptr<FileWriter> posting_file_writer_;

    // for column length info
    VectorWithLock<u32> column_lengths_;
};
} // namespace infinity