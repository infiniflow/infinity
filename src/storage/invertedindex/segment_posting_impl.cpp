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

module infinity_core:segment_posting.impl;

import :segment_posting;
import :stl;
import :byte_slice;
import :posting_list_format;
import :posting_writer;
import :term_meta;
import :byte_slice_reader;
import :index_defines;
import :file_reader;

import third_party;

import internal_types;

namespace infinity {
void SegmentPosting::Init(std::shared_ptr<ByteSliceList> slice_list, RowID base_row_id, u64 doc_count, TermMeta &term_meta) {
    slice_list_ = std::move(slice_list);
    base_row_id_ = base_row_id;
    doc_count_ = doc_count;
    term_meta_ = term_meta;
    posting_writer_ = nullptr;
    posting_reader_ = nullptr;
}

void SegmentPosting::Init(RowID base_row_id, const std::shared_ptr<PostingWriter> &posting_writer) {
    base_row_id_ = base_row_id;
    doc_count_ = posting_writer->GetDF();
    posting_writer_ = posting_writer;
    GetInMemTermMeta(term_meta_);
}

void SegmentPosting::Init(std::shared_ptr<ByteSliceList> doc_slice_list,
                          std::shared_ptr<ByteSliceList> pos_slice_list,
                          RowID base_row_id,
                          u64 doc_count,
                          TermMeta &term_meta,
                          u64 pos_begin,
                          u64 pos_size,
                          const std::shared_ptr<FileReader> &posting_reader) {
    doc_slice_list_ = std::move(doc_slice_list);
    pos_slice_list_ = std::move(pos_slice_list);
    base_row_id_ = base_row_id;
    doc_count_ = doc_count;
    term_meta_ = term_meta;
    doc_start_ = term_meta.doc_start_;
    posting_writer_ = nullptr;
    pos_begin_ = pos_begin;
    pos_size_ = pos_size;
    posting_reader_ = std::make_shared<FileReader>(posting_reader->path_, 1024);
}

const std::shared_ptr<ByteSliceList> &SegmentPosting::GetPosSliceListPtr() {
    if (pos_slice_list_.get() == nullptr) {
        ByteSlice *pos_slice = ByteSlice::CreateSlice(pos_size_);

        posting_reader_->Seek(doc_start_ + pos_begin_);
        posting_reader_->Read((char *)pos_slice->data_, pos_size_);

        pos_slice_list_ = std::make_shared<ByteSliceList>(pos_slice);
    }
    return pos_slice_list_;
}

} // namespace infinity