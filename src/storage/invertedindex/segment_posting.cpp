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
import byte_slice_reader;
import index_defines;
module segment_posting;

namespace infinity {
SegmentPosting::SegmentPosting(const PostingFormatOption &posting_option)
    : base_doc_id_(INVALID_DOCID), doc_count_(0), posting_writer_(nullptr), posting_option_(posting_option) {}

void SegmentPosting::Init(const SharedPtr<ByteSliceList> &slice_list, docid_t base_doc_id, u64 doc_count, TermMeta &term_meta) {
    slice_list_ = slice_list;
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
    term_meta_ = term_meta;
    posting_writer_ = nullptr;
}

void SegmentPosting::Init(docid_t base_doc_id, PostingWriter *posting_writer) {
    base_doc_id_ = base_doc_id;
    doc_count_ = posting_writer->GetDF();
    posting_writer_ = posting_writer;
    GetInMemTermMeta(term_meta_);
}

} // namespace infinity