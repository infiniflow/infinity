#pragma once

#include "common/memory/byte_slice.h"
#include "format/posting_list_format.h"
#include "format/term_meta.h"
#include "posting_writer.h"

#include <memory>

namespace infinity {

class SegmentPosting {
public:
    SegmentPosting(const PostingFormatOption &posting_option = OPTION_FLAG_ALL);
    ~SegmentPosting() {}

    bool operator==(const SegmentPosting &seg_posting) const {
        return (slice_list_ == seg_posting.slice_list_) && (term_meta_ == seg_posting.term_meta_) &&
               (posting_option_ == seg_posting.posting_option_) && (base_doc_id_ == seg_posting.base_doc_id_) &&
               (doc_count_ == seg_posting.doc_count_) && (posting_writer_ == seg_posting.posting_writer_);
    }

    void Init(const std::shared_ptr<ByteSliceList> &slice_list, docid_t base_doc_id, uint64_t doc_count);

    void Init(docid_t base_doc_id, uint64_t doc_count);

    // for realtime segment posting
    void Init(docid_t base_doc_id, uint32_t doc_count, PostingWriter *posting_writer);

    docid_t GetBaseDocId() const { return base_doc_id_; }
    void SetBaseDocId(docid_t base_doc_id) { base_doc_id_ = base_doc_id; }

    uint32_t GetDocCount() const { return doc_count_; }
    void SetDocCount(const uint32_t doc_count) { doc_count_ = doc_count; }

    const PostingFormatOption &GetPostingFormatOption() const { return posting_option_; }
    void SetPostingFormatOption(const PostingFormatOption &option) { posting_option_ = option; }

    const PostingWriter *GetInMemPostingWriter() const { return posting_writer_; }
    bool IsRealTimeSegment() const { return posting_writer_ != NULL; }

    void GetTermMetaForRealtime(TermMeta &tm) {
        df_t df = posting_writer_->GetDF();
        tf_t ttf = posting_writer_->GetTotalTF();
        // termpayload_t term_payload = posting_writer_->GetTermPayload();
        tm.SetDocFreq(df);
        tm.SetTotalTermFreq(ttf);
        // tm.SetPayload(term_payload);
    }

    TermMeta GetCurrentTermMeta() const;

    const std::shared_ptr<ByteSliceList> &GetSliceListPtr() const { return slice_list_; }

private:
    std::shared_ptr<ByteSliceList> slice_list_;
    uint32_t base_doc_id_;
    uint32_t doc_count_;
    TermMeta term_meta_;
    PostingWriter *posting_writer_;
    PostingFormatOption posting_option_;
};
} // namespace infinity