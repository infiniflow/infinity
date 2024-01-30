module;

import stl;
import byte_slice;
import posting_list_format;
import posting_writer;
import term_meta;
import index_defines;
export module segment_posting;

namespace infinity {

export class SegmentPosting {
public:
    SegmentPosting(const PostingFormatOption &posting_option = OPTION_FLAG_ALL);
    ~SegmentPosting() {}

    bool operator==(const SegmentPosting &seg_posting) const {
        return /*(slice_list_ == seg_posting.slice_list_) && */ (term_meta_ == seg_posting.term_meta_) &&
               /*(posting_option_ == seg_posting.posting_option_) */ (base_doc_id_ == seg_posting.base_doc_id_) &&
               (doc_count_ == seg_posting.doc_count_) && (posting_writer_ == seg_posting.posting_writer_);
    }

    void Init(const SharedPtr<ByteSliceList> &slice_list, docid_t base_doc_id, u64 doc_count);

    void Init(docid_t base_doc_id, u64 doc_count);

    // for in memory segment posting
    void Init(docid_t base_doc_id, u32 doc_count, PostingWriter *posting_writer);

    docid_t GetBaseDocId() const { return base_doc_id_; }
    void SetBaseDocId(docid_t base_doc_id) { base_doc_id_ = base_doc_id; }

    u32 GetDocCount() const { return doc_count_; }
    void SetDocCount(const u32 doc_count) { doc_count_ = doc_count; }

    const PostingFormatOption &GetPostingFormatOption() const { return posting_option_; }
    void SetPostingFormatOption(const PostingFormatOption &option) { posting_option_ = option; }

    const PostingWriter *GetInMemPostingWriter() const { return posting_writer_; }
    bool IsInMemorySegment() const { return posting_writer_ != nullptr; }

    void GetInMemTermMeta(TermMeta &tm) {
        df_t df = posting_writer_->GetDF();
        tf_t ttf = posting_writer_->GetTotalTF();
        // termpayload_t term_payload = posting_writer_->GetTermPayload();
        tm.SetDocFreq(df);
        tm.SetTotalTermFreq(ttf);
        // tm.SetPayload(term_payload);
    }

    TermMeta GetSegmentTermMeta() const;

    const SharedPtr<ByteSliceList> &GetSliceListPtr() const { return slice_list_; }

private:
    SharedPtr<ByteSliceList> slice_list_;
    u32 base_doc_id_;
    u32 doc_count_;
    TermMeta term_meta_;
    PostingWriter *posting_writer_;
    PostingFormatOption posting_option_;
};
} // namespace infinity