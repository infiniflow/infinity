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
import internal_types;

module segment_posting;

namespace infinity {
void SegmentPosting::Init(SharedPtr<ByteSliceList> slice_list, RowID base_row_id, u64 doc_count, TermMeta &term_meta) {
    slice_list_ = std::move(slice_list);
    base_row_id_ = base_row_id;
    doc_count_ = doc_count;
    term_meta_ = term_meta;
    posting_writer_ = nullptr;
}

void SegmentPosting::Init(RowID base_row_id, const SharedPtr<PostingWriter> &posting_writer) {
    base_row_id_ = base_row_id;
    doc_count_ = posting_writer->GetDF();
    posting_writer_ = posting_writer;
    GetInMemTermMeta(term_meta_);
}

} // namespace infinity