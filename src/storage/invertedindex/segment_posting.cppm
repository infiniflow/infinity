// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import byte_slice;
import posting_list_format;
import posting_writer;
import term_meta;
import index_defines;
import internal_types;

export module segment_posting;

namespace infinity {

export class SegmentPosting {
public:
    SegmentPosting(const PostingFormatOption &posting_option = OPTION_FLAG_ALL);
    ~SegmentPosting() {}

    bool operator==(const SegmentPosting &seg_posting) const {
        return /*(slice_list_ == seg_posting.slice_list_) && */ (term_meta_ == seg_posting.term_meta_) &&
               /*(posting_option_ == seg_posting.posting_option_) */ (base_row_id_ == seg_posting.base_row_id_) &&
               (doc_count_ == seg_posting.doc_count_) && (posting_writer_ == seg_posting.posting_writer_);
    }

    // for on disk segment posting
    void Init(const SharedPtr<ByteSliceList> &slice_list, RowID base_row_id, u64 doc_count, TermMeta &term_meta);
    // for in memory segment posting
    void Init(RowID base_row_id, PostingWriter *posting_writer);

    RowID GetBaseRowId() const { return base_row_id_; }
    void SetBaseRowId(RowID base_row_id) { base_row_id_ = base_row_id; }

    u32 GetDocCount() const { return doc_count_; }
    void SetDocCount(const u32 doc_count) { doc_count_ = doc_count; }

    const PostingFormatOption &GetPostingFormatOption() const { return posting_option_; }
    void SetPostingFormatOption(const PostingFormatOption &option) { posting_option_ = option; }

    const PostingWriter *GetInMemPostingWriter() const { return posting_writer_; }
    bool IsInMemorySegment() const { return posting_writer_ != nullptr; }

    void GetInMemTermMeta(TermMeta &tm) {
        df_t df = posting_writer_->GetDF();
        tf_t ttf = posting_writer_->GetTotalTF();
        tm.SetDocFreq(df);
        tm.SetTotalTermFreq(ttf);
    }

    const TermMeta &GetTermMeta() const { return term_meta_; }

    const SharedPtr<ByteSliceList> &GetSliceListPtr() const { return slice_list_; }

private:
    SharedPtr<ByteSliceList> slice_list_;
    RowID base_row_id_;
    u32 doc_count_;
    TermMeta term_meta_;
    PostingWriter *posting_writer_;
    PostingFormatOption posting_option_;
};
} // namespace infinity