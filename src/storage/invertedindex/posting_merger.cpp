module;

module posting_merger;

import stl;

import file_writer;
import doc_list_encoder;
import inmem_posting_decoder;
import position_list_encoder;
import posting_list_format;
import index_defines;
import posting_writer;
import vector_with_lock;
import posting_decoder;
import term_meta;
import column_index_iterator;
import segment_term_posting;
import internal_types;
import third_party;

namespace infinity {

class DocMerger {
public:
    DocMerger(const PostingFormatOption &format_option, PostingDecoder *posting_decoder)
        : format_option_(format_option), posting_decoder_(posting_decoder) {}

    docid_t CurrentDoc() const { return doc_id_; }

    bool End() const { return posting_decoder_ == nullptr; }

    bool HasNext();

    void Merge(docid_t doc_id, PostingWriter *posting_writer);

    tf_t MergePosition(docid_t doc_id, PostingWriter *posting_writer);

private:
    docid_t doc_id_buf_[MAX_DOC_PER_RECORD];
    tf_t tf_list_buf_[MAX_DOC_PER_RECORD];
    docpayload_t doc_payload_buf_[MAX_DOC_PER_RECORD];
    pos_t pos_buf_[MAX_POS_PER_RECORD];

    u32 doc_count_in_buf_{0};
    u32 doc_buffer_cursor_{0};
    u32 last_doc_buf_cursor_{0};
    docid_t doc_id_{0};

    u32 pos_count_in_buf_{0};
    u32 pos_buf_cursor_{0};
    u32 pos_index_{0};

    PostingFormatOption format_option_;
    PostingDecoder *posting_decoder_{nullptr};
};

void DocMerger::Merge(docid_t doc_id, PostingWriter *posting_writer) {
    tf_t tf = MergePosition(doc_id, posting_writer);
    if (doc_id != INVALID_DOCID) {
        if (format_option_.HasTermFrequency() && !format_option_.HasPositionList()) {
            posting_writer->SetCurrentTF(tf);
        }
        docpayload_t doc_payload = doc_payload_buf_[last_doc_buf_cursor_];
        posting_writer->EndDocument(doc_id, doc_payload);
    }
}

tf_t DocMerger::MergePosition(docid_t doc_id, PostingWriter *posting_writer) {
    tf_t tf = 0;
    pos_t pos = 0;
    while (true) {
        if (format_option_.HasPositionList()) {
            if (pos_buf_cursor_ == pos_count_in_buf_) {
                pos_buf_cursor_ = 0;
                pos_count_in_buf_ = posting_decoder_->DecodePosList(pos_buf_, MAX_POS_PER_RECORD);
                if (pos_count_in_buf_ == 0) {
                    break;
                }
            }
            if (doc_id != INVALID_DOCID) {
                pos += pos_buf_[pos_buf_cursor_];
                posting_writer->AddPosition(pos);
            }
            pos_buf_cursor_++;
        }
        tf++;
        pos_index_++;

        if (format_option_.HasTfList() && tf >= tf_list_buf_[last_doc_buf_cursor_]) {
            break;
        }
    }
    return tf;
}

bool DocMerger::HasNext() {
    if (!posting_decoder_)
        return false;
    if (doc_buffer_cursor_ == doc_count_in_buf_) {
        doc_buffer_cursor_ = 0;
        doc_count_in_buf_ = posting_decoder_->DecodeDocList(doc_id_buf_, tf_list_buf_, doc_payload_buf_, MAX_DOC_PER_RECORD);
        if (doc_count_in_buf_ == 0) {
            posting_decoder_ = nullptr;
            return false;
        }
    }
    doc_id_ += doc_id_buf_[doc_buffer_cursor_];
    last_doc_buf_cursor_ = doc_buffer_cursor_;
    if (!End()) {
        doc_buffer_cursor_++;
    }
    return true;
}

class PostingDumper {
public:
    PostingDumper(const PostingFormat &posting_format, VectorWithLock<u32> &column_length_array) : column_lengths_(column_length_array) {
        posting_writer_ = MakeShared<PostingWriter>(posting_format, column_lengths_);
    }

    ~PostingDumper() {}

    void Dump(const String &term);

    void EndSegment() { posting_writer_->EndSegment(); }

    u32 GetDF() const { return posting_writer_->GetDF(); }

    u32 GetTotalTF() const { return posting_writer_->GetTotalTF(); }

    u64 GetPostingLength() const { return posting_writer_->GetDumpLength(); }

    SharedPtr<PostingWriter> GetPostingWriter() { return posting_writer_; }

private:
    SharedPtr<PostingWriter> posting_writer_;
    // for column length info
    VectorWithLock<u32> &column_lengths_;
};

class SortedPosting {
public:
    SortedPosting(const PostingFormatOption &format_option, docid_t base_doc_id, PostingDecoder *posting_decoder)
        : format_option_(format_option), base_doc_id_(base_doc_id), doc_merger_(format_option, posting_decoder) {}
    ~SortedPosting() {}

    bool Next() {
        bool ret = doc_merger_.HasNext();
        if (ret) {
            current_doc_id_ = doc_merger_.CurrentDoc();
        }
        return ret;
    }

    void Merge(const SharedPtr<PostingDumper> &pos_dumper) {
        if (current_doc_id_ == INVALID_DOCID) {
            doc_merger_.Merge(INVALID_DOCID, nullptr);
            return;
        }
        SharedPtr<PostingWriter> posting_writer = pos_dumper->GetPostingWriter();
        doc_merger_.Merge(base_doc_id_ + current_doc_id_, posting_writer.get());
    }

private:
    PostingFormatOption format_option_;
    docid_t base_doc_id_{0};
    docid_t current_doc_id_{INVALID_DOCID};
    DocMerger doc_merger_;
};

PostingMerger::PostingMerger(optionflag_t flag, VectorWithLock<u32> &column_length_array)
    : posting_format_(PostingFormatOption(flag)), column_lengths_(column_length_array) {
    posting_dumper_ = MakeShared<PostingDumper>(posting_format_, column_lengths_);
}

PostingMerger::~PostingMerger() {}

void PostingMerger::Merge(const Vector<SegmentTermPosting *>& segment_term_postings, const RowID& merge_base_rowid) {
    // segment_term_postings is already sorted by base_row_id
    for (u32 i = 0; i < segment_term_postings.size(); ++i) {
        SegmentTermPosting *term_posting = segment_term_postings[i];
        RowID base_row_id = term_posting->GetBaseRowId();
        u32 base_doc_id = base_row_id - merge_base_rowid;
        PostingDecoder *decoder = term_posting->GetPostingDecoder();
        SortedPosting sorted_posting(posting_format_.GetOption(), base_doc_id, decoder);
        while (sorted_posting.Next()) {
            sorted_posting.Merge(posting_dumper_);
        }
    }

    posting_dumper_->EndSegment();
}

void PostingMerger::Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta) {
    posting_dumper_->GetPostingWriter()->Dump(file_writer, term_meta);
}

u32 PostingMerger::GetDF() { return posting_dumper_->GetDF(); }

u32 PostingMerger::GetTotalTF() { return posting_dumper_->GetTotalTF(); }

} // namespace infinity
