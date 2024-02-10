module;

module posting_merger;

import stl;
import memory_pool;
import file_writer;
import doc_list_encoder;
import inmem_posting_decoder;
import pos_list_encoder;
import posting_list_format;
import index_defines;
import posting_writer;
import posting_decoder;
import term_meta;

namespace infinity {

class DocMerger {
public:
    DocMerger(const PostingFormatOption &format_option, PostingDecoder *posting_decoder);

    ~DocMerger();

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

DocMerger::DocMerger(const PostingFormatOption &format_option, PostingDecoder *posting_decoder)
    : format_option_(format_option), posting_decoder_(posting_decoder) {}

DocMerger::~DocMerger() {}

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
        if (format_option_.HasTfBitmap() && posting_decoder_->IsDocEnd(pos_index_)) {
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
    PostingDumper(const PostingFormatOption &format_option) : format_option_(format_option) {}

    ~PostingDumper() {}

    void Dump();

private:
    PostingFormatOption format_option_;
    Vector<SharedPtr<PostingWriter>> posting_writers_;
};

void PostingDumper::Dump() {}

class SortedPosting {
public:
    SortedPosting(const PostingFormatOption &format_option, docid_t base_doc_id, PostingDecoder *posting_decoder)
        : format_option_(format_option), base_doc_id_(base_doc_id), doc_merger_(format_option, posting_decoder) {}
    ~SortedPosting() {}

    bool HasNext() { return false; }

    void Merge(const SharedPtr<PostingDumper> &pos_dumper) {}

    Pair<segmentid_t, docid_t> GetCurrent() const { return current_; }

private:
    PostingFormatOption format_option_;
    docid_t base_doc_id_;
    Pair<segmentid_t, docid_t> current_{INVALID_SEGMENTID, INVALID_DOCID};
    DocMerger doc_merger_;
};

struct SortedPostingComparator {
    bool operator()(const SortedPosting *lhs, const SortedPosting *rhs) { return lhs->GetCurrent().first > rhs->GetCurrent().first; }
};

using PriorityQueue = Heap<SortedPosting *, SortedPostingComparator>;

PostingMerger::PostingMerger() { posting_dumper_ = MakeShared<PostingDumper>(format_option_); }

PostingMerger::~PostingMerger() {}

void PostingMerger::Merge(const Segment &segment) {}

} // namespace infinity
