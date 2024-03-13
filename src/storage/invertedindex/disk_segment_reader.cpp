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

module disk_index_segment_reader;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import file_reader;
import dict_reader;
import term_meta;
import byte_slice;
import posting_list_format;
import internal_types;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag)
    : base_row_id_(base_row_id) {
    Path path = Path(index_dir) / base_name;
    String path_str = path.string();
    String dict_file = path_str;
    dict_file.append(DICT_SUFFIX);
    dict_reader_ = MakeShared<DictionaryReader>(dict_file, PostingFormatOption(flag));
    String posting_file = path_str;
    posting_file.append(POSTING_SUFFIX);
    posting_reader_ = MakeShared<FileReader>(fs_, posting_file, 1024);
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {}

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta))
        return false;
    posting_reader_->Seek(term_meta.doc_start_);
    u64 file_length = term_meta.pos_end_ - term_meta.doc_start_;
    ByteSlice *slice = ByteSlice::CreateSlice(file_length, session_pool);
    posting_reader_->Read((char *)slice->data_, file_length);
    SharedPtr<ByteSliceList> byte_slice_list = MakeShared<ByteSliceList>(slice, session_pool);
    seg_posting.Init(byte_slice_list, base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}

} // namespace infinity
