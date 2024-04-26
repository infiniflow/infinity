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
import third_party;
import byte_slice_reader;

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

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool, bool fetch_position) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }
    u64 file_length = term_meta.pos_start_ - term_meta.doc_start_;
    if (fetch_position) {
        file_length = term_meta.pos_end_ - term_meta.doc_start_;
    }
    ByteSlice *slice = ByteSlice::CreateSlice(file_length, session_pool);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)slice->data_, file_length);

        ByteSliceReader byte_slice_reader;
        byte_slice_reader.Open(slice);
    }
    SharedPtr<ByteSliceList> byte_slice_list = MakeShared<ByteSliceList>(slice, session_pool);
    seg_posting.Init(std::move(byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}

bool DiskIndexSegmentReader::GetSegmentPostingBack(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool, bool fetch_position) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }

    u64 file_length = term_meta.pos_end_ - term_meta.doc_start_;
    u64 doc_header_length = sizeof(u32) * 2;
    u64 pos_header_length = sizeof(u32) * 2;

    ByteSlice *doc_header_slice = ByteSlice::CreateSlice(doc_header_length, session_pool);
    ByteSlice *pos_header_slice = ByteSlice::CreateSlice(pos_header_length, session_pool);
    ByteSlice *doc_slice = nullptr;
    ByteSlice *pos_slice = nullptr;
    u64 pos_begin = -1;
    u64 pos_size = -1;
    {
        std::lock_guard<std::mutex> lock(mutex_);

        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)doc_header_slice->data_, doc_header_length);

        ByteSliceReader doc_byte_slice_reader;
        doc_byte_slice_reader.Open(doc_header_slice);
        auto doc_begin = doc_byte_slice_reader.Tell();
        auto doc_skiplist_size = doc_byte_slice_reader.ReadVUInt32();
        auto doc_list_size = doc_byte_slice_reader.ReadVUInt32();
        auto doc_header_real_length = doc_byte_slice_reader.Tell() - doc_begin;
        auto doc_size = doc_header_real_length + doc_skiplist_size + doc_list_size;
        doc_slice = ByteSlice::CreateSlice(doc_size, session_pool);
        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)doc_slice->data_, doc_size);
        pos_begin = doc_byte_slice_reader.Tell() + doc_skiplist_size + doc_list_size;
        pos_size = file_length - doc_size;
        if (fetch_position) {
            posting_reader_->Seek(term_meta.doc_start_ + pos_begin);
            posting_reader_->Read((char *)pos_header_slice->data_, pos_header_length);
            ByteSliceReader pos_byte_slice_reader;
            pos_byte_slice_reader.Open(pos_header_slice);
            auto pos_byte_slice_begin = pos_byte_slice_reader.Tell();
            auto pos_skiplist_size = pos_byte_slice_reader.ReadVUInt32();
            auto pos_list_size = pos_byte_slice_reader.ReadVUInt32();
            auto pos_header_real_length = pos_byte_slice_reader.Tell() - pos_byte_slice_begin;
            auto pos_size_cal = pos_header_real_length + pos_skiplist_size + pos_list_size;
            assert(pos_size_cal + doc_size == file_length);
            pos_slice = ByteSlice::CreateSlice(pos_size_cal, session_pool);
            posting_reader_->Seek(term_meta.doc_start_ + pos_begin);
            posting_reader_->Read((char *)pos_slice->data_, pos_size_cal);
        }
    }
    SharedPtr<ByteSliceList> doc_byte_slice_list = MakeShared<ByteSliceList>(doc_slice, session_pool);

    SharedPtr<ByteSliceList> pos_byte_slice_list = nullptr;
    if (fetch_position) {
        pos_byte_slice_list = MakeShared<ByteSliceList>(pos_slice, session_pool);
    }
    seg_posting.Init(std::move(doc_byte_slice_list),
                     std::move(pos_byte_slice_list),
                     base_row_id_,
                     term_meta.doc_freq_,
                     term_meta,
                     pos_begin,
                     pos_size,
                     posting_reader_,
                     session_pool);
    return true;
}

} // namespace infinity
