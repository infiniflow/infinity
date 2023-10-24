#pragma once

#include "common/memory/byte_slice.h"
#include "format/posting_list_format.h"
#include "format/term_meta.h"

#include <memory>

namespace infinity {

class SegmentPosting {
public:
    SegmentPosting(const PostingFormatOption &posting_option = OPTION_FLAG_ALL);
    ~SegmentPosting();

    docid_t GetBaseDocId() const { return base_doc_id_; }
    void SetBaseDocId(docid_t base_doc_id) { base_doc_id_ = base_doc_id; }

private:
    std::shared_ptr<ByteSliceList> slice_list_;
    uint32_t base_doc_id_;
    uint32_t doc_count_;
    TermMeta term_meta_;
    PostingFormatOption posting_option_;
};
} // namespace infinity