module;

export module infinity_core:posting_merger;

import :stl;

import :file_writer;
import :posting_decoder;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :column_index_iterator;
import :segment_term_posting;
import internal_types;
import :vector_with_lock;

namespace infinity {

class PostingDumper;
export class PostingMerger {
public:
    PostingMerger(optionflag_t flag, VectorWithLock<u32> &column_length_array);

    ~PostingMerger();

    void Merge(const Vector<SegmentTermPosting *> &segment_term_postings, const RowID &merge_base_rowid);

    void Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta);

    u32 GetDF();

    u32 GetTotalTF();

private:
    PostingFormat posting_format_;
    SharedPtr<PostingDumper> posting_dumper_;
    df_t df_;
    ttf_t ttf_;
    // for column length info
    VectorWithLock<u32> &column_lengths_;
};
} // namespace infinity