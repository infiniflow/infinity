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

#include <cassert>
#include <sys/mman.h>

module disk_index_segment_reader;

import stl;

import segment_posting;
import index_defines;
import index_segment_reader;
import file_reader;
import dict_reader;
import term_meta;
import byte_slice;
import posting_list_format;
import internal_types;
import third_party;
import byte_slice_reader;
import infinity_exception;
import status;
import logger;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag)
    : base_row_id_(base_row_id) {
    Path path = Path(index_dir) / base_name;
    String path_str = path.string();
    String dict_file = path_str;
    dict_file.append(DICT_SUFFIX);
    dict_reader_ = MakeShared<DictionaryReader>(dict_file, PostingFormatOption(flag));
    posting_file_ = path_str;
    posting_file_.append(POSTING_SUFFIX);
    int rc = fs_.MmapFile(posting_file_, data_ptr_, data_len_);
    assert(rc == 0);
    if (rc != 0) {
        Status status = Status::MmapFileError(posting_file_);
        RecoverableError(status);
    }
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {
    int rc = fs_.MunmapFile(posting_file_);
    assert(rc == 0);
    if (rc != 0) {
        Status status = Status::MunmapFileError(posting_file_);
        RecoverableError(status);
    }
}

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, SegmentPosting &seg_posting, bool fetch_position) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }
    u64 file_length = fetch_position ? (term_meta.pos_end_ - term_meta.doc_start_) : (term_meta.pos_start_ - term_meta.doc_start_);
    ByteSlice *slice = ByteSlice::NewSlice(data_ptr_ + term_meta.doc_start_, file_length);
    SharedPtr<ByteSliceList> byte_slice_list = MakeShared<ByteSliceList>(slice);
    seg_posting.Init(std::move(byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}

} // namespace infinity
