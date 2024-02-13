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
import column_index_iterator;

namespace infinity {

SegmentTermPosting::SegmentTermPosting(segmentid_t segment_id, docid_t base_doc) : segment_id_(segment_id), base_doc_id_(base_doc) {}

bool SegmentTermPosting::HasNext() {
    if (column_index_iterator_->HasNext()) {
        posting_decoder_ = column_index_iterator_->Next(term_);
        return true;
    }
    return false;
}

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
    PostingDumper(MemoryPool *memory_pool, RecyclePool *buffer_pool, const PostingFormatOption &format_option, const Vector<Segment> &segments)
        : format_option_(format_option), segments_(segments) {
        for (u32 i = 0; i < segments.size(); ++i) {
            SharedPtr<PostingWriter> posting_writer = MakeShared<PostingWriter>(memory_pool, buffer_pool, format_option);
            posting_writers_.push_back(posting_writer);
            segmentid_t segment_id = segments[i].GetSegmentId();
            map_[segment_id] = posting_writer;
        }
    }

    ~PostingDumper() {}

    void Dump();

    void EndSegment() {
        for (u32 i = 0; i < posting_writers_.size(); ++i) {
            posting_writers_[i]->EndSegment();
        }
    }

    u32 GetDF() const {
        u32 df = 0;
        for (u32 i = 0; i < posting_writers_.size(); ++i) {
            df += posting_writers_[i]->GetDF();
        }
        return df;
    }

    u32 GetTotalTF() const {
        u32 ttf = 0;
        for (u32 i = 0; i < posting_writers_.size(); ++i) {
            ttf += posting_writers_[i]->GetTotalTF();
        }
        return ttf;
    }

    u64 GetPostingLength() const {
        u64 length = 0;
        for (u32 i = 0; i < posting_writers_.size(); ++i) {
            length += posting_writers_[i]->GetDumpLength();
        }
        return length;
    }

    SharedPtr<PostingWriter> GetPostingWriter(segmentid_t segment_id) { return map_[segment_id]; }

private:
    PostingFormatOption format_option_;
    Vector<SharedPtr<PostingWriter>> posting_writers_;
    HashMap<segmentid_t, SharedPtr<PostingWriter>> map_;
    Vector<Segment> segments_;
};

void PostingDumper::Dump() {}

class SortedPosting {
public:
    SortedPosting(const PostingFormatOption &format_option, docid_t base_doc_id, PostingDecoder *posting_decoder)
        : format_option_(format_option), base_doc_id_(base_doc_id), doc_merger_(format_option, posting_decoder) {}
    ~SortedPosting() {}

    bool Next() {
        bool ret = doc_merger_.HasNext();
        if (!ret)
            return false;
        current_doc_id_ = doc_merger_.CurrentDoc();
        return ret;
    }

    void Merge(const SharedPtr<PostingDumper> &pos_dumper) {
        if (current_doc_id_ == INVALID_DOCID) {
            doc_merger_.Merge(INVALID_DOCID, nullptr);
            return;
        }
        SharedPtr<PostingWriter> posting_writer = pos_dumper->GetPostingWriter(current_segment_);
        doc_merger_.Merge(current_doc_id_, posting_writer.get());
    }

    docid_t GetCurrentDocID() const { return current_doc_id_; }

private:
    PostingFormatOption format_option_;
    docid_t base_doc_id_;
    segmentid_t current_segment_{INVALID_SEGMENTID};
    docid_t current_doc_id_{INVALID_DOCID};
    DocMerger doc_merger_;
};

struct SortedPostingComparator {
    bool operator()(const SortedPosting *lhs, const SortedPosting *rhs) { return lhs->GetCurrentDocID() > rhs->GetCurrentDocID(); }
};

using PriorityQueue = Heap<SortedPosting *, SortedPostingComparator>;

PostingMerger::PostingMerger(MemoryPool *memory_pool, RecyclePool *buffer_pool, const Vector<Segment> &segments)
    : memory_pool_(memory_pool), buffer_pool_(buffer_pool) {
    posting_dumper_ = MakeShared<PostingDumper>(memory_pool, buffer_pool, format_option_, segments);
}

PostingMerger::~PostingMerger() {}

void PostingMerger::Merge(const Vector<SegmentTermPosting *> &segment_term_postings) {
    PriorityQueue queue;
    for (u32 i = 0; i < segment_term_postings.size(); ++i) {
        SegmentTermPosting *term_posting = segment_term_postings[i];
        u64 base_doc_id = term_posting->GetBaesDocId();
        PostingDecoder *decoder = term_posting->GetPostingDecoder();
        SortedPosting *sorted_posting = new SortedPosting(format_option_, base_doc_id, decoder);
        if (sorted_posting->Next()) {
            queue.push(sorted_posting);
        } else
            delete sorted_posting;
    }
    while (!queue.empty()) {
        SortedPosting *sorted_posting = queue.top();
        queue.pop();
        if (sorted_posting->Next()) {
            queue.push(sorted_posting);
        } else
            delete sorted_posting;
    }
    posting_dumper_->EndSegment();
}

} // namespace infinity
