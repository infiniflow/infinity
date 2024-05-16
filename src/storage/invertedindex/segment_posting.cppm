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
import file_reader;
import memory_pool;
import file_system;

export module segment_posting;

namespace infinity {

export class SegmentPosting {
public:
    SegmentPosting(){};
    ~SegmentPosting(){};

    // for on disk segment posting
    void Init(SharedPtr<ByteSliceList> slice_list, RowID base_row_id, u64 doc_count, TermMeta &term_meta);
    void Init(SharedPtr<ByteSliceList> doc_slice_list,
              SharedPtr<ByteSliceList> pos_slice_list,
              RowID base_row_id,
              u64 doc_count,
              TermMeta &term_meta,
              u64 pos_begin,
              u64 pos_size,
              const SharedPtr<FileReader> &posting_reader,
              MemoryPool *session_pool);
    // for in memory segment posting
    void Init(RowID base_row_id, const SharedPtr<PostingWriter> &posting_writer);

    RowID GetBaseRowId() const { return base_row_id_; }
    void SetBaseRowId(RowID base_row_id) { base_row_id_ = base_row_id; }

    u32 GetDocCount() const { return doc_count_; }
    void SetDocCount(const u32 doc_count) { doc_count_ = doc_count; }

    const SharedPtr<PostingWriter> &GetInMemPostingWriter() const { return posting_writer_; }
    bool IsInMemorySegment() const { return posting_writer_.get(); }

    void GetInMemTermMeta(TermMeta &tm) {
        df_t df = posting_writer_->GetDF();
        tf_t ttf = posting_writer_->GetTotalTF();
        tm.SetDocFreq(df);
        tm.SetTotalTermFreq(ttf);
    }

    const TermMeta &GetTermMeta() const { return term_meta_; }

    const SharedPtr<ByteSliceList> &GetSliceListPtr() const { return slice_list_; }

    const SharedPtr<ByteSliceList> &GetDocSliceListPtr() const { return doc_slice_list_; }

    const SharedPtr<ByteSliceList> &GetPosSliceListPtr();

private:
    SharedPtr<ByteSliceList> slice_list_{nullptr};
    SharedPtr<ByteSliceList> doc_slice_list_{nullptr};
    SharedPtr<ByteSliceList> pos_slice_list_{nullptr};
    RowID base_row_id_ = INVALID_ROWID;
    u32 doc_count_ = 0;
    TermMeta term_meta_;
    SharedPtr<PostingWriter> posting_writer_{nullptr};
    SharedPtr<FileReader> posting_reader_{nullptr};
    FileSystem* fs_ = nullptr;
    String* path_ = nullptr;
    u64 pos_begin_ = 0;
    u64 pos_size_ = 0;
    u64 doc_start_ = 0;
    MemoryPool *session_pool_;
};
} // namespace infinity